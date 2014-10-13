#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iterator>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include "library.h"

using namespace std;

int get_entry_size(int page_size);
int get_free_space_size(int page_size);
int get_number_of_pages(int page_size);
size_t fwrite_with_check(const void *ptr, size_t size, size_t count, FILE *file);
size_t fread_with_check(void *ptr, size_t size, size_t count, FILE *file);
uint32_t alloc_page_at_end(FILE *file, int page_size);
int reach_page(Heapfile *heapfile, PageID pid);
uint32_t read_offset(FILE *file);

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
    int pos = 0;
	for (std::vector<V>::iterator it = record->begin(); it != record->end(); ++it) {
        for (const char *i = *it; *i != '\0'; i++) {
            *((char*) buf + pos) = *i;
            pos++;
        }
	}
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 * Read attribute by attribute, since we are not assuming anything about what size is and whose multiple it could be of
 */
void fixed_len_read(void *buf, int size, Record *record) {
	assert(size >= ATTRIBUTE_SIZE); //at least one attribute is being read

	while (size >= ATTRIBUTE_SIZE){
        char *temp = (char *) malloc(ATTRIBUTE_SIZE + 1);
        memset(temp, '\0', ATTRIBUTE_SIZE + 1);
        strncpy(temp, (char *)buf, ATTRIBUTE_SIZE);
        record->push_back(temp);

        size -= ATTRIBUTE_SIZE;
        buf = ((char*)buf) + ATTRIBUTE_SIZE;
	}
}

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
	assert(slot_size <= page_size);

	page->data = malloc(page_size);
	page->page_size = page_size;
	page->slot_size = slot_size;
	page->slot_info = new ByteArray;

	int num_slots = fixed_len_page_capacity(page);
	for (int i = 0; i < num_slots; i++)
		page->slot_info->push_back('0');
}

/**
 * Calculates the maximal number of records/slots that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
	return (page->page_size / page->slot_size);
}

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
	
	int num_free = 0;
	//std::vector<char>::iterator it;
	for (int i = 0; i < page->slot_info->size(); i++){
		if (page->slot_info->at(i) == '0')
			num_free++;
	}

	return num_free; 
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r){
	int free_slots = fixed_len_page_freeslots(page);
	if (free_slots > 0) {
		for (int ind = 0; ind < page->slot_info->size(); ind++){
			if (page->slot_info->at(ind) == '0'){
				write_fixed_len_page(page, ind, r);
                page->slot_info->at(ind) = '1';
				return ind;
			}
		}
	
	}
	return -1;
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r){
	char *buf = ((char *)page->data + (slot * SLOT_SIZE));
	fixed_len_write(r, buf);
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r){

    if (page->slot_info->at(slot) == '0')
        return;
    
	char *buf = ((char * )page->data + (slot * SLOT_SIZE));
	fixed_len_read(buf, SLOT_SIZE, r);
}


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
            fseek(file, -(number_of_pages * (get_entry_size(page_size)) + OFFSET_SIZE), SEEK_CUR);
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
    if (reach_page(heapfile, pid) == -1) {
        page->data = NULL;
        return;
    }

    fread_with_check(page, sizeof(Page), 1, file);
    page->data = malloc(page_size);
    if (page->data == NULL) {
        fputs("Memory error\n", stderr); 
        exit(2);
    }
    char *slot_info = (char *) malloc(fixed_len_page_capacity(page) * sizeof(char));

    fread_with_check(slot_info, fixed_len_page_capacity(page) * sizeof(char), 1, file);
    fread_with_check(page->data, page_size, 1, file);

    page->slot_info = new ByteArray;
    read_bytes(slot_info, fixed_len_page_capacity(page) * sizeof(char), page->slot_info);
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid) {
    int page_size = heapfile->page_size;
    FILE *file = heapfile->file_ptr;

    if (reach_page(heapfile, pid) == -1) {
        page->data = NULL;
        return;
    }
    char *slot_info = (char *) malloc(fixed_len_page_capacity(page) * sizeof(char));
    write_bytes(page->slot_info, slot_info);

    fwrite_with_check(page, sizeof(Page), 1, file);
    fwrite_with_check(slot_info, fixed_len_page_capacity(page) * sizeof(char), 1, file);
    fwrite_with_check(page->data, page_size, 1, file);
}

/**
 * Read lines in file into page. Return when page is full.
 */
int read_csv2page(ifstream *file, Page *page) {
    int free_space = fixed_len_page_freeslots(page);
    char chars_to_remove[] = ",\"";
    string line;
	int numRecs = 0;

    for(;free_space > 0 && getline(*file, line); free_space--) {
        for (int i = 0; i < strlen(chars_to_remove); ++i) {
            line.erase(remove(line.begin(), line.end(), chars_to_remove[i]), line.end());
        }
        Record *record = new Record;
        fixed_len_read((void *) line.c_str(), SLOT_SIZE, record);

        add_fixed_len_page(page, record);
		numRecs++;
    }
	return numRecs;
}

/**
 * Write attribute from record into buf.
 */
void write_attr(Record *record, int attr_id, void *buf) {
    char *r = (char *) malloc(SLOT_SIZE);

    fixed_len_write(record, r);

    for (int i = 0; i < ATTRIBUTE_SIZE; i++) {
        *((char *) buf + i) = *(r + attr_id * ATTRIBUTE_SIZE + i);
    }

    free(r);
}

/**
 * Read attribute from buf into record.
 */
void read_attr(Record *record, int attr_id, void *buf) {
    char *value = (char *) buf;

    assert(strlen(value) == ATTRIBUTE_SIZE);

    record->at(attr_id) = value;
}

RecordIterator::RecordIterator(Heapfile *hFile) {
    page_size = hFile->page_size;
    heapfile = hFile;

    cur_rid = (RecordID*) malloc(sizeof(RecordID));
    cur_rid->page_id = 1;
    cur_rid->slot = 0;
    has_next = true;

    cur_page = new Page;
    read_page(hFile, cur_rid->page_id, cur_page);

    if (cur_page->data != NULL) {
        find_next();
    } else {
        has_next = false;
    }
}

Record RecordIterator::next() {
    Record *record = new Record();
    read_fixed_len_page(cur_page, cur_rid->slot, record);

    cur_rid->slot++;
    find_next();

    return *record;
}

bool RecordIterator::hasNext() {
    return has_next;
}

void RecordIterator::find_next() {
    while (cur_page->slot_info->at(cur_rid->slot) == '0') {
        cur_rid->slot++;
        if (cur_rid->slot >= fixed_len_page_capacity(cur_page)) {
            
            cur_rid->page_id++;
            cur_rid->slot = 0;

            free(cur_page->data);
            cur_page = new Page;

            read_page(heapfile, cur_rid->page_id, cur_page);

            if (cur_page->data == NULL) {
                has_next = false;
                break;
            }
        }
    }
}

int reach_page(Heapfile *heapfile, PageID pid) {
    int page_size = heapfile->page_size;
    FILE *file = heapfile->file_ptr;

    int number_of_pages_per_dir = get_number_of_pages(page_size);
    int nth_dir = pid / number_of_pages_per_dir;
    int order_in_dir = pid % number_of_pages_per_dir;

    rewind(file);

    for (int i = 0; i < nth_dir; i++) {
        fseek(file, sizeof(Page), SEEK_CUR);
        uint32_t next_dir_offset = read_offset(file);
        if (next_dir_offset == 0) {
            return -1;
        }
        fseek(file, next_dir_offset, SEEK_SET);
    }

    fseek(file, sizeof(Page) + OFFSET_SIZE, SEEK_CUR);
    fseek(file, (order_in_dir - 1) * get_entry_size(page_size), SEEK_CUR);
    uint32_t offset = read_offset(file);

    if (offset != 0) {
        fseek(file, offset, SEEK_SET);
    } else {
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

    char *slot_info = (char *) malloc(fixed_len_page_capacity(new_page) * sizeof(char));
    write_bytes(new_page->slot_info, slot_info);

    fwrite_with_check(new_page, sizeof(Page), 1, file);
    fwrite_with_check(slot_info, fixed_len_page_capacity(new_page) * sizeof(char), 1, file);
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

int get_entry_size(int page_size) {
    return get_free_space_size(page_size) + OFFSET_SIZE;
}

int get_free_space_size(int page_size) {
    return ceil(log2(page_size * 8) / 8);
}

int get_number_of_pages(int page_size) {
    return (page_size - OFFSET_SIZE) / (get_free_space_size(page_size) + OFFSET_SIZE);
}

uint32_t read_offset(FILE *file) {
    uint32_t *buffer = (uint32_t *) malloc(OFFSET_SIZE);
    if (buffer == NULL) {
        fputs("Memory error\n", stderr); 
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

void read_bytes(void *buf, int numSlots, ByteArray *slot_info) {
    for (int i = 0; i < numSlots; i++) {
        slot_info->push_back(*((char *) buf + i));
    }
}

void write_bytes(ByteArray *slot_info, void *buf) {
    for (int i = 0; i < slot_info->size(); i++) {
        *((char*) buf + i) = slot_info->at(i);
    }
}
