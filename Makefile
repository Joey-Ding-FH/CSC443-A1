#Makefile

CC = g++
ALL = csv2heapfile scan csv2colstore

library.o: library.cc library.h
	$(CC) -o $@ -c $<

%: 
	$(CC) -o $* $*.cc  library.o

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o

scan: scan.cc library.o
	$(CC) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) -o $@ $< library.o

clean:
	rm -f $(ALL) *.o
