#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);
void select(char *heapfile_name, int page_size, int attr_id, char *start, char *end);

int main(int argc, char *argv[]) {
    check_argv(argc, argv);

    char *heapfile_name = argv[1];
    int attr_id = atoi(argv[2]);
    char *start = argv[3];
    char *end = argv[4];
    int page_size = atoi(argv[5]);

    //start timer
    clock_t start_timer = clock();

    // Initialize heap file.
    Heapfile *heapfile = new Heapfile;
    FILE *f = fopen(heapfile_name, "rb+");
    fread(heapfile, sizeof(Heapfile), 1, f);
    heapfile->file_ptr = f;
    heapfile->page_size = page_size;
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    select(heapfile_name, page_size, attr_id, start, end);

    fclose(f);

    int msecTime = (clock() - start_timer) * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "TIME: %d milliseconds\n", msecTime);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 6) {
        fputs("usage: select <heapfile> <attribute_id> <start> <end> <page_size>\n",stderr);
        exit(2);
    }

    if ((atoi(argv[2]) <= 0 or atoi(argv[2]) >= ATTR_PER_RECORD) && !(strcmp(argv[2], "0") == 0)) {
        fprintf(stderr, "usage: <attribute_id> must be integer and greater or equal to zero and smaller than number of attribute which is %d \n", ATTR_PER_RECORD);
        exit(2);
    }

    if (atoi(argv[5]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}

/**
 * Select all records in heapfile using the given page_size.
 */
void select(char *heapfile_name, int page_size, int attr_id, char *start, char *end) {
    Heapfile *heapfile = new Heapfile;
    FILE *f = fopen(heapfile_name, "rb+");
    fread(heapfile, sizeof(Heapfile), 1, f);
    heapfile->file_ptr = f;
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    RecordIterator *i = new RecordIterator(heapfile);
    while (i->hasNext()) {
        char *buf = (char *) malloc(SLOT_SIZE);
        Record record = i->next();
        char *attr = (char *) malloc(ATTRIBUTE_SIZE);
        write_attr(&record, attr_id, attr);

        int compare_start = strcmp(start, attr);
        int compare_end = strcmp(end, attr);

        if (compare_start <= 0 && compare_end >= 0) {
            if (ATTRIBUTE_SIZE > 5) {
                attr[5] = '\0';
            }
            cout << attr << endl;
        }
    }
    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
}