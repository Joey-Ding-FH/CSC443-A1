#ifndef CSV2HEAPFILE_H
#define CSV2HEAPFILE_H

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

uint32_t read_offset(FILE *file);

#endif