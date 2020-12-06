CFLAGS = -g -Wall -std=c++11
CC = g++
INPUT = ../hw3_test/hw3example.input

all: kvs.o hash_table.o
	$(CC) -o $^ 
	./kvs $(INPUT)

kvs.o: kvs.cpp
	$(CC) $(CFLAGS) -c $<

hash_table.o: hash_table.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) kvs *.output

rmtmp:
	$(RM) storage/*.tmp
