#include <iostream>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "library.h"

int main(int argc, char *argv[])
{	

	if (argc != 7)
	{
		fprintf(stderr, "USAGE: select3 <colstore_name> <attribute_id> "
			"<return_attribute_id> <start> <end> <page_size>");
		exit(1);
	}

	char *dirName = argv[1];
	char *cmpFile = argv[2];
	char *retFile = argv[3];
	char *startVal = argv[4];
	char *endVal = argv[5];
	int pageSize = atoi(argv[6]);
	std::vector<RecordID> recordIds;

	//assuming: colstore_name is a directory that exists

	DIR *colStore = opendir(argv[1]);
	if (colStore == NULL)
	{
		fprintf(stderr, "Could not open directory %s for reading.\n", argv[1]);
		exit(1);

	}

	//find file for attribute to compare
	struct dirent *entr = readdir(colStore);
	int filesFound = 0;
	while (entr != NULL && filesFound  < 2)
	{
		if (strncmp(entr->d_name, cmpFile, strlen(cmpFile)) == 0)
			filesFound++;
		if (strncmp(entr->d_name, retFile, strlen(retFile)) == 0)
			filesFound++;
		entr = readdir(colStore);
	}
	closedir(colStore);

	if (entr == NULL && filesFound < 2)
	{
		fprintf(stderr, "Could not find both files for attributes %s and %s\n", cmpFile, retFile);
		exit(1);
	}

	if (chdir(dirName) == -1)
	{
		fprintf(stderr, "Could not navigate to storage directory: %s", dirName);
		exit(1);
	}

	Heapfile *compareFile = new Heapfile();
	init_heapfile(compareFile, pageSize, fopen(cmpFile, "rb"));

	RecordIterator *recIter = new RecordIterator(compareFile);
	
	int comparelen = (strlen(startVal) < ATTRIBUTE_SIZE) ? strlen(startVal)  : ATTRIBUTE_SIZE;
	comparelen = (strlen(endVal) < comparelen) ? strlen(endVal) : comparelen;


	while (recIter->hasNext())
	{
		Record rec = recIter->next();
		if (rec.size() != 1)
		{
			fprintf(stderr, "Error: Record size is %d\n", (int)rec.size());
			exit(1);
		}

		if (memcmp(startVal, rec[0], comparelen) <= 0
			&& memcmp(endVal, rec[0], comparelen) >= 0)
			recordIds.push_back(*recIter->cur_rid);
	}

	Heapfile *resultFile = new Heapfile();
	init_heapfile(resultFile, pageSize, fopen(retFile, "rb"));
	char buf[10];
	int maxIter = recordIds.size();
	int i = 0;


	//plan: read in one page at a time, scan through result-set RIDs 
	//return those that are in this page, delete from vector
	while (!recordIds.empty() && i < maxIter)
	{
		int pid = recordIds[0].page_id; //arbitrarily pick the first recordId

		Page *curPage = new Page();
		read_page(resultFile, pid, curPage);
		std::vector<RecordID>::iterator it = recordIds.begin();
		
		//for (std::vector<RecordID>::iterator it = recordIds.begin(); 
				//it != recordIds.end(); it++)
		while (it != recordIds.end())
		{
			if (it->page_id == pid)
			{
				Record *curRec = new Record();
				read_fixed_len_page(curPage, it->slot, curRec);
				if (curRec->size() != 1)
				{	
					cout << "Record size is: " << curRec->size() << endl;
					exit(1);
				}
				//fixed_len_read(buf, ATTRIBUTE_SIZE, curRec);
				char temp[6]; 
				memset(temp, '\0', 6);
				strncpy(temp, curRec->at(0), 5);
				fprintf(stdout, "%s \n", temp);

				it = recordIds.erase(it);
				cout << "Size of recordIds is : " << recordIds.size() << endl;
			}
			else
				it++;
		}
		i++;
		delete curPage;
		cout << "Left inner loop" << endl;
		cout << "value of i is: " << i << endl;
		cout << "size of recordsids is: " << recordIds.size() << endl;
	}

	cout << "Left the loop" << endl;

	if (!recordIds.empty()) //smthg weird happened
	{
		//so do smthg?
	}

	fclose(compareFile->file_ptr);
	fclose(resultFile->file_ptr);


	return 0;
}
