/*
* cachelab.c - Cache Lab helper functions for performing tests
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cachelab.h"
#include <time.h>

//initialize
trans_func_t func_list[MAX_TRANS_FUNCS];
int func_counter = 0;

/*
* printSummary - Summarizes all of the cache simulation statistics.
*/
void printSummary(int hits, int misses, int evictions) {

  printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
  //try to print to a file
  FILE* output_fp = fopen(".csim_results", "w");
  assert(output_fp);
  fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
  fclose(output_fp);
}

/*
* initMatrix - Initialize matrix of desired size
*/
void initMatrix(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;
  srand(time(NULL));
  //fill the matrix with randomly generated values
  for(i = 0; i < N; i++) {
    for(j = 0; j < M; j++) {
      A[i][j] = rand();
      B[j][i] = rand();
    }
  }
}

void randMatrix(int M, int N, int A[N][M]) {
  int i, j;
  srand(time(NULL));
  //fill with randomly generated values
  for(i = 0; i < N; i++) {
    for(j = 0; j < M; j++) {
      A[i][j] = rand();
    }
  }
}
    
/*
* correctTrans - baseline transpose function to evaluate correctness
*/
void correctTrans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;
  //transpose all values in matrix (flip)
  for(i = 0; i < N; i++)  {
    for(j = 0; j < M; j++)  {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
* registerTransFunction - Add the given trans function into your list of functions to test
*/
void registerTransFunction(void (*trans)(int M, int N, int[N][M], int[M][N]), char* desc) {
  func_list[func_counter].func_ptr = trans;
  func_list[func_counter].description = desc;
  //initialize values
  func_list[func_counter].correct = 0;
  func_list[func_counter].num_hits = 0;
  func_list[func_counter].num_misses = 0;
  func_list[func_counter].num_evictions = 0;
  func_counter++
}
