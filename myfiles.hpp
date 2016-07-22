#ifndef __myfiles__
#define __myfiles__

#include <string>
#include "datastructures.hpp"

char* read_output( void );
char* read_expect( void );
FILE* open_file( std::string random_file );
long  find_filesize( FILE* file );
std::string find_random_starting_cell( void );
// bool store_cell_in_reproduce_set(vuc loc_memblock);
std::string write_new_cell(vuc loc_memblock);
vuc check_reproductive( void );
void write_progcell(vuc loc_memblock);
double check_training_performance( void );
double check_reproduction_performance(vuc origin, vuc repro);
std::multimap<std::string, double> get_directory_content(std::string mydir);
void restrict_cell_population( void );

#endif
