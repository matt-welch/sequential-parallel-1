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

serial: MatrixMultiply.c
	gcc MatrixMultiply.c -o matmult $(CONFIG)

parallel: MatMultPar.c
	gcc MatMultPar.c -o matmultpar $(CONFIG)

clean:
	rm -f matmult matmultpar *.o core core.*

tidy: clean
	rm -f *.*~ *~

DEBUG_FLAGS = -g3 -ggdb -O0 -Wall -pedantic -DDEBUG -lpthread
CONFIG		= -Wall -pedantic -lpthread -O2
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

