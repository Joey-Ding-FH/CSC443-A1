#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "csv2heapfile.h"
#include "library.h"

using namespace std;

int main () {
  /*
  Heapfile *heapfile = new Heapfile();
  FILE *pFile = fopen ("file" , "w");

  init_heapfile(heapfile, 16384, pFile);

  fclose(pFile);
  */

  
  
  // FILE *pFile = fopen ("file2" , "wb");
  // uint32_t a = (uint32_t) 1;
  // uint32_t b = (uint32_t) 32714;

  // fwrite(&a, sizeof(uint32_t), 1, pFile);
  // fwrite(&b, sizeof(uint32_t), 1, pFile);

  // fclose(pFile);

  // char * buffer;
  // size_t result;

  // pFile = fopen ( "file2" , "rb" );
  // if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // // allocate memory to contain the whole file:
  // buffer = (char*) malloc (sizeof(char));
  // if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // // copy the file into the buffer:
  // while ((result = fread (buffer,sizeof(char),1,pFile)) == sizeof(char)) {
  // 	cout << "char is " << buffer << endl;
  // 	for (int i = 7; i >= 0; i--) { // or (int i = 0; i < 8; i++)  if you want reverse bit order in bytes
  //       cout << ((*buffer >> i) & 1);
  //   }
  //   cout << endl;
  // }
  
  

  // pFile = fopen ( "file2" , "rb" );
  // if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // cout << "pos is " << ftell(pFile) << endl;
  // cout << read_offset(pFile) << endl;
  // cout << "pos is " << ftell(pFile) << endl;
  // cout << read_offset(pFile) << endl;
  // fclose(pFile);

  FILE *pFile = fopen ("heapfile" , "wb");
  fclose(pFile);
  
  Heapfile *heapfile = new Heapfile();
  int page_size = 16384;
  FILE *file = fopen ("heapfile" , "r+");

  init_heapfile(heapfile, page_size, file);
  alloc_page(heapfile);
  alloc_page(heapfile);

  fseek(file, sizeof(Page) + 4, SEEK_SET);
  cout << read_offset(file) << endl;

  fseek(file, 3, SEEK_CUR);
  cout << read_offset(file) << endl;
  // char * buffer;
  // size_t result;

  // FILE *pFile = fopen ( "heapfile" , "rb" );
  // if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // // allocate memory to contain the whole file:
  // buffer = (char*) malloc (sizeof(char));
  // if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}


  // fseek(file, sizeof(Page) + sizeof(uint32_t), SEEK_SET);
  // int n = 0;
  // // copy the file into the buffer:
  // while ((result = fread (buffer,sizeof(char),1,pFile)) == sizeof(char) && n <=10) {
  //   n++;
  //  cout << "char is " << buffer << endl;
  //  for (int i = 7; i >= 0; i--) { // or (int i = 0; i < 8; i++)  if you want reverse bit order in bytes
  //       cout << ((*buffer >> i) & 1);
  //   }
  //   cout << endl;
  // }

  Page *page = new Page();

  read_page(heapfile, 1, page);
  read_page(heapfile, 2, page);
  read_page(heapfile, 3, page);

  return 0;
}