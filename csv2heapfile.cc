#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    check_argv(argc, argv);

    char *csv_file = argv[1];
    char *heapfile_name = argv[2];
    int page_size = atoi(argv[3]);

    //start timer
    clock_t start = clock();

    // Initialize heap file.
    Heapfile *heapfile = new Heapfile;
    init_heapfile(heapfile, page_size, fopen(heapfile_name , "rb+"));

    if (!ifstream(csv_file))
    {
        fputs("csv file doesn't exist.\n", stderr);
        exit(2);
    }
    ifstream file(csv_file);
    int pid;
    int old_pid = 0;
    while (1) {
        Page *page = (Page *) malloc(sizeof(Page));
        init_fixed_len_page(page, page_size, SLOT_SIZE);

        read_csv2page(&file, page);
        if (file.eof()) {
            free(page->data);
            free(page);
            break;
        }
        pid = alloc_page(heapfile);
        write_page(page, heapfile, pid);

        free(page->data);
        free(page);
    }
    cout << "numer of page is " << pid << endl;

    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
    file.close();

    int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "TIME: %d milliseconds\n", msecTime);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 4) {
        fputs("usage: csv2heapfile <csv_file> <heapfile> <page_size>\n",stderr);
        exit(2);
    }

    if (atoi(argv[3]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}
