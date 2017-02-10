#include <stdlib.h>
#include <iostream>      /* cin, cout */
#include <fstream>       /* ifstream, ofstream */
#include <sys/stat.h>
#include <vector>
#include <pstream.h>     /* redi. install dev-cpp/pstreams */
#include <strings.h>
#include "intelligence.hpp"   /* equalFiles */
#include "mysystem.hpp"       /* my_mkdir */

int test_my_mkdir( void ) {
  std::string dirname = "/home/psteger/test/";
  std::vector<char> writable(dirname.begin(), dirname.end());
  writable.push_back('\0');
  // get the char* using &writable[0] or &*writable.begin()
  my_mkdir(&writable[0]);
  struct stat st = {0};
  if(stat(&writable[0], &st) == -1)
    {
      std::cout << "[EEE] directory not created by my_mkdir" << std::endl;
      return 1;
    }
  else
    {
      std::cout << "[OK] directory created by my_mkdir" << std::endl;
      return 0;
    }}

std::string test_my_system( void ){
  std::string cmd = "ls /home";
  std::string out = my_system(cmd);
  return out;}

int test_equalFiles( void){
  std::ofstream infile;
  infile.open ("input.dat");
  infile << "37" << std::endl;
  infile << "42" << std::endl;
  infile.close();
  std::ifstream in1("input.dat", std::ios::binary);
  std::ifstream in2("input.dat", std::ios::binary);
  if(equalFiles(in1, in2)){
    std::cout << "[OK] equalFiles" << std::endl;
    return 0;
  }
  else{
    std::cout << "[EE] files not equal" << std::endl;
    return 3;
  }}
