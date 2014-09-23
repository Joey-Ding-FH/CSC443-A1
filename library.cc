#include <iostream>
#include <iterator>
#include <assert.h>
#include "library.h"

#define ATTRIBUTE_LENGTH 10
#define BIT_SIZE 0.125

int fixed_len_sizeof(Record *record) {
	int result = 0;

	for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
		result += strlen(*it) * sizeof(char);
	}

	return result;
}

void fixed_len_write(Record *record, void *buf) {
	for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
		for (const char *i = *it; *i != '\0'; i++) {
			*(char*)buf = *i;
			buf = ((char*)buf) + 1;
    	}
	}
}

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
			temp = (char *) malloc(ATTRIBUTE_LENGTH);
		}
	}
}

void init_fixed_len_page(Page *page, int page_size, int slot_size) {
	assert(slot_size <= page_size);

	page->data = malloc(page_size);
	page->page_size = page_size;
	page->slot_size = slot_size;
}

int fixed_len_page_capacity(Page *page) {
	int page_size = page->page_size;
	int slot_size = page->slot_size;

	return (page_size - sizeof(int)) / (slot_size + BIT_SIZE);
}












