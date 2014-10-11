#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>

using namespace std;

#define OFFSET_SIZE sizeof(uint32_t)
#define ATTRIBUTE_SIZE 10
#define ATTR_PER_RECORD 100
#define SLOT_SIZE ATTRIBUTE_SIZE * ATTR_PER_RECORD


typedef const char* V;
typedef vector<V> Record;
typedef int PageID;
typedef vector<char> ByteArray;

typedef struct {
    void *data;
    int page_size;
    int slot_size;
    ByteArray *slot_info; //byte array to store info about slots: 0 if free 1 if not
} Page;

typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;
 
typedef struct {
    int page_id;
    int slot;
} RecordID;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);

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

class RecordIterator {
    private:
        Heapfile *heapfile;
        RecordID *cur_rid;
    public:
        RecordIterator(Heapfile *hFile);
        Record next();
        bool hasNext();
};
 
#endif