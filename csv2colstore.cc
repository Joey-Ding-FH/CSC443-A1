#include <iterator>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include "library.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "USAGE: csv2colstore <csv_file> <colstore_name>"
            "<pagesize>\n");
        exit(1);
    }
    //USAGE: csv2colstore <csv_file> <colstore_name> <pagesize>

    if (!ifstream(argv[1]))
    {
        fprintf(stderr, "Cannot open CSV file: %s", argv[1]);
        exit(1);
    }
    ifstream csvfile(argv[1]);

    if (mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH) == -1)
    {
        if (errno != EEXIST) { //if directory exists, swallow exception
            fprintf(stderr, "Could not create storage directory: %s\n", argv[2]);
            exit(1);
        }

    }

    if (chdir(argv[2]) == -1)
    {
        fprintf(stderr, "Could not navigate to storage directory: %s\n", argv[2]);
        exit(1);
    }

    int pageSize = atoi(argv[3]);

    std::vector<Heapfile> attributeFiles;
    std::vector<Page> workingPages; 
    std::vector<int> workingPageIDs;

    char filename[3];

    for (int i = 0 ; i < ATTR_PER_RECORD; i++)
    {
        sprintf(filename, "%d", i);
        FILE *file = fopen(filename, "wb+r"); 
        Heapfile *hpFile = new Heapfile();
        init_heapfile(hpFile, pageSize, file);

        int pageID = alloc_page(hpFile);

        Page *curPage = new Page();
        init_fixed_len_page(curPage, pageSize, ATTRIBUTE_SIZE);

        attributeFiles.push_back(*hpFile); //line-up heapfiles with Pages
        workingPages.push_back(*curPage);
        workingPageIDs.push_back(pageID);
    }

	//cout << "Finished initializing structures" << endl;

    // can either:
    // initialize 100 Heap files, keep them in a vector
    // every time we read an attribute, move to a different heap file
    // also need array of page pointers to update as we read attribute by attribute
    // (make sure slot_size is only size of 1 attribute)
    // when page is full write out to disk, replace entry in page array...? 


    string line;

    while (getline(csvfile, line)) {
		int attrInd = 0; //index/"attributeID"
		int len = strlen(line.c_str());
        char *temp = (char *) malloc(len + 1);
		memset(temp, '\0', len+1);
        strncpy(temp, line.c_str(), len);

        char *buf;
        buf = strtok (temp, ",");
		//cout << "Finished string business" << endl;
        while (buf != NULL)
        {
            Page *curPage = &workingPages[attrInd];
            Heapfile *curFile = &attributeFiles[attrInd];

            Record *rec = new Record();
            fixed_len_read(buf, ATTRIBUTE_SIZE, rec);

			//test test
			//cout << "Record entries are: " << endl; 
			//for (int i = 0; i < rec->size(); i++)
				//cout << rec->at(i) << endl;

            if (add_fixed_len_page(curPage, rec) == -1) //page full do smthg
            {
				//cout << "Doing page full stuff" << endl;
                write_page(curPage, curFile, workingPageIDs[attrInd]);

				//cout << "Page written successfully" << endl;

                int newPageId = alloc_page(curFile);

				//cout << "Page allocated successfully" << endl;

                Page *newPage = new Page();
                init_fixed_len_page(newPage, pageSize, ATTRIBUTE_SIZE);
                add_fixed_len_page(newPage, rec);

				//cout << "New page initialized" << endl;

                workingPages[attrInd] = *newPage;
                workingPageIDs[attrInd] = newPageId;
                
            }

            //attrInd = (attrInd + 1) % ATTR_PER_RECORD;
			attrInd++;

            buf = strtok (NULL, ",");
        }
    }

    //cleanup: write all the pages and close all the files
    for (int i = 0; i < ATTR_PER_RECORD; i++)
    {
        Page *curPage = &workingPages[i];
        Heapfile *curFile = &attributeFiles[i];
        write_page(curPage, curFile, workingPageIDs[i]);

        fflush(curFile->file_ptr);
        fclose(curFile->file_ptr);

    }

    return 0;
}
