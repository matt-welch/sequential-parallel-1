/**********************************************************
 * FILENAME:    MatMultPar.c
 * DESCRIPTION: C-source for matrix multiply - parallel 
 * AUTHOR:      James Matthew Welch [JMW]
 * SCHOOL:      Arizona State University
 * CLASS:       CSE520: Computer Architecture II
 * INSTRUCTOR:  Dr. Carole-Jean Wu
 * SECTION:     83380
 * TERM:        Fall 2013
 *********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

/* change d size as needed */
int dimension = 1024;

int main(int argc, char *argv[])
{
	int d, i, j, k;
	double *A, *B, *C;
	struct timeval begin, end;

	if (argc == 2)
		dimension = atoi(argv[1]);

	printf("Dimension = %d\n", dimension);

	srand(292);
	for (d = dimension ; d < dimension+1; d=d+128 ) {
		int runs = 3;
		for ( ; runs>=0; runs--) {
			A = (double*)malloc(d*d*sizeof(double));
			B = (double*)malloc(d*d*sizeof(double));
			C = (double*)malloc(d*d*sizeof(double));
			gettimeofday(&begin, NULL); /* returns the wall clock time */


			for(i = 0; i < d; i++) {
				for(j = 0; j < d; j++) {
					A[d*i+j] = (rand()/(RAND_MAX - 1.0));
					B[d*i+j] = (rand()/(RAND_MAX - 1.0));
					C[d*i+j] = 0.0;
				}
			}

			gettimeofday(&end, NULL);
			printf("[%d/%d] init loop took %ldus\t", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));
			
			gettimeofday(&begin, NULL);
			for(i = 0; i < d; i++) {
				for(j = 0; j < d; j++) {
					for(k = 0; k < d; k++) {
						C[d*i+j] += A[d*i+k] * B[d*k+j];
					}
				}
			}
			gettimeofday(&end, NULL);
			printf("[%d/%d] %ldus\n", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));
			
			free(A);
			free(B);
			free(C);
		}
		printf("\n");
	}

	return 0;
}
