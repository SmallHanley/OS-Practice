CFLAGS = -g -Wall -std=c++11
CC = g++
INPUT = ../hw3_test/hw3example.input

kvs: kvs.o hash_table.o
	$(CC) -o $@ $^ 
	./kvs $(INPUT)

run:
	./kvs $(INPUT)

kvs.o: kvs.cpp hash_table.h
	$(CC) $(CFLAGS) -c $<

hash_table.o: hash_table.cpp hash_table.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) kvs *.output *.o

rmtmp:
	$(RM) storage/*.tmp
