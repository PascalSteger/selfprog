// for mkdir
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

// for time_numbers
#include "timestamp.hpp"

int my_mkdir(char* dirname) {
  struct stat st = {0};
  if (stat(dirname, &st) == -1) {
    mkdir(dirname, 0700);
  }
}

char* my_timestamp( void ) {
  char *s;
  s = timestring ();
  return s;
}
