#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    char *argv2 = (char *) malloc(strlen(argv[2]) * sizeof(char));
    strcpy(argv2, argv[2]);
    check_argv(argc, argv);

    char *heapfile_name = argv[1];
    int pid = atoi(strtok(argv2, "-"));
    int slot = atoi(strtok(NULL, "-"));
    int page_size = atoi(argv[3]);

    //start timer
    clock_t start = clock();

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

    Page *page = new Page;
    if (pid > heapfile->number_of_page) {
        fputs("record with <record_id> doesn't exist.\n", stderr);
        exit(2);
    }
    read_page(heapfile, pid, page);
    
    if (slot >= fixed_len_page_capacity(page)) {
        fputs("record with <record_id> doesn't exist.\n", stderr);
        exit(2);
    }
    page->slot_info->at(slot) = '0';

    write_page(page, heapfile, pid);

    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
    free(argv2);

    int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "TIME: %d milliseconds\n", msecTime);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 4) {
        fputs("usage: delete <heapfile> <page_id>-<slot> <page_size>\n",stderr);
        exit(2);
    }

    char *pid = strtok(argv[2], "-");
    char *slot = strtok(NULL, "-");

    if (pid == NULL || slot == NULL) {
        fputs("usage: <record_id> format should be <page_id>-<slot>\n",stderr);
        exit(2);
    }
    if (atoi(pid) <= 0) {
        fputs("usage: <page_id> must be integer and greater than zero\n",stderr);
        exit(2);
    }

    if (atoi(argv[3]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}