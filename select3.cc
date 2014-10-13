#include <iostream>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include "library.h"

int main(int argc, char *argv[])
{	

	if (argc != 7)
	{
		fprintf(stderr, "USAGE: select3 <colstore_name> <attribute_id> "
			"<return_attribute_id> <start> <end> <page_size>");
		exit(1);
	}

	//assuming: colstore_name is a directory that exists

	if (chdir(argv[1]) == -1)
	{
		fprintf(stderr, "Could not navigate to storage directory: %s", argv[2]);
		exit(1);
	}

	//TODO: parsing for case where <colstore_name> is a full path? or whatever...

	DIR *colStore = opendir(argv[1]);
	if (colStore == NULL)
	{
		fprintf(stderr, "Could not open directory %s for reading.", argv[2]);
		exit(1);

	}

	char *cmpFile = argv[2];
	char *retFile = argv[3];
	char *startVal = argv[4];
	char *endVal = argv[5];
	int pageSize = atoi(argv[6]);
	std::vector<RecordID> recordIds;

	//find file for attribute to compare
	struct dirent *entr = readdir(colStore);

	while (entr != NULL)
	{
		if (strncmp(entr->d_name, cmpFile, strlen(cmpFile)) == 0)
			break;
		entr = readdir(colStore);
	}

	if (entr == NULL)
	{
		fprintf(stderr, "Could not find file for attribute %s", cmpFile);
		exit(1);
	}

	Heapfile *compareFile = new Heapfile();
	init_heapfile(compareFile, pageSize, fopen(cmpFile, "rb"));

	RecordIterator *recIter = new RecordIterator(compareFile);

	while (recIter->hasNext())
	{
		Record rec = recIter->next();
		if (rec.size() != 1)
		{

		}

		if (memcmp(startVal, rec[0], ATTRIBUTE_SIZE) <= 0
			&& memcmp(endVal, rec[0], ATTRIBUTE_SIZE) >= 0)
			recordIds.push_back(*recIter->cur_rid);
	}

	Heapfile *resultFile = new Heapfile();
	init_heapfile(resultFile, pageSize, fopen(retFile, "rb"));
	char buf[5];
	int maxIter = recordIds.size();
	int i = 0;


	//plan: read in one page at a time, scan through result-set RIDs 
	//return those that are in this page, delete from vector
	while (!recordIds.empty() && i < maxIter)
	{
		int pid = recordIds[0].page_id; //arbitrarily pick the first recordId

		Page *curPage = new Page();
		read_page(resultFile, pid, curPage);
		for (std::vector<RecordID>::iterator it = recordIds.begin(); 
				it != recordIds.end(); it++)
		{
			if (it->page_id == pid)
			{
				Record *curRec = new Record();
				read_fixed_len_page(curPage, it->slot, curRec);
				fixed_len_read(buf, 5, curRec);
				fprintf(stdout, "%s \n", buf);

				recordIds.erase(it);
			}
		}
		i++; //for safety
	}

	if (!recordIds.empty()) //smthg weird happened
	{
		//so do smthg?
	}

	fclose(compareFile->file_ptr);
	fclose(resultFile->file_ptr);


	return 0;
}