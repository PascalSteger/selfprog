#include "helpers.hpp"

#include <iostream>
#include <sys/stat.h>
#include <vector>

int test_my_mkdir( void ) {
  std::string dirname = "/home/psteger/test/";

  std::vector<char> writable(dirname.begin(), dirname.end());
  writable.push_back('\0');

  // get the char* using &writable[0] or &*writable.begin()
  my_mkdir(&writable[0]);
  struct stat st = {0};
  if(stat(&writable[0], &st) == -1)
    {
      std::cout << "directory not created by my_mkdir";
      return 1;
    }
  else
    {
      std::cout << "directory created by my_mkdir";
      return 0;
    }
}

int test_timestamp( void ) {
  std::string ts = my_timestamp();
  std::cout << "check timestamp: " << ts;
  return 0;
}
