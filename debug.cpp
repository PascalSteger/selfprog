#include <iostream>
#include <string>
#include "debug.hpp"

bool print_debug( std::string output ){
  if (DEBUG){
    std::cout << output << std::endl;
  }
  return DEBUG;
}
