#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "library.h"

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		fprintf(stderr, "USAGE: write_fixed_len_page <csv_file> <page_file> <page_size>\n");
		exit(1);
	}

	//start timer
	clock_t start = clock();

	if (!(ifstream(argv[1])))
	{
		fprintf(stderr, "Could not open csv file: %s", argv[1]);
		exit(1);
	}
	ifstream csvfile(argv[1]);

	FILE *pageFile = fopen(argv[2], "a");

	int pageSize = atoi(argv[3]);

	int numPages = 0;
	int numRecords = 0;

	

	while (!csvfile.eof())
	{
		Page *page = new Page();
		init_fixed_len_page(page, pageSize, SLOT_SIZE);
		numRecords += read_csv2page(&csvfile, page);

		char *slot_info = (char *) malloc(fixed_len_page_capacity(page) * sizeof(char));
		write_bytes(page->slot_info, slot_info);

		//cout << "Byte array is: " << slot_info << endl;

		//write page to file
		fwrite(page, sizeof(Page), 1, pageFile);
		fwrite(slot_info, fixed_len_page_capacity(page), 1, pageFile);
		fwrite(page->data, page->page_size, 1, pageFile);

		char buf[page->page_size + 1];
		memset(buf, '\0', page->page_size + 1);
		strncpy(buf, (char *)page->data, page->page_size);

		//cout << "Page data is " << buf << endl;
		numPages++;

	}


	int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;

	fprintf(stdout, "NUMBER OF RECORDS: %d\n", numRecords);
	fprintf(stdout, "NUMBER OF PAGES: %d\n", numPages);
	fprintf(stdout, "TIME: %d milliseconds\n", msecTime);
	return(0);

}
