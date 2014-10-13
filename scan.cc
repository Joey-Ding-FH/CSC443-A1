#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);
void scan(char *heapfile_name, int page_size);

int main(int argc, char *argv[]) {
    check_argv(argc, argv);

    char *heapfile_name = argv[1];
    int page_size = atoi(argv[2]);

    scan(heapfile_name, page_size);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 3) {
        fputs("usage: scan <heapfile> <page_size>\n",stderr);
        exit(2);
    }

    if (atoi(argv[2]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}

/**
 * Scan all records in heapfile using the given page_size.
 */
void scan(char *heapfile_name, int page_size) {
    Heapfile *heapfile = new Heapfile;
    heapfile->page_size = page_size;
    heapfile->file_ptr = fopen(heapfile_name, "rb");
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    RecordIterator *i = new RecordIterator(heapfile);
    while (i->hasNext()) {
        char *buf = (char *) malloc(SLOT_SIZE);
        Record record = i->next();
        for (int i = 0; i < record.size(); i++) {
            cout << record.at(i) << ", ";
        }
        cout << endl;
    }
    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
}
