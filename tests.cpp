#include <stdlib.h>
#include <iostream>      /* cin, cout */
#include <fstream>       /* ifstream, ofstream */
#include <sys/stat.h>
#include <vector>
#include <pstream.h>     /* redi. install dev-cpp/pstreams */
#include <strings.h>
#include <assert.h>
#include "intelligence.hpp"   /* equalFiles */
#include "mysystem.hpp"       /* my_mkdir */

// run tests via ./nature -t

void test_my_mkdir( void ) {
  std::cout << " - test_my_mkdir" << std::endl;
  std::string dirname = "/home/au/dev/selfprog/test/";
  std::vector<char> writable(dirname.begin(), dirname.end());
  writable.push_back('\0');
  // get the char* using &writable[0] or &*writable.begin()
  my_mkdir(&writable[0]);
  struct stat st;
  assert(stat(&writable[0], &st) != -1);
}

void test_my_system( void ){
  std::cout << " - test_my_system" << std::endl;
  std::string cmd = "cat /home/au/dev/selfprog/test.cmd";
  std::string out = my_system(cmd);
  assert(out == "run\n");
}

void test_equalFiles( void){
  std::cout << " - test_equalFiles" << std::endl;
  std::ofstream infile;
  infile.open ("input.dat");
  infile << "37" << std::endl;
  infile << "42" << std::endl;
  infile.close();
  std::ifstream in1("input.dat", std::ios::binary);
  std::ifstream in2("input.dat", std::ios::binary);
  assert(equalFiles(in1, in2));
}
