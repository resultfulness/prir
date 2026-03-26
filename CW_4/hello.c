#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main (int argc, char *argv[])
{
  int nthreads, tid;
  int n = 5;

  omp_set_num_threads(8);
  /* Fork a team of threads giving them their own copies of variables */
  #pragma omp parallel private(nthreads, tid) if(n >=4)
  {

    /* Obtain thread number */
    tid = omp_get_thread_num();
    printf("Hello World from thread = %d\n", tid);

    /* Only master thread does this */
    if (tid == 0)
      {
        nthreads = omp_get_num_threads();
        printf("Number of threads = %d\n", nthreads);
      }

   }  /* All threads join master thread and disband */

}

