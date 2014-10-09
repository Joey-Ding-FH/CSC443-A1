#include <iostream>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "library.h"
#include "csv2heapfile.h"

using namespace std;

#define SLOT_SIZE 1000
#define OFFSET_SIZE sizeof(uint32_t)

int get_free_space_size(int page_size);
int get_number_of_pages(int page_size);
size_t fwrite_with_check(const void *ptr, size_t size, size_t count, FILE *file);
size_t fread_with_check(void *ptr, size_t size, size_t count, FILE *file);
uint32_t alloc_page_at_end(FILE *file, int page_size);
int reach_page(Heapfile *heapfile, PageID pid);

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
	heapfile->page_size = page_size;

	Page *first_page = new Page();
	init_fixed_len_page(first_page, page_size, SLOT_SIZE);

	fwrite(first_page, sizeof(Page), 1, file);
    fwrite(first_page->data, page_size, 1, file);

    free(first_page->data);

	heapfile->file_ptr = file;
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile) {
	int page_size = heapfile->page_size;
	FILE *file = heapfile->file_ptr;

	rewind(file);
	fflush(file);

	fseek(file, sizeof(Page) + OFFSET_SIZE, SEEK_SET);

	int free_space_size = get_free_space_size(page_size);
	int number_of_pages = get_number_of_pages(page_size);
	int page_number = 0;

	while(read_offset(file) != 0) {
		fseek(file, free_space_size, SEEK_CUR);
		page_number++;
		if (page_number % number_of_pages == 0) {
			// If current directory page is full, go to next one.
			fseek(file, -(number_of_pages * (free_space_size + OFFSET_SIZE) + OFFSET_SIZE), SEEK_CUR);
			uint32_t next_directory_page_offset = read_offset(file);

			if (next_directory_page_offset == 0) {
				// If need to create new directory page.
				next_directory_page_offset = alloc_page_at_end(file, page_size);
    			fwrite_with_check(&next_directory_page_offset, sizeof(uint32_t), 1, file);
			}
			fseek(file, next_directory_page_offset, SEEK_SET);
			fseek(file, sizeof(Page) + OFFSET_SIZE, SEEK_SET);
		}
	}
	fseek(file, -OFFSET_SIZE, SEEK_CUR);

	uint32_t offset = alloc_page_at_end(file, page_size);

	// Create new entry.
	fwrite_with_check(&offset, sizeof(uint32_t), 1, file);
	fwrite_with_check(&page_size, free_space_size, 1, file);

	fflush(file);

	return page_number + 1;
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page) {
	int page_size = heapfile->page_size;
	FILE *file = heapfile->file_ptr;
	
	rewind(file);

	if (reach_page(heapfile, pid) == -1) {
		return;
	}

	page->data = malloc(page_size);

	fwrite_with_check(page, sizeof(Page), 1, file);
	fwrite_with_check(page->data, page_size, 1, file);
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid) {
	int page_size = heapfile->page_size;
	FILE *file = heapfile->file_ptr;
	
	rewind(file);

	if (reach_page(heapfile, pid) == -1) {
		return;
	}

	fread_with_check(page, sizeof(Page), 1, file);
	fread_with_check(page->data, page_size, 1, file);
}

int reach_page(Heapfile *heapfile, PageID pid) {
	int page_size = heapfile->page_size;
	FILE *file = heapfile->file_ptr;

	fseek(file, sizeof(Page) + OFFSET_SIZE, SEEK_SET);
	fseek(file, (pid - 1) * (OFFSET_SIZE + get_free_space_size(page_size)), SEEK_CUR);

	uint32_t offset = read_offset(file);

	if (offset != 0) {
		fseek(file, offset, SEEK_SET);
	} else {
		fputs("pid doesn't exist\n",stderr);
		return -1;
	}
	return 0;
}

uint32_t alloc_page_at_end(FILE *file, int page_size) {
	Page *new_page = new Page();
	init_fixed_len_page(new_page, page_size, SLOT_SIZE);

	long int current = ftell(file);
	fseek (file, 0, SEEK_END);
	uint32_t offset = ftell(file);

	fwrite_with_check(new_page, sizeof(Page), 1, file);
	fwrite_with_check(new_page->data, page_size, 1, file);

	free(new_page->data);

	fseek(file, current, SEEK_SET);

	return offset;
}

size_t fwrite_with_check(const void *ptr, size_t size, size_t count, FILE *file) {
	int result;
	if ((result = fwrite(ptr, size, count, file)) != 1) {
		fputs("Write error\n",stderr); 
	}
	return result;
}

size_t fread_with_check(void *ptr, size_t size, size_t count, FILE *file) {
	int result;
	if ((result = fread(ptr, size, count, file)) != 1) {
		fputs("Read error\n",stderr); 
	}
	return result;
}

int get_free_space_size(int page_size) {
	return ceil(log2(page_size * 8) / 8);
}

int get_number_of_pages(int page_size) {
	return page_size / (get_free_space_size(page_size) + OFFSET_SIZE);
}

uint32_t read_offset(FILE *file) {
	uint32_t *buffer = (uint32_t *) malloc(OFFSET_SIZE);
	if (buffer == NULL) {
		fputs("Memory error\n",stderr); 
		exit(2);
	}

	if (fread_with_check(buffer, OFFSET_SIZE, 1, file) != 1) {
		free(buffer);
		return 0;
	}

	int result = *buffer;
	free(buffer);

    return result;
}

