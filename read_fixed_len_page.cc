#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "library.h"

int main(int argc, char *argv[])
{	
	if (argc != 3)
	{
		fprintf(stderr, "USAGE: read_fixed_len_page <page_file> <page_size>\n");
		exit(1);
	}
	//start timer
	clock_t start = clock();


	FILE *pageFile = fopen(argv[1], "rb");

	if (pageFile == NULL)
	{
		fprintf(stderr, "Cannot open page file: %s", argv[1]);
		exit(1);
	}

	
	int pageSize = atoi(argv[2]);

	while (!feof(pageFile))
	{
		Page *page = new Page();
		init_fixed_len_page(page, pageSize, SLOT_SIZE);

		//fprintf(stdout, "Page initialized with pageSize %d, slotsize %d\n", page->page_size, page->slot_size);

		//read in metadata first

		char *slotInfo = (char *)malloc(fixed_len_page_capacity(page) * sizeof(char));
		fread(page, sizeof(Page), 1, pageFile);

		//cout << "Read some metadata" << endl;
		//fprintf(stdout, "Page read in with pageSize %d, slotsize %d\n", page->page_size, page->slot_size);


		fread(slotInfo, fixed_len_page_capacity(page) * sizeof(char), 1, pageFile);

		//cout << "Read in slot info" << endl;

		page->slot_info = new ByteArray;

		read_bytes(slotInfo, fixed_len_page_capacity(page), page->slot_info);	
		//cout << "Metadata read successfully" << endl;
		
		page->data = malloc(pageSize);
		//read the page & output the records
		fread(page->data, pageSize, 1, pageFile);
		
		//cout << "Page data read successfully" << endl;

		for (int slot = 0; slot < fixed_len_page_capacity(page); slot++)
		{
			Record *rec = new Record();
			//char *buf = (char*) malloc(ATTRIBUTE_SIZE + 1);
			//memset(buf, '\0', ATTRIBUTE_SIZE + 1);
			if (page->slot_info->at(slot) == '1')
			{
				read_fixed_len_page(page, slot, rec); //if slot empty, record empty
				
				//fprintf(stdout, "Page details: page_size %d, slot size %d\n", page->page_size, page->slot_size);
				//cout << "Record size is : " << rec->size() << endl;


				//exit(1);
				
				//cout << "Record read into buffer: " << rec->at(0) << endl;
				for (int i=0; i < rec->size(); i++)
				{
					//strncpy(buf, rec->at(i), ATTRIBUTE_SIZE);
					char format;
					if (i == rec->size() - 1) //last attribute
						format = '\n';
					else 
						format = ',';

					fprintf(stdout, "%s%c", rec->at(i), format);
				}
			}
		}
	}

	fclose(pageFile);
		
	int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	fprintf(stdout, "TIME: %d milliseconds\n", msecTime);

	return(0);
}
