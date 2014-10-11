#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"

using namespace std;

int main() {
	Record *record = new Record(); 

	V v1 = "1234567890";
	V v2 = "abcdefghij";

	record->push_back(v2);
	record->push_back(v1);

	//cout << fixed_len_sizeof(record) << endl;

	void *buf = malloc(fixed_len_sizeof(record));

	fixed_len_write(record, buf);

	cout << "buf is " << (char *)buf << endl;

	Record *new_record = new Record();

	fixed_len_read(buf, 20, new_record);

	
	for (std::vector<V>::iterator it = new_record->begin() ; it != new_record->end(); ++it) {
		cout << *it << endl;
	} 

	//cout << fixed_len_sizeof(new_record) << endl;

	//free(buf);


	//Page *p = new Page();
	//init_fixed_len_page(p, 86, 10);

	//cout << fixed_len_page_capacity(p) << endl;

	//cout << fixed_len_page_freeslots(p) << endl;
	
	return 0;
}