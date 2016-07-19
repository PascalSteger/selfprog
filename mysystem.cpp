#include <iostream>
#include <string>
#include <sys/stat.h>    /* struct stat */
#include <pstream.h>     /* redi */


#include "debug.hpp"
#include "timestamp.hpp"
#include "datastructures.hpp"

int my_mkdir(std::string dirname) {
  struct stat st = {0};
  if (stat(dirname.c_str(), &st) == -1) {
    mkdir(dirname.c_str(), 0700);
  }
  return 0;}

std::string my_timestamp( void ) {
  char * s;
  s = timestring();
  std::string ss(s);
  DEBUG && std::cout << "my_timestamp: ss = " << ss <<std::endl;
  return ss;}

std::string my_system(std::string command) {
  //std::cout << "         my_system:  " << command << std::endl;
  // run a process and create a streambuf that reads its stdout and stderr
  redi::ipstream proc(command, redi::pstreams::pstderr|redi::pstreams::pstdout);
  std::string line;
  // read child's stderr and stdout
  std::string str ("");
  while (proc >> line){
    //std::cout << "output: " << line << std::endl;
    str += line + "\n";
  }
  return str;}
