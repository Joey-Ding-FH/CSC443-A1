#ifndef CSV2HEAPFILE_H
#define CSV2HEAPFILE_H

#include "library.h"

using namespace std;

typedef int PageID;

typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;
 
typedef struct {
    int page_id;
    int slot;
} RecordID;

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

// TODO - delete this.
uint32_t read_offset(FILE *file);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

#endif