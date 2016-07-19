#include <random>        /* srand */
#include <time.h>        /* time */
#include <iostream>      /* cout, endl */

#include "debug.hpp"

void  initialize_random(){
  int stime = time(NULL);
  std::cout << "random seed: " << stime << std::endl;
  srand(stime);
  // debug option: initialize to always the same random number
  if(DEBUG) {
      srand(1900); // for DEBUGGING. if whole progam is working, use srand(time(NULL)) to get a new starting point with each call to nature.exe
    }
  return;}