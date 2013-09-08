/* structure to pass parameters to a processor thread */
typedef struct _thread_parameters{
	int tid;
	float x0;
	float x1;
	float y0;
	float y1;
	int rowsPerThread;
	int width;
	int maxIterations;
	int *output;
} thread_parameters;

