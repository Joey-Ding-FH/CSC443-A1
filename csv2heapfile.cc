#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    check_argv(argc, argv);

    char *csv_file = argv[1];
    char *heapfile_name = argv[2];
    int page_size = atoi(argv[3]);

    // Initialize heap file.
    Heapfile *heapfile = new Heapfile;
    init_heapfile(heapfile, page_size, fopen(heapfile_name , "wb+r"));

    if (!ifstream(csv_file))
    {
        fputs("csv file doesn't exist.\n", stderr);
        exit(2);
    }
    ifstream file(csv_file);
    int pid = 1;
    
    while (!file.eof()) {
        alloc_page(heapfile);
        Page *page = (Page *) malloc(sizeof(Page));

        read_page(heapfile, pid, page);
        read_csv2page(&file, page);
        write_page(page, heapfile, pid);

        free(page);
        pid++;
    }

    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
    file.close();
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
