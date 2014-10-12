#include <iostream>
#include <iterator>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "library.h"

int main(int argc, char *argv[])
{

	assert(argc == 4);
	//USAGE: csv2colstore <csv_file> <colstore_name> <pagesize>


	//1. open the file
	//2. make some heap files?
	//3. make a directory to put all these heap files (in a loop)
	//4. read the file...

	FILE *csvfile = fopen(argv[1], "rb");

	if (csvfile == NULL){

		fprintf(stderr, "Cannot open CSV file: %s", argv[1]);
		exit(1);
	}

	if (mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH) == -1)
	{
		if (errno != EEXIST) { //if directory exists, swallow exception keep going
			fprintf(stderr, "Could not create storage directory: %s", argv[2]);
			exit(1);
		}

	}

	if (chdir(argv[2]) == -1)
	{
		fprintf(stderr, "Could not navigate to storage directory: %s", argv[2]);
		exit(1);
	}

	int pageSize = atoi(argv[3]);

	std::vector<Heapfile> attributeFiles;
	std::vector<Page> workingPages; 
	std::vector<int> workingPageIDs;

	char *filename;

	for (int i = 0 ; i < ATTR_PER_RECORD; i++)
	{
		sprintf(filename, "%d", i);
		FILE *file = fopen(filename, "w"); //probably won't compile : /
		Heapfile *hpFile = new Heapfile();
		init_heapfile(hpFile, pageSize, file);

		int pageID = alloc_page(hpFile);

		Page *curPage = new Page();
		init_fixed_len_page(curPage, pageSize, ATTRIBUTE_SIZE);

		attributeFiles.push_back(*hpFile); //line-up heapfiles with Pages
		workingPages.push_back(*curPage);
		workingPageIDs.push_back(pageID);
	}

	// can either:
	// initialize 100 Heap files, keep them in a vector
	// every time we read an attribute, move to a different heap file
	// also need array of page pointers to update as we read attribute by attribute
	// (make sure slot_size is only size of 1 attribute)
	// when page is full write out to disk, replace entry in page array...? 

	int numBytesRead = 0; //keep track of much we read
	int attrInd = 0; //index of current attribute being read
	char *buf; //temporary buffer for reading

	while (!feof(csvfile))
	{
		if (fread(buf, ATTRIBUTE_SIZE, 1, csvfile) != ATTRIBUTE_SIZE)
			break;  //do nothing or throw error? decide later

		Page *curPage = &workingPages[attrInd];
		Heapfile *curFile = &attributeFiles[attrInd];

		Record *rec = new Record();
		fixed_len_read(buf, ATTRIBUTE_SIZE, rec);

		if (add_fixed_len_page(curPage, rec) == -1) //page full do smthg
		{
			write_page(curPage, curFile, workingPageIDs[attrInd]);

			int newPageId = alloc_page(curFile);

			Page *newPage = new Page();
			init_fixed_len_page(newPage, pageSize, ATTRIBUTE_SIZE);

			workingPages[attrInd] = *newPage;
			workingPageIDs[attrInd] = newPageId;

			free(curPage);
		}

		attrInd = (attrInd + 1) % ATTR_PER_RECORD;
	}

	return 0;
}