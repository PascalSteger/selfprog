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
#include "myoutput.hpp"
#include "similarity.hpp"

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
  if(DEBUG){
    // override cell name with known good one
    random_file = PATH_PROG + "cell";
  }
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

vuc check_reproductive( void ){
  FILE * compfile;
  long outfsize = 0;
  compfile = fopen ( (PATH_CELL+"outcell").c_str() , "rb" );
  if (compfile==NULL) {
    fputs ("File error\n", stderr);
    exit (1);}
  fseek (compfile , 0 , SEEK_END);
  outfsize = ftell (compfile);
  rewind (compfile);
  vuc outblock(outfsize);
  std::fread(&outblock[0], sizeof(unsigned char), outblock.size(), compfile);
  fclose(compfile);
  print_debug( "the cell output content is in memory" );
  return outblock;
}

void write_progcell(vuc loc_memblock){
  /*         od -An -tx1 singles/$nextfile > progcell */
  FILE * progcell;
  progcell = fopen ((PATH_CELL+"progcell").c_str(), "wb");
  // get char values one by one, in hex representation
  for(unsigned int k=0; k<loc_memblock.size(); k++){
    fprintf(progcell, " %02hhx", loc_memblock[k]);
  }
  fclose (progcell);
}

double check_training_performance(){
  // determine how well output performs on training samples
  std::string fnout = PATH_CELL + "output";
  FILE * fout = open_file(fnout);
  long foutsize = find_filesize(fout);
  vuc foutmem(foutsize);
  std::fread(&foutmem[0], sizeof(unsigned char), foutmem.size(), fout);
  fclose(fout);

  std::cout << std::endl << " output is of size " << foutsize << std::endl;
  print_chars_v(foutmem);

  std::string fnexp = PATH_CELL + "expect";
  FILE * fexp = open_file(fnexp);
  long fexpsize = find_filesize(fexp);
  vuc fexpmem(fexpsize);
  std::fread(&fexpmem[0], sizeof(unsigned char), fexpmem.size(), fexp);
  fclose(fexp);

  std::cout << std::endl << " expect is of size " << fexpsize << std::endl;
  print_chars_v(fexpmem);
  std::cout << std::endl;

  double cosphi = calc_similarity(foutmem, foutsize, fexpmem, fexpsize);
  return cosphi;
}
