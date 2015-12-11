// read in cell to byte/char array
// print char values one by one, to get back cell.he

#include <iostream>     /* cin, cout */
#include <fstream>      /* ifstream */
#include <stdlib.h>     /* exit, EXIT_FAILURE, srand, rand */
#include <time.h>       /* time */
#include <limits.h>     /* UCHAR_MAX */

void print_chars(char* mem, int siz){
  // print char values one by one, in hex representation
  for(int i=0; i<siz; i++){
    //std::cout << int(mem[i]) << " ";
    printf(" %02hhx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;
}

int main () {
  std::streampos size;
  char * memblock;

  // ios::ate (pointer at end) allows tellg() to directly give filesize
  std::ifstream file ("cell", std::ios::in|std::ios::binary|std::ios::ate);
  if (!file.is_open())
  {
     std::cout << "Unable to open file" << std::endl;
     exit(EXIT_FAILURE);
  }
  size = file.tellg();
  memblock = new char [size];
  file.seekg (0, std::ios::beg);
  file.read (memblock, size);
  file.close();
  //std::cout << "the entire file content is in memory" << std::endl;
  print_chars(memblock, size);

  // change one byte at random position to random char value
  /* initialize random seed: */
  srand (time(NULL));
  unsigned pos_change = rand()%size;
  char val = rand()%UCHAR_MAX;
  printf("change byte at position %d to %02hhx\n", pos_change, val);
  memblock[pos_change] = val;
  print_chars(memblock, size);



  // delete byte at random position
  if(size == 1){
    std::cerr << "Too short program" << std::endl;
    exit(EXIT_FAILURE);
  }
  unsigned pos_del = rand()%size;
  printf("delete byte at position %d\n", pos_del);
  unsigned size_del = int(size)-1;
  char * memblock2;
  memblock2 = new char [size_del];
  for(int i=0; i<size; i++){
    if(i < pos_del){
      memblock2[i] = memblock[i];
    } else {
      memblock2[i] = memblock[i+1];
    }
  }
  size = size_del;
  memblock = new char [size_del];
  memblock = memblock2;
  print_chars(memblock, size);


  // add byte at random position
  unsigned pos_add = rand()%size;
  val = rand()%UCHAR_MAX;
  printf("add byte %02hhx at position %d\n", val, pos_add);
  unsigned size_add = int(size)+1;
  char * memblock3;
  memblock3 = new char [size_add];
  for(int i=0; i<size_add; i++){
    if(i < pos_add){
      memblock3[i] = memblock[i];
    } else if(i == pos_add) {
      memblock3[i] = val;
    } else if(i > pos_add) {
      memblock3[i] = memblock[i-1];
    }
  }
  size = size_add;
  memblock = new char [size_add];
  memblock = memblock3;
  print_chars(memblock, size);


  // write memblock to new file, byte-wise
  std::ofstream outfile ("cell2", std::ios::out|std::ios::binary);
  if (!outfile.is_open())
  {
     std::cout << "Unable to open file" << std::endl;
     exit(EXIT_FAILURE);
  }
  outfile.write (memblock, size);
  outfile.close();
  std::cout << "cell2 written" << std::endl;


  delete[] memblock;
  return 0;
}
