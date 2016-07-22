#include <iostream>   // streampos
#include <stdlib.h>   // atof
#include <fstream>    // ifstream
#include <string>     // std::string
#include <glob.h>     // for glob (find file matching pattern)
#include <random>
#include <sys/stat.h> // chmod
#include <dirent.h>   // listing directory contents
#include <map>        // std::multimap

#include "debug.hpp"
#include "paths.hpp"
#include "datastructures.hpp"
#include "mysystem.hpp"
#include "timestamp.hpp"
#include "myoutput.hpp"
#include "similarity.hpp"
#include "mysystem.hpp"  // my_system

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

// bool store_cell_in_reproduce_set(vuc loc_memblock){
//   FILE * progcell;
//   std::string fn_working = PATH_REPRODUCE + "cell_" + my_timestamp() + musec();
//   progcell = fopen(fn_working.c_str(), "wb");
//   fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), progcell);
//   // make file executable
//   chmod(fn_working.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
//   return true;}

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
  return filename;}

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
  return outblock;}

void write_progcell(vuc loc_memblock){
  /*         od -An -tx1 singles/$nextfile > progcell */
  FILE * progcell;
  progcell = fopen ((PATH_CELL+"progcell").c_str(), "wb");
  // get char values one by one, in hex representation
  for(unsigned int k=0; k<loc_memblock.size(); k++){
    fprintf(progcell, " %02hhx", loc_memblock[k]);
  }
  fclose (progcell);}

double check_training_performance(){
  // determine how well output performs on training samples
  std::string fnout = PATH_CELL + "output";
  FILE * fout = open_file(fnout);
  long foutsize = find_filesize(fout);
  vuc foutmem(foutsize);
  std::fread(&foutmem[0], sizeof(unsigned char), foutmem.size(), fout);
  fclose(fout);

  if(DEBUG){
    std::cout << std::endl << " output is of size " << foutsize << std::endl;
    print_chars_v(foutmem);
  }

  std::string fnexp = PATH_CELL + "expect";
  FILE * fexp = open_file(fnexp);
  long fexpsize = find_filesize(fexp);
  vuc fexpmem(fexpsize);
  std::fread(&fexpmem[0], sizeof(unsigned char), fexpmem.size(), fexp);
  fclose(fexp);

  if(DEBUG){
    std::cout << std::endl << " expect is of size " << fexpsize << std::endl;
    print_chars_v(fexpmem);
    std::cout << std::endl;
  }

  double cosphi = calc_similarity(foutmem, foutsize, fexpmem, fexpsize);
  return cosphi;}

std::multimap<std::string, double> get_directory_content(std::string mydir){
  std::multimap<std::string, double> dircontents;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (mydir.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      // printf ("%s\n", ent->d_name);
      std::string cellname = ent->d_name;
      // std::cout << "cellname = " << cellname << std::endl;

      std::size_t found = cellname.find("_");
      if (found != std::string::npos) {
        // std::cout << "first '_' found at: " << found << '\n';
      } else {
        continue;
      }

      found = cellname.find("_", found+1, 1);
      if (found != std::string::npos) {
        // std::cout << "second '_' found at: " << found << '\n';
      } else {
        continue;
      }
      std::string dd = cellname.substr(found+1);     // get from "_" to the end
      // std::cout << " dd = " << dd << std::endl;

      double performance = atof(dd.c_str());
      dircontents.insert(std::pair<std::string, double> (cellname, performance));
    }
    closedir(dir);
  } else {
    perror ("could not open directory");
  }
  return dircontents;
}

void restrict_cell_population(){
  // list all (file,test_score) in PATH_REPRODUCE
  std::multimap<std::string, double> allcells = get_directory_content(PATH_REPRODUCE);

  // old: determine number of cells to be deleted
  // unsigned int delete_N = max(0, N_CONCURRENT - allcells.size());

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1);

  // as long as there are too many cells:
  while(allcells.size() > N_CONCURRENT){
    // find random element
    // std::cout << "find random element " << std::flush << std::endl;
    std::multimap<std::string, double>::iterator it = allcells.begin();
    std::size_t idx = rand() % allcells.size();
    std::advance(it, idx);

    // death match
    std::string filename = PATH_REPRODUCE + (*it).first;
    double performance = (*it).second;
    // std::cout << filename << " " << performance << std::endl;

    double ra = dis(gen);

    // std::cout << "   random number: " << ra << std::flush << std::endl;
    if(ra < performance){
      // if not good enough: remove from multimap
      my_system("rm " + filename );
      allcells.erase(it);
      std::cout << "cell " + filename + " died!" << std::flush << std::endl;
    }
  }
}
