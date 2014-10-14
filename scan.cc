#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
    FILE *f = fopen(heapfile_name, "rb+");
    fread(heapfile, sizeof(Heapfile), 1, f);
    heapfile->file_ptr = f;
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    uint32_t count = 0;
    RecordIterator *i = new RecordIterator(heapfile);
    while (i->hasNext()) {
        count++;
        cout << "pageID " << i->cur_rid->page_id;
        cout << ", slot " << i->cur_rid->slot << ": ";
        Record record = i->next();
        for (int j = 0; j < record.size(); j++) {
            cout << record.at(j);
            if (j != record.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    cout << "Total number of records: " << count << endl;
    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
}
