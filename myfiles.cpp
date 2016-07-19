#include <iostream>   /* streampos */
#include <fstream>    /* ifstream */

#include "debug.hpp"

char* read_output(){
  std::streampos size;
  char * memblock;
  std::ifstream file ("/tmp/cell/output", std::ios::in|std::ios::binary|std::ios::ate);
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

  std::ifstream file ("/tmp/cell/expect", std::ios::in|std::ios::binary|std::ios::ate);
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
