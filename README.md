sequential-parallel-1
=====================

CSE520 Architecture II Programming Assignment 1
Compilation and testing instructions:

Unpacking instructions: 
1) untar the source and report with the command: 
	tar xvf Assignment1_Welch_Matt.tar.bz2
2) navigate to the Asignment1/ folder

Matrix Multiplication: 
1) Navigate to the directory MatrixMultiply/
2) Run "make" to build the serial and paralell programs
3) execute the serial version with ./matmult DIM RUNS, 
	where DIM is the matrix dimension to calculate (default=840)
	and RUNS is the number of runs to perform for averaging (default=3)
4) execute the parallel version with ./matmultpar DIM NTHREADS RUNS
	where DIM is the matrix dimension to calculate (default=840)
	NTHREADS is the number of threads to use (default=4)
	and RUNS is the number of runs to perform for averaging (default=3)
5) alternatively, run ./test_script.sh to measure an array of matrix dimensions
	across 2 to 8 threads in addition to serial performance

Mandelbrot Threads: 
1) navigate to Mandelbrot/mandelbrot_threads
2) run "make" to build the parallel program
3) run ./mandelbrot_threads NTHREADS
	where NTHREADS is the desired number of threads
4) alternatively, run the test script ./test_script.sh to execute with the 
	full range of threads

Mandelbrot ispc: 
NOTE: Mandelbrot ispc assumes you have the ispc compiler in your path
1) navigate to Mandelbrot/mandelbrot_threads
2) run "make" to build the parallel program
3) run ./mandelbrot to see the speedup enabled by AVX extensions
4) alternatively, edit the Makefile to change from the -x2 targets to see 
	the difference between AVX and AVX-x2

Mandelbrot tasks: 
NOTE: Mandelbrot ispc assumes you have the ispc compiler in your path
1) navigate to the Mandelbrot/mandelbrot_tasks/ folder
2) edit the file mandelbrot.ispc to choose a particular task size, 
	this value is typically between 1 and 1024, but less than 128 should be
	fairly constant performance
3) run ./mandelbrot to run the serial and parallel with tasks versoins 
	of the program

nQueens
1) navigate to the nQueen/ directory 
2) run "make" to build the threads version of the program
3) run nQueen 0 N to run the serial version with N queens
4) run nQueen 1 N D to run the threads version with N queens 
	at parallel depth D
5) the tasks verison of the program is non funcional
