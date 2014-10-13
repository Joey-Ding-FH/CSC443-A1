#include <iostream>
#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "library.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "USAGE: csvgen <fileName> <numLines>.\n");
		exit(1);

	}

	char *fileName = argv[1];
	int numLines = atoi(argv[2]);


	if (numLines <= 0)
	{
		fprintf(stderr, "Number of lines must be greater than 0.\n");
		exit(1);
	}
	//ASCII ranges we care about
	int startRange = 97;
	int endRange = 122;


	ofstream outfile;
	outfile.open(fileName);

	int lineCount = 0;
	while (lineCount <= numLines)
	{
		for (int i = 0; i < ATTR_PER_RECORD; i++)
		{
			std::string buf(ATTRIBUTE_SIZE, '\0');
			for (int i = 0; i < ATTRIBUTE_SIZE; i++)
			{
				char randc = (rand() % (endRange - startRange)) + startRange;
				buf.replace(i, 1, 1, randc);
			}

			outfile.write(buf.c_str(), ATTRIBUTE_SIZE);

			if (i == ATTR_PER_RECORD -1) //last entry in the row
				outfile.write("\n", 1);	
			else
				outfile.write(",", 1);	
			
		}
		lineCount++;
	}
	outfile.close();
}