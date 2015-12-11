// for mkdir
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <pstream.h>
#include <strings.h>

// for time_numbers
#include "timestamp.hpp"

int my_mkdir(std::string dirname) {
  struct stat st = {0};
  if (stat(dirname.c_str(), &st) == -1) {
    mkdir(dirname.c_str(), 0700);
  }
}

char* my_timestamp( void ) {
  char *s;
  s = timestring ();
  return s;
}

std::string my_system( std::string command) {
  // run a process and create a streambuf that reads its stdout and stderr
  redi::ipstream proc(command, redi::pstreams::pstderr|redi::pstreams::pstdout);
  std::string line;
  // read child's stderr
  while (proc >> line)
    std::cout << "output: " << line << std::endl;
}
