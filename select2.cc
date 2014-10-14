#include <iostream>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "library.h"


int main(int argc, char *argv[]){
	if (argc != 6)
	{
		fprintf(stderr, "USAGE: select2 <colstore_name> <attribute_id> <start>"
			"<end> <page_size>");
		exit(1);
	}

	char *dirName = argv[1];
	char *fileName = argv[2];
	char *startVal = argv[3];
	char *endVal = argv[4];
	int pageSize = atoi(argv[5]);

	//assuming: colstore_name is a directory that exists
	DIR *colStore = opendir(dirName);
	if (colStore == NULL)
	{
		fprintf(stderr, "Could not open directory %s for reading. Exiting with error: %d\n",
			dirName, errno);
		exit(1);

	}

	struct dirent *entr = readdir(colStore);

	while (entr != NULL)
	{
		if (strncmp(entr->d_name, fileName, strlen(fileName)) == 0)
			break;
		entr = readdir(colStore);
	}

	if (entr == NULL)
	{
		fprintf(stderr, "Could not find file for attribute %s", fileName);
		exit(1);
	}
	closedir(colStore);

	if (chdir(dirName) == -1)
	{
		fprintf(stderr, "Could not navigate to storage directory: %s\n", dirName);
		exit(1);
	}

	Heapfile *hpFile = new Heapfile();
	init_heapfile(hpFile, pageSize, fopen(fileName, "rb"));

	//cout << "Heapfile initialized for attributeId: " << fileName << endl;

	RecordIterator *recIter = new RecordIterator(hpFile);
	
	int comparelen = (strlen(startVal) < ATTRIBUTE_SIZE) ? strlen(startVal)  : ATTRIBUTE_SIZE;
	comparelen = (strlen(endVal) < comparelen) ? strlen(endVal) : comparelen;

	//cout << "Length of comparison is :" << comparelen << endl;

	while (recIter->hasNext())
	{
		Record rec = recIter->next();
		if(rec.size() != 1)
		{
			fprintf(stderr, "Error: Record size is %d\n", (int)rec.size());
			exit(1);
		}

		if (memcmp(startVal, rec[0], comparelen) <= 0
			&& memcmp(endVal, rec[0], comparelen) >= 0)
		{
			char ret[6];
			memset(ret, '\0', 6);
			strncpy(ret, rec[0], 5);
			fprintf(stdout, "%s \n", ret);
		}
	}

	fclose(hpFile->file_ptr);

	return 0;
}
