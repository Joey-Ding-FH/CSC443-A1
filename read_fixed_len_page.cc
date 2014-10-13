#include <stdlib.h>
#include <iostream>
#include "library.h"

int main(int argc, char *argv[])
{	
	if (argc != 3)
	{
		fprintf(stderr, "USAGE: read_fixed_len_page <page_file> <page_size>");
		exit(1);
	}

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
		//read in metadata first
		fread(page, sizeof(Page), 1, pageFile);
		fread(page->slot_info, fixed_len_page_capacity(page) * sizeof(char), 1, pageFile);	
		//read the page & output the records
		fread(page->data, pageSize, 1, pageFile);

		for (int slot = 0; slot < fixed_len_page_capacity(page); slot++)
		{
			Record *rec = new Record();
			char *buf = (char*) malloc(ATTRIBUTE_SIZE + 1);
			memset(buf, '\0', ATTRIBUTE_SIZE + 1);
			read_fixed_len_page(page, slot, rec); //if slot empty, record empty
			for (int i=0; i < rec->size(); i++)
			{
				strncpy(buf, rec->at(i), ATTRIBUTE_SIZE);
				char format;
				if (i + 1 == rec->size()) //last attribute
					format = '\n';
				else 
					format = ',';

				fprintf(stdout, "%s%c", buf, format);
			}
		}
	}

	fclose(pageFile);

}
