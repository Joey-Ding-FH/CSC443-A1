#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"

using namespace std;

void check_argv(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    char *heapfile_name;
    int page_size;

    check_argv(argc, argv);

    heapfile_name = (char *) malloc(sizeof(char) * strlen(argv[2]));
    strcpy(heapfile_name, argv[1]);

    page_size = atoi(argv[2]);

    scan(heapfile_name, page_size);
}

void check_argv(int argc, char *argv[]) {
    if(argc != 3) {
        fputs("usage: scan <heapfile> <page_size>\n",stderr);
        exit(2);
    }

    if (atoi(argv[2]) <= 0) {
        fputs("usage: <page_size> must be integer and greater than zero\n",stderr);
        exit(2);
    }
}
