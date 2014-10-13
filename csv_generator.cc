#include <iostream>
#include <fstream>
#include <stdint.h>
#include "library.h"

using namespace std;

int main (int argc, char *argv[]) {
	if(argc != 3) {
        fputs("usage: generateCSV <csv_file> <number_of_lines>\n",stderr);
        exit(2);
    }

    char *csv_file_name = argv[1];
    uint32_t number_of_lines = atoi(argv[2]);
    if (number_of_lines <= 0) {
    	fputs("usage: <number_of_lines> must be greater than zero\n",stderr);
        exit(2);
    }

	ofstream myfile;
	myfile.open ("big_test.csv");

	// TODO
	
	myfile.close();
	return 0;
}