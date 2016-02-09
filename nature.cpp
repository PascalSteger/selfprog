// nature.cpp: program to emulate evolution of digital life
// aim: produce super intelligence through natural selection

#include <stdio.h>
#include <iostream>      /* cin, cout */
#include <fstream>       /* ifstream */
#include <stdlib.h>      /* exit, EXIT_FAILURE, srand, rand */
#include <pstream.h>     /* redi */
#include <time.h>        /* time */
#include <sys/time.h>    /* gettimeofday */
#include <limits.h>      /* UCHAR_MAX */
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h> /* SHA-1 */
#include <functional>    /* hash */
#include <string>
#include <sstream>       /* stringstream, for conversion long unsigned => string */
#include <sys/stat.h>    /* chmod */
#include <map>           /* std::multimap */
#include <openssl/sha.h> /* SHA1 */

#include "paths.hpp"
#include "tests.hpp"
#include "intelligence.hpp"
#include "timestamp.hpp"

typedef std::vector<unsigned char> vuc;

bool DEBUG = false;

void print_chars(char* mem, int siz){
  // print char values one by one, in hex representation
  for(int i=0; i<siz; i++){
    //std::cout << int(mem[i]) << " ";
    printf(" %02hhx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}
void print_chars_v(vuc mem){
  for(unsigned int i=0; i<mem.size(); ++i){
    printf(" %02hx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}

int my_mkdir(std::string dirname) {
  struct stat st = {0};
  if (stat(dirname.c_str(), &st) == -1) {
    mkdir(dirname.c_str(), 0700);
  }
  return 0;}

std::string my_timestamp( void ) {
  char * s;
  s = timestring();
  std::string ss(s);
  DEBUG && std::cout << "my_timestamp: ss = " << ss <<std::endl;
  return ss;}

std::string my_system( std::string command) {
  // run a process and create a streambuf that reads its stdout and stderr
  redi::ipstream proc(command, redi::pstreams::pstderr|redi::pstreams::pstdout);
  std::string line;
  // read child's stderr and stdout
  std::string str ("");
  while (proc >> line){
    //std::cout << "output: " << line << std::endl;
    str += line + "\n";
  }
  return str;}

static void show_usage( std::string name ) {
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-t,--test\t\tTest functionality\n"
              << std::endl;}

int test_my_mkdir( void ) {
  std::string dirname = "/home/psteger/test/";
  std::vector<char> writable(dirname.begin(), dirname.end());
  writable.push_back('\0');
  // get the char* using &writable[0] or &*writable.begin()
  my_mkdir(&writable[0]);
  struct stat st = {0};
  if(stat(&writable[0], &st) == -1)
    {
      std::cout << "[EEE] directory not created by my_mkdir" << std::endl;
      return 1;
    }
  else
    {
      std::cout << "[OK] directory created by my_mkdir" << std::endl;
      return 0;
    }}

std::string test_my_system( void ){
  std::string cmd = "ls /home";
  std::string out = my_system(cmd);
  return out;}

int test_equalFiles( void){
  std::ofstream infile;
  infile.open ("input.dat");
  infile << "37" << std::endl;
  infile << "42" << std::endl;
  infile.close();
  std::ifstream in1("input.dat", std::ios::binary);
  std::ifstream in2("input.dat", std::ios::binary);
  if(equalFiles(in1, in2)){
    std::cout << "[OK] equalFiles" << std::endl;
    return 0;
  }
  else{
    std::cout << "[EE] files not equal" << std::endl;
    return 3;
  }}

std::string musec( void ){
  timeval tv;
  gettimeofday(&tv, NULL);
  // tv.tv_usec is long unsigned int
  std::string mus;
  std::stringstream strstream;
  strstream << tv.tv_usec;
  strstream >> mus;
  return mus;}

vuc my_hash(vuc mem) {
  unsigned char ha[20];
  SHA1(&mem[0], mem.size(), ha);
  std::string myha = reinterpret_cast<const char*>(ha);
  std::vector<unsigned char> vmyha (myha.begin(), myha.end());
  //print_chars_v(vmyha);
  return vmyha;}

int main(int argc, char* argv[]) {
  // initialize to always the same random number
  srand (1919); // if working, use srand(time(NULL))

  // run through all possible parameters with a loop
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      return 0;
    }
    else if ((arg == "-t") || (arg == "--test")) {
      test_my_mkdir();
      //test_timestamp();
      //test_copy();
      return 0;
    }  }

  my_mkdir("/tmp/cell/");
  my_mkdir("/tmp/cell/backup");
  /* rsync -avqz --exclude 'backup' ~/dev/cell/ /tmp/cell */
  /* cd /tmp/cell */

  FILE * file;
  long fsize = 0;
  size_t result;

  file = fopen ( "/home/psteger/dev/selfprog/cell" , "rb" );
  if (file==NULL) {fputs ("File error", stderr); exit (1);}
  // obtain file size:
  fseek (file , 0 , SEEK_END);
  fsize = ftell (file);
  rewind (file);

  vuc memblock(fsize);
  std::fread(&memblock[0], sizeof(unsigned char), memblock.size(), file);
  fclose(file);
  //print_chars(memblock, fsize);
  DEBUG && std::cout << "the entire cell content is in memory" << std::endl;

  /********************  define input to learn intelligent answers ********************/
  FILE * input;
  input = fopen ("/tmp/cell/input", "wb");
  std::string strin ("3*3\n4*3\n2*7");
  fwrite (&(strin.c_str())[0] , sizeof(char), strin.size(), input);
  fclose (input);

  /********************  define genepool  ********************/
  // need std::multimap for storing multiple std::vector values at same key
  std::multimap<vuc, vuc> genepool;
  //std::multimap<vuc, vuc>::iterator it;

  vuc vmyha = my_hash(memblock);
  genepool.insert(std::pair<vuc, vuc>(vmyha, memblock));

  std::cout << "gene pool size: " << genepool.size() << std::endl;
  //for (it=mymultimap.begin(); it!=genepool.end(); ++it) {
  //  print_chars_v((*it).first);
  //  print_chars_v((*it).second);
  //}

  // TODO repeat indefinitely in a later step
  for(unsigned int i = 0; i<1000; i++){
    // determine number of bytes to get changed
    int cycles = rand() % 10;
    //cycles = 1; // TODO: comment
    DEBUG && std::cout << "cycles = " << cycles << std::endl;

    // TODO set starting point to any of the active cells
    vuc loc_memblock = memblock;

    for(unsigned int j=1; j <= cycles; j++){
      // choose of three options:
      //  0: add random byte at random position,
      //  1: change byte at random position
      //  2: delete byte from random position
      int option = rand()%3;
      if(option == 0){
        DEBUG && std::cout << "delete random byte" << std::endl;
        if(loc_memblock.size() == 1){
          std::cerr << "Too short program: cannot delete byte" << std::endl;
          continue;
        }
        unsigned pos_del = rand()%loc_memblock.size();
        DEBUG && printf("delete byte at position %d\n", pos_del);
        loc_memblock.erase( loc_memblock.begin()+pos_del);
        //print_chars(loc_memblock, fsize);
      } else if (option == 1) {
        DEBUG && std::cout << "add random byte at random position" << std::endl;
        unsigned pos_add = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        DEBUG && printf("add byte %02hhx at position %d\n", val, pos_add);
        loc_memblock.insert( loc_memblock.begin()+pos_add, val);
      } else if (option == 2) {
        DEBUG && std::cout << "change random byte" << std::endl;
        unsigned pos_change = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        DEBUG && printf("change byte at position %d to %02hhx\n", pos_change, val);
        loc_memblock[pos_change] = val;
        //print_chars(loc_memblock, fsize);
      } else {
        std::cerr << "option must be one of 1,2,3, got " << option << std::endl;
        exit(1);
      }    }
    DEBUG && std::cout << "finished changes" << std::endl;

    /********************  compare to old genes   ********************/
    // get hash of gene = vuc
    vuc vmyha = my_hash(loc_memblock);

    // find all programs with same hash
    std::pair <std::multimap<vuc,vuc>::iterator, std::multimap<vuc,vuc>::iterator> ret;
    ret = genepool.equal_range(vmyha);
    bool found_old = false;
    for (std::multimap<vuc,vuc>::iterator it=ret.first; it!=ret.second; ++it){
      // compare new vuc to all previous ones
      // if old found => continue
      if(loc_memblock == it->second){
        std::cout << "found old gene" << std::endl;
        // print_chars_v(it->second);
        found_old = true;
        break;
      }
    }
    if(found_old) continue;

    // store new (hash, vuc) key-value-pair
    genepool.insert(std::pair<vuc, vuc>(vmyha, loc_memblock));
    std::cout << "gene pool size: " << genepool.size() << std::endl;

    /********************  write new cell         ********************/
    // write memblock to new file, byte-wise
    std::string filename ("/tmp/cell/cell_");
    filename = filename + my_timestamp() + musec();
    DEBUG && std::cout << "filename with timestamp = " << filename << std::endl;
    FILE * outfile;
    outfile = fopen (filename.c_str(), "wb");
    fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), outfile);
    fclose (outfile);

    /********************  make file executable   ********************/
    chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

    /********************  execute with sample input text ********************/
    // NEXT
    std::string output;
    //output = my_system("ls");
    output = my_system(filename+" < /tmp/cell/input > /tmp/cell/output &> /dev/null; echo $?"); //echo $? > exitcode
    //std::cout << "output: " << output << std::endl;
    //int success=999;
    /* check new program exits after finite time */
    /* or even better: kill any program after fixed time (if it has not finished by then) */
    /*     PID=$(ps ax|grep newcell|grep -iv grep|awk '{print $1}') */
    /*     if [[ -n "$PID" ]] */
    /*     then */
    /*         sleep 1 */
    /*         PID=$(ps ax|grep newcell|grep -iv grep|awk '{print $1}') */
    /*         if [[ ! -z "$PID" ]]; then */
    /*             #echo "program took too long" */
    /*             pkill newcell */
    /*             success=998 */
    /*         else */
    /*             success=0 */
    /*         fi */
    /*     else */
    /*         success=$(cat exitcode) */
    /*     fi */
    /*     if (( $success == 0 )) */
    /*     then */
    /*         echo "$i: $cycles: success" */
    /*         cp newcell backup/cell_$(timestamp) */
    /*     fi */
    /* done */

    /********************  sorting out non-reproductive cells ********************/
    /*         od -An -tx1 singles/$nextfile > progcell */
    FILE * progcell;
    progcell = fopen ("/tmp/cell/progcell", "wb");
    // get char values one by one, in hex representation
    for(unsigned int i=0; i<loc_memblock.size(); i++){
    // TODO put in std::string
      fprintf(progcell, " %02hhx", loc_memblock[i]);
    }
    fclose (progcell);

    /*         ./singles/$nextfile < progcell > outcell */
    my_system(filename + " < /tmp/cell/progcell > /tmp/cell/outcell");

    /********************  read in output of cell run on itself ********************/
    FILE * compfile;
    long outfsize = 0;
    compfile = fopen ( "/tmp/cell/outcell" , "rb" );
    if (compfile==NULL) {fputs ("File error", stderr); exit (1);}
    // obtain file size:
    fseek (compfile , 0 , SEEK_END);
    outfsize = ftell (compfile);
    rewind (compfile);
    vuc outblock(outfsize);
    std::fread(&outblock[0], sizeof(unsigned char), outblock.size(), compfile);
    fclose(compfile);
    DEBUG && std::cout << "the cell output content is in memory" << std::endl;

    if(outblock == loc_memblock){
      std::cout << "new recompiling cell" << std::endl;
    } else {
      remove(filename.c_str());}
    remove("/tmp/cell/outcell");

  }
}
