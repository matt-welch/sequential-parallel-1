#/************************************************************
# * FILENAME:    Makefile
# * DESCRIPTION: build options for mandlebrot_threads (part 1) 
# * AUTHOR:      James Matthew Welch [JMW]
# * SCHOOL:      Arizona State University
# * CLASS:       CSE520: Computer Architecture II
# * INSTRUCTOR:  Dr. Carole-Jean Wu
# * SECTION:     83380
# * TERM:        Fall 2013
# ***********************************************************/

mandelbrot_threads: mandelbrot.o mandelbrot_serial.o mandelbrot_threads.o
	g++ -o mandelbrot_threads mandelbrot.o mandelbrot_serial.o mandelbrot_threads.o -lpthread

mandelbrot.o: mandelbrot.cpp ../timing.h
	g++ -c $(CONFIG) mandelbrot.cpp 

mandelbrot_serial.o: mandelbrot_serial.cpp
	g++ -c $(CONFIG) mandelbrot_serial.cpp

mandelbrot_threads.o: mandelbrot_threads.cpp
	g++ -c $(CONFIG) mandelbrot_threads.cpp

clean:
	rm -f mandelbrot_threads *.o core core.*
	ctags --c-kinds=+defglmpstux -R

tidy: clean
	rm -f *.*~ *~ *.ppm

DEBUG_FLAGS= -g3 -ggdb -O0 -Wall -pedantic -DDEBUG -lpthread
CONFIG= -Wall -pedantic -lpthread

debug: CONFIG=$(DEBUG_FLAGS)
debug: clean mandelbrot_threads

fast: CONFIG=-O2 -lpthread 
fast: clean mandelbrot_threads

run: mandelbrot_threads
	./mandelbrot_threads

show: run
	eog mandelbrot-thread.ppm &
