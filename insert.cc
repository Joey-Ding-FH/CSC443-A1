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
    char *csv_file;
    char *heapfile_name;
    int page_size;

    check_argv(argc, argv);

    heapfile_name = (char *) malloc(sizeof(char) * strlen(argv[1]));
    strcpy(heapfile_name, argv[1]);

    csv_file = (char *) malloc(sizeof(char) * strlen(argv[2]));
    strcpy(csv_file, argv[2]);

    page_size = atoi(argv[3]);

    // Initialize heap file.
    Heapfile *heapfile = new Heapfile;
    heapfile->page_size = page_size;
    heapfile->file_ptr = fopen(heapfile_name, "rb+");
    if (heapfile->file_ptr == NULL) {
        fputs("heap file doesn't exist.\n", stderr);
        exit(2);
    }

    if (!ifstream(csv_file))
    {
        fputs("csv file doesn't exist.\n", stderr);
        exit(2);
    } 
    ifstream file(csv_file);
    int pid = 1;
    char chars_to_remove[] = ",\"";
    string line;
    Page *cur_page = new Page;
    read_page(heapfile, pid, cur_page);
    
    while (getline(file, line)) {
        for (int i = 0; i < strlen(chars_to_remove); ++i) {
            line.erase(remove(line.begin(), line.end(), chars_to_remove[i]), line.end());
        }
        Record *record = new Record;
        fixed_len_read((void *) line.c_str(), SLOT_SIZE, record);

        if (cur_page->data == NULL) {
            alloc_page(heapfile);
        }

        if (fixed_len_page_freeslots(cur_page) > 0) {
            add_fixed_len_page(cur_page, record);
        } else {
            free(cur_page->data);
            cur_page = new Page;
            write_page(cur_page, heapfile, pid);
            pid++;
            read_page(heapfile, pid, cur_page);

            if (cur_page->data == NULL) {
                alloc_page(heapfile);
            }

            add_fixed_len_page(cur_page, record);
        }
    }
    write_page(cur_page, heapfile, pid);

    fflush(heapfile->file_ptr);
    fclose(heapfile->file_ptr);
    file.close();
}

void check_argv(int argc, char *argv[]) {
    if(argc != 4) {
        fputs("usage: insert <heapfile> <csv_file> <page_size>\n",stderr);
        exit(2);
    }

    if (atoi(argv[3]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}