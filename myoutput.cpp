#include <iostream>   /* cout, endl */
#include "datastructures.hpp"

void        print_chars(char* mem, int siz){
  // print char values one by one, in hex representation
  for(int i=0; i<siz; i++){
    //std::cout << int(mem[i]) << " ";
    printf(" %02hhx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}

void        print_chars_v(vuc mem){
  for(unsigned int i=0; i<mem.size(); ++i){
    printf(" %02hx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}

void        print_status(status_struct mystatus){
  // delete previous output line:
  std::cout << "\r                                                                 \r";
  std::cout << "N: " << mystatus.N; //<< " P: " << mystatus.poolsize;
  std::cout << " new: " << mystatus.nu << std::flush;}

static void show_usage( std::string name ) {
  std::cerr << "Usage: " << name << " <option(s)>"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help message\n"
            << "\t-t,--test\t\tTest functionality\n"
            << std::endl;}
