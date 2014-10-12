#Makefile

CC = g++

library.o: library.cc library.h
	$(CC) -o $@ -c $<

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o

scan: scan.cc library.o
	$(CC) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) -o $@ $< library.o

clean:
	rm -f csv2colstore *.o
