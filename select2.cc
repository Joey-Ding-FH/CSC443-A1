#include <iostream>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include "library.h"


int main(int argc, char *argv[]){
	if (argc != 6)
	{
		fprintf(stderr, "USAGE: select2 <colstore_name> <attribute_id> <start>"
			"<end> <page_size>");
		exit(1);
	}

	//assuming: colstore_name is a directory that exists

	if (chdir(argv[1]) == -1)
	{
		fprintf(stderr, "Could not navigate to storage directory: %s", argv[2]);
		exit(1);
	}

	DIR *colStore = opendir(argv[1]);
	if (colStore == NULL)
	{
		fprintf(stderr, "Could not open directory %s for reading.", argv[2]);
		exit(1);

	}

	char *fileName = argv[2];
	char *startVal = argv[3];
	char *endVal = argv[4];
	int pageSize = atoi(argv[5]);

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

	Heapfile *hpFile = new Heapfile();
	init_heapfile(hpFile, pageSize, fopen(fileName, "rb"));

	RecordIterator *recIter = new RecordIterator(hpFile);

	while (recIter->hasNext())
	{
		Record rec = recIter->next();
		if(rec.size() != 1)
		{
			//print some sort of error..or just keep going?
			exit(1);
		}
		if (memcmp(startVal, rec[0], ATTRIBUTE_SIZE) == 0)
		{
			char buf[5]; 
			fixed_len_read(buf, 5, &rec);
			fprintf(stdout, "%s \n", buf);
		}
	}

	fclose(hpFile->file_ptr);

	return 0;
}
