#ifndef __myfiles__
#define __myfiles__

#include <string>

char* read_output( void );
char* read_expect( void );
FILE* open_file( std::string random_file );
long  find_filesize( FILE* file );
std::string find_random_starting_cell( void );

#endif
