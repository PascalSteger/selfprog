#include <string>     /* std::cout */
#include <iostream>
#include <stdlib.h>   /* atof */
#include "tests.hpp"
#include "datastructures.hpp"
#include "myoutput.hpp"
#include "debug.hpp"

param_struct parse_params( int argc, char* argv[]){
  param_struct fill;
  fill.pois_cycles = 0.5;
  // run through all possible runtime parameters with a loop
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      exit(0);
    }
    else if ((arg == "-t") || (arg == "--test")) {
      test_my_mkdir();
      //test_timestamp();
      //test_copy();
      exit(0);
    } else if ((arg == "-i") || (arg == "--iterations")) {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        fill.N_MAX_ITERATIONS = atoi(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
      } else { // Uh-oh, there was no argument to the destination option.
        std::cerr << "-i|--iterations option requires one argument." << std::endl;
      }
    } else if ((arg == "-c") || (arg == "--cycles")) {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        fill.pois_cycles = atof(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
      } else { // Uh-oh, there was no argument to the destination option.
        std::cerr << "-c|--cycles option requires one argument." << std::endl;
      }
    }
  }
  if(DEBUG) std::cout << "N_MAX_ITERATIONS = " << fill.N_MAX_ITERATIONS << std::endl;
  return fill;}
