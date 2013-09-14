#/**************************************************
# * FILENAME:    Makefile
# * DESCRIPTION: build options for MatrixMultply 
# * AUTHOR:      James Matthew Welch [JMW]
# * SCHOOL:      Arizona State University
# * CLASS:       CSE520: Computer Architecture II
# * INSTRUCTOR:  Dr. Carole-Jean Wu
# * SECTION:     83380
# * TERM:        Fall 2013
# **************************************************/

all: serial parallel

serial: matmult.o
	gcc MatrixMultiply.c -o matmult $(CONFIG)

parallel: matmultpar.o
	gcc MatMultPar.c -o matmultpar $(CONFIG)


matmult.o: MatrixMultiply.c
	gcc -c MatrixMultiply.c -o matmult.o $(CONFIG)

matmultpar.o: MatMultPar.c
	gcc -c MatMultPar.c -o matmultpar.o $(CONFIG)

clean:
	rm -f matmult matmultpar *.o core core.*
	ctags --c-kinds=+defglmpstux -R

tidy: clean
	rm -f *.*~ *~

DEBUG_FLAGS = -g3 -ggdb -O0 -Wall -pedantic -DDEBUG -lpthread
CONFIG		= -Wall -pedantic -lpthread -O3
EASY_FLAGS	= -lpthread

debug: CONFIG=$(DEBUG_FLAGS)
debug: serial parallel

easy: CONFIG=$(EASY_FLAGS)
easy: serial parallel

test: debug
	tests
	testp

tests: debug
	./matmult

testp: debug
	./matmultpar

run: serial parallel
	./matmultpar 256 4  0
	./matmult 256 0
