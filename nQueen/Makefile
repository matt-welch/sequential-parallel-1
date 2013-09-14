#/************************************************************
# * FILENAME:    Makefile
# * DESCRIPTION: build options for nQueen 
# * AUTHOR:      James Matthew Welch [JMW]
# * SCHOOL:      Arizona State University
# * CLASS:       CSE520: Computer Architecture II
# * INSTRUCTOR:  Dr. Carole-Jean Wu
# * SECTION:     83380
# * TERM:        Fall 2013
# ***********************************************************/

nqueen: nqueen.o
	gcc -o nqueen nqueen.o -lpthread

nqueen.o: nqueen_corrected.c
	gcc -o nqueen.o -c nqueen_corrected.c $(CONFIG)
	
clean:
	rm -f nqueen *.o core core.*
	ctags --c-kinds=+defglmpstux -R

tidy: clean
	rm -f *.*~ *~ output

DEBUG_FLAGS= -g3 -ggdb -O0 -Wall -pedantic -DDEBUG -lpthread
CONFIG= -lpthread

hard: CONFIG= -lpthread -Wall -pedantic
hard: nqueen

debug: CONFIG=$(DEBUG_FLAGS)
debug: clean nqueen

run: nqueen
	./nqueen 1 8 1

