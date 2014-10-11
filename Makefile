#Makefile

CC = g++

library.o: library.cc library.h
	$(CC) -o $@ -c $<

