/*
* csim.c - A cache simulator that can replay traces from Valgrind and output statistics such as number
* of hits, misses, and evictions. The replacement policy is LRU (least recently used).
*
* Implementation assumptions:
* 1. Each load/store can cause at most one cache miss.
* 2. Instruction loads (I) are ignored since we are interested in evaluating trans.c in terms of its data
* cache performance.
* 3. Data modify (M) is treated as a load followed by a store to the same address. Therefore, an M
* operation can result in two cache hits, or a miss and a hit plus a possible eviction.
*/

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "cachelab.h"

#define ADDRESS_LENGTH 64

/////////////////////////////////////////////////////////////////////////////////////////////////////

// Globals set by command line args

//print trace
int verbosity = 0;
//index bits
int s = 0;
//block offset bits
int b = 0;
//associativity bits
int E = 0;
char* trace_file = NULL;
//total number of set bits (2^s)
int S;
//total block size (2^b)
int B;

//counters for cache stats
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////

/*
* Type: Memory address
*/
typedef unsigned long long int mem_addr_t;

/*
* Type: Cache line
*
* A structure for representing a line in a cache, contains a counter for tracking line use
*/
typedef struct cache_line {
  char valid;
  mem_addr_t tag;
  int counter;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;

// The cache that we are simulating
cache_t cache;

/*
* initCache - Allocate proper data structures to hold info regarding the sets and cache lines.
*/
void initCache()  {
  
  S = pow(2, s);
  cache = malloc(S * sizeof(cache_set_t));
  
  //iterate through the sets, allocate space for lines
  for(int i = 0; i < S; i++)  {
    cache[i] = malloc(E * sizeof(cache_line_t));
    for(int j = 0; j < E; j++)  {
      cache[i][j].valid = '0';
      cache[i][j].tag = 0;
      cache[i][j].counter = 0;
    }
  }
}

/*
* freeCache - Free each piece of memory
*/
void freeCache()  {
  
  //recalculate in case initCache() not called
  S = pow(2,s);
  //iterate through each set
  for(int i = 0; i < S; i++)  {
    free(cache[i]);
  }
  
  free(cache);
}

/*
* accessData - Access data at given memory address addr.
* If alreayd in cache, increase hit_count.
* If not, bring it into cache, increase miss count.
* Increase eviction count if necesary.
*/
void accessData(mem_addr_t addr)  {
  
  //mask address to isolate the set and tag bits
  mem_addr_t mask = pow(2, s + b);
  mask--;
  
  //set proper bits
  mask -= (pow(2,b) - 1);
  
  //isolate set number
  mem_addr_t targSet = addr & mask;
  targSet = targSet >> b;
  
  //extract tag
  mem_addr_t targTag = addr >> (s + b);
  
  //set values to arbitrary values for finding desired lines
  int found = 0;
  int min = 5000;
  int max = 0;
  int minIndex = -1;
  
  //iterate through all sets and find min and max counters
  for(int i = 0; i < E; i++)  {
    
    //if it's the new min
    if(cache[targSet][i].counter < min) {
      min = cache[targSet][i].counter;
      minIndex = j;
      }
    
    //if it's the new max
    if(cache[targSet][i].counter > max) {
      max = cache[targSet][i].counter;
      }
    }
   
   //iterate through and find the line, if possible
   for(int i = 0; i < E; i++) {
    
    //if it's valid and the correct tag, increment 
    if(cache[targSet][i].valid == '1' && cache[targSet][i].tag == targTag)  {
      found = 1;
      hit_count++;
      cache[targSet][i].counter = max + 1;
      break;
    }
  }
  
  //if no hits
  if(found == 0)  {
  
    miss_count++;
    int foundEmpty = 0;
    
    //iterate through to find any that are empty
    for(int k = 0; k < E; k++)  {
      if(cache[targSet][k].valid == '0')  {
      
        //set that to the target
        cache[targSet][k].valid = '1';
        cache[targSet][k].tag = targTag;
        cache[targSet][k].counter = max + 1;
        
        foundEmpty = 1;
        break;
        }
      }
      
      //if no empty ones have been found, mark as an eviction
      if(foundEmpty == 0) {
        eviction_count++;
        
        //evict the minimum counter value line
        cache[targSet][minIndex].tag = targTag;
        cache[targSet][minIndex].counter = max + 1;
        }
      }
}

/*
* replayTrace - replays the given trace file against the cache
*/
void replayTrace(char* trace_fn)  {
 
  //read trace file, make successive calls
  char buf[1000];
  mem_addr_t addr = 0;
  unsigned int len = 0;
  FILE* trace_fp = fopen(trace_fn, "r");
  
  //error catch
  if(!trace_fp) {
    fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
    exit(1);
  }
  
  //handle all calls in the buffer
  while(fgets(buf, 1000, trace_fp) != NULL) {
    if(buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
      sscanf(buf+3, "%llx,%u", &addr, &len);
      
      if(verbosity)
        printf("%c %llx,%u ", buf[1], addr, len);
      
      //if it's a load
      if(buf[1] == 'L')
        accessData(addr);
      
      //if it's a store
      else if(buf[1] == 'S')
        accessData(addr);
      
      //otherwise, do data move and access
      else if(buf[1] == 'M')  {
        accessData(addr);
        accessData(addr);
      }
        
      if(verbosity)
        printf("\n");
    }
  }
  
  fclose(trace_fp);
}

/*
* printUsage - Print usage info
*/
void printUsage(char* argv[]) {
  
  char c;
  //Parse the command line arguments: -h, -v, -s, -E, -b, -t
  while((c=getopt(argc,argv,"s:e:b:t:vh")) != -1) {
    switch(c) {
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        trace_file = optarg;
        break;
      case 'v':
        verbosity = 1;
        break;
      case 'h':
        printUsage(argv);
        exit(0);
      default:
        printUsage(argv);
        exit(1);
    }
  }
  
  //make sure that all the required command line args were specified
  if(s == 0 || E == 0 || b == 0 || trace_file NULL) {
    printf("%s: Missing required command line argument\n", argv[0]);
    printUsage(argv);
    exit(1);
  }
  
  initCache();

  //final print and wipe 
 #ifdef DEBUG_ON
   printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
 #endif
   replayTrace(trace_file);
  
  //free allocated memory
  freeCache();
  
  //output hit and miss stats
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}
