#ifndef __myoutput__
#define __myoutput__

#include <string>  /* std::string */
#include "datastructures.hpp"  /* status_struct, vuc */

void print_chars( char* mem, int siz );
void print_chars_v( vuc mem );
void print_status( status_struct mystatus );
void show_usage( std::string name );

#endif
