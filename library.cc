#include <iostream>
#include <iterator>
#include <assert.h>
#include "library.h"

#define ATTRIBUTE_LENGTH 10
#define BIT_SIZE 0.125

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
	int result = 0;

	for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
		result += strlen(*it) * sizeof(char);
	}

	return result;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
	for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
		for (const char *i = *it; *i != '\0'; i++) {
			*(char*)buf = *i;
			buf = ((char*)buf) + 1;
    		}
	}
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
	int total_size_read = 0;
	char *temp = (char *) malloc(ATTRIBUTE_LENGTH);
	int pos = 0;

	while (total_size_read < size && *(((char *)buf) + total_size_read) != '\0') {
		*(temp + pos) = *(((char *)buf) + total_size_read);
		total_size_read++;
		pos++;
		if (pos == ATTRIBUTE_LENGTH) {
			pos = 0;
			record->push_back(temp);
			temp = (char *) malloc(ATTRIBUTE_LENGTH); //this looks bad, why are you allocating more memory? 
								  //can't you just reuse the same temp?
		}
	} //also you didn't call free, so temp will cause a memory leak..
}

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
	assert(slot_size <= page_size);

	page->data = malloc(page_size);
	page->page_size = page_size;
	page->slot_size = slot_size;
}

/**
 * Calculates the maximal number of records that fit in a page
 * ("records" or slots? clarify with someone)
 */
int fixed_len_page_capacity(Page *page) {
	int page_size = page->page_size;
	int slot_size = page->slot_size;

	return (page_size - sizeof(int)) / (slot_size + BIT_SIZE); //???
}


/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
	
	return -1;
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r){
	
	return -1;
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r){
	
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r){
	
}












