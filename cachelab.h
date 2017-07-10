 /*
 * cachelab.h - Creates prototypes for Cachelab helper functions in performing tests.
 */
 
 #ifndef CACHELAB_TOOLS_H
 #define CACHELAB_TOOLS_H
 #define MAX_TRANS_FUNCS 100
 
 typedef struct trans_func {
  void (*func_ptr) (int M, int N, int[N][M], int[M][N]);
  char* description;
  char correct;
  unsigned int num_hits;
  unsigned int num_misses;
  unsigned int num_evictions;
} trans_func_t;

/*
* printSummary - Provides a standard way for the cache to display its final hit and miss statistics
*/

void printSummary (int hits, int misses, int evictions);

//Fill the matrix
void initMatrix(int M, int N, int A[N][M], int B[M][N]);

//Baseline trans function that produces correct results
void correctTrans(int M, int N, int A[N][M], int B[M][N]);

//Add the given function to the function list
void registerTransFunction(
  void (*trans)(int M, int N, int[N][M], int[M][N]), char* desc);
  
#endif /* CACHELAB_TOOLS_H */
