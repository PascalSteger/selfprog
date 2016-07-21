#include <iostream>   /* streampos */
#include <fstream>    /* ifstream */
#include <string>     /* std::string */
#include <glob.h>     /* for glob (find file matching pattern) */
#include <random>
#include <sys/stat.h>    /* chmod */

#include "debug.hpp"
#include "paths.hpp"
#include "datastructures.hpp"
#include "mysystem.hpp"
#include "timestamp.hpp"

char* read_output(){
  std::streampos size;
  char * memblock;
  std::ifstream file ((PATH_CELL+"output").c_str(),
                      std::ios::in|std::ios::binary|std::ios::ate);
  if (file.is_open())
    {
      size = file.tellg();
      memblock = new char [size];
      file.seekg (0, std::ios::beg);
      file.read (memblock, size);
      file.close();
      DEBUG && std::cout << "the entire file content is in memory: " << memblock << std::endl;
      return memblock;
      delete[] memblock;
    }
  return new char [0];};

char* read_expect(){
  std::streampos size;
  char * memblock;

  std::ifstream file (PATH_CELL+"expect", std::ios::in|std::ios::binary|std::ios::ate);
  if (file.is_open())
    {
      size = file.tellg();
      memblock = new char [size];
      file.seekg (0, std::ios::beg);
      file.read (memblock, size);
      file.close();
      DEBUG && std::cout << "the entire file content is in memory" << std::endl;
      return memblock;
      delete[] memblock;
    }
  return new char [0];};

FILE* open_file(std::string random_file){
  FILE * file;
  //random_file = "/home/au/dev/selfprog/cell"; // override cell name with known good one
  file = fopen ( random_file.c_str(), "rb" );
  if (file==NULL) {fputs ("File error", stderr); exit (1);}
  return file;}

long  find_filesize(FILE* file){
  fseek (file , 0 , SEEK_END);
  long fsize = ftell (file);
  rewind (file);
  return fsize;}

std::string find_random_starting_cell( void ){
  glob_t glob_result;
  glob("/tmp/cell/reproduce/cell*", GLOB_TILDE, NULL, &glob_result );
  std::string random_file = glob_result.gl_pathv[rand()%glob_result.gl_pathc];
  return random_file;}

bool store_cell_in_reproduce_set(vuc loc_memblock){
  FILE * progcell;
  std::string fn_working = PATH_REPRODUCE + "cell_" + my_timestamp() + musec();
  progcell = fopen(fn_working.c_str(), "wb");
  fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), progcell);
  // make file executable
  chmod(fn_working.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
  return true;
}

std::string write_new_cell(vuc loc_memblock){
  // write memblock to new file, byte-wise
  std::string filename (PATH_CELL+"cell_");
  filename = filename + my_timestamp() + musec();
  DEBUG && std::cout << "filename with timestamp = " << filename << std::endl;
  FILE * outfile;
  outfile = fopen (filename.c_str(), "wb");
  fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), outfile);
  fclose (outfile);
  // make file executable
  chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
  return filename;
}
