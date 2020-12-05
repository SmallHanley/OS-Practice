CFLAGS = -g -Wall -std=c++11
CC = g++
CFLAGS = -g -Wall -std=c++11
INPUT = ../hw3_test/hw3example.input

all: kvs
	./kvs $(INPUT)

kvs: kvs.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) kvs *.output

rmtmp:
	$(RM) storage/*.tmp
