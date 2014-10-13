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
    int attr_id = atoi(argv[3]);
    char *new_value = argv[4];
    int page_size = atoi(argv[5]);

    // Initialize heap file.
    Heapfile *heapfile = new Heapfile;
    heapfile->page_size = page_size;
    heapfile->file_ptr = fopen(heapfile_name, "rb+");
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    Page *page = new Page;
    
    read_page(heapfile, pid, page);

    Record *record = new Record;
    read_fixed_len_page(page, slot, record);

    read_attr(record, attr_id, new_value);

    write_fixed_len_page(page, slot, record);

    write_page(page, heapfile, pid);

    delete page;
    delete record;
    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
    free(argv2);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 6) {
        fputs("usage: update <heapfile> <page_id>-<slot> <attribute_id> <new_value> <page_size>\n",stderr);
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

    if ((atoi(slot) <= 0) && !(strcmp(slot, "0") == 0)) {
        fputs("usage: <slot> must be integer and greater than or equal to zero\n",stderr);
        exit(2);
    }

    if ((atoi(argv[3]) <= 0 or atoi(argv[3]) >= ATTR_PER_RECORD) && !(strcmp(argv[3], "0") == 0)) {
        fprintf(stderr, "usage: <attribute_id> must be integer and greater than or equal to zero and smaller than number of attribute which is %d \n", ATTR_PER_RECORD);
        exit(2);
    }

    if (atoi(argv[5]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}