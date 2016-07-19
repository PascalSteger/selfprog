// nature.cpp: program to emulate evolution of digital life
// aim: produce super intelligence through natural selection

#include <stdio.h>
#include <iostream>      /* cin, cout */
#include <fstream>       /* ifstream, ofstream */
#include <stdlib.h>      /* exit, EXIT_FAILURE, srand, rand */
#include <time.h>        /* time */
#include <sys/time.h>    /* gettimeofday */
#include <limits.h>      /* UCHAR_MAX, char_to_bin */
#include <sys/types.h>   /* pid_t */
#include <sys/stat.h>
#include <openssl/sha.h> /* SHA-1 */
#include <functional>    /* hash */
#include <string.h>
#include <sstream>       /* stringstream, for conversion long unsigned => string */
#include <sys/stat.h>    /* chmod */
#include <map>           /* std::multimap */
#include <openssl/sha.h> /* SHA1 */
#include <unistd.h>
/*#include <signal.h>      // kill_child */
#include <sys/wait.h>    /* sleep() */
#include <glob.h>        /* for glob (find file matching pattern) */
#include <random>        /* for poisson distribution */
#include <math.h>          /* math.sqrt */

#include "paths.hpp"
#include "tests.hpp"
#include "intelligence.hpp"
// #include "timestamp.hpp"
#include "datastructures.hpp" /* typedefs, vuc*/
#include "similarity.hpp"     /* calc_similarity */
#include "mysystem.hpp"
#include "debug.hpp"
#include "myfiles.hpp"

void        print_chars(char* mem, int siz){
  // print char values one by one, in hex representation
  for(int i=0; i<siz; i++){
    //std::cout << int(mem[i]) << " ";
    printf(" %02hhx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}

void        print_chars_v(vuc mem){
  for(unsigned int i=0; i<mem.size(); ++i){
    printf(" %02hx", mem[i]);
    if(i % 16 == 15) {
      printf("\n");
    }
  }
  std::cout << std::endl;}

static void show_usage( std::string name ) {
    std::cerr << "Usage: " << name << " <option(s)>"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-t,--test\t\tTest functionality\n"
              << std::endl;}

std::string musec( void ){
  timeval tv;
  gettimeofday(&tv, NULL);
  // tv.tv_usec is long unsigned int
  std::string mus;
  std::stringstream strstream;
  strstream << tv.tv_usec;
  strstream >> mus;
  return mus;}

vuc         my_hash(vuc mem) {
  unsigned char ha[20];
  SHA1(&mem[0], mem.size(), ha);
  std::string myha = reinterpret_cast<const char*>(ha);
  std::vector<unsigned char> vmyha (myha.begin(), myha.end());
  //print_chars_v(vmyha);
  return vmyha;}

void        setup_dirs(){
  my_mkdir("/tmp/cell/");
  my_mkdir("/tmp/cell/reproduce");
  // clean directory to contain no previous files
  my_system("rm /tmp/cell/cell_*");
  // seed new directory with at least one reproducing program
  my_system("cp /home/au/dev/selfprog/cell /tmp/cell/reproduce/");
  // TODO copy meaningful files input_111 and output, after debugging
  my_system("cp /home/au/dev/selfprog/input_111 /tmp/cell/input");
  my_system("cp /home/au/dev/selfprog/expect_111 /tmp/cell/expect");
  my_mkdir("/tmp/cell/backup");}
void        define_sample_input(){
  FILE * input;
  input = fopen ("/tmp/cell/input", "w");
  std::string strin ("111");
  fwrite (&(strin.c_str())[0] , sizeof(char), strin.size(), input);
  fclose (input);}
char *chartobin ( unsigned char c ){
  static char bin[CHAR_BIT + 1] = {0};
  int         i;

  for ( i = CHAR_BIT - 1; i >= 0; i-- )
    {
      bin[i] = (c % 2) + '0';
      c /= 2;
    }

  return bin;}
std::string find_random_starting_cell(){
  glob_t glob_result;
  glob("/tmp/cell/reproduce/*",GLOB_TILDE,NULL,&glob_result);
  std::string random_file = glob_result.gl_pathv[rand()%glob_result.gl_pathc];
  return random_file;}
void  initialize_random(){
  int stime = time(NULL);
  std::cout << "random seed: " << stime << std::endl;
  srand(stime);
  // debug option: initialize to always the same random number
  if(DEBUG) {
      srand(1900); // for DEBUGGING. if whole progam is working, use srand(time(NULL)) to get a new starting point with each call to nature.exe
    }
  return;}
param_struct parse_params( int argc, char* argv[]){
  param_struct fill;
  fill.Niterations = 1000;
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
        fill.Niterations = atoi(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
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
  DEBUG && std::cout << "Niterations = " << fill.Niterations << std::endl;
  return fill;}
void        print_status(status_struct mystatus){
  // delete previous output line:
  std::cout << "\r                                                                 \r";
  std::cout << "N: " << mystatus.N; //<< " P: " << mystatus.poolsize;
  std::cout << " new: " << mystatus.nu << std::flush;}
int main(int argc, char* argv[]) {
  param_struct params = parse_params(argc, argv);
  status_struct now;

  // set random generator and Poisson distribution characteristics for count of # changes
  initialize_random();
  std::default_random_engine generator;
  std::poisson_distribution<int> distribution(params.pois_cycles);

  setup_dirs();
  std::string random_file = find_random_starting_cell(); // find random file from directory with reproducing cells
  // random_file = "/home/psteger/dev/selfprog/cell"; // override cell name with known good one
  std::cout << ".. starting gene: " << random_file << std::endl;
  FILE * file = open_file(random_file);
  long fsize = find_filesize(file);
  vuc memblock(fsize);
  std::fread(&memblock[0], sizeof(unsigned char), memblock.size(), file);
  fclose(file);
  //print_chars_v(memblock);
  DEBUG && std::cout << "the entire cell content is in memory" << std::endl;
  sleep(1);


  // write input file dynamically to test intelligent answer
  // TODO: replace with system to choose from set of patterns
  //       with defined optimal outputs
  // define_sample_input();


  /********************  define genepool  ********************/
  // need std::multimap for storing multiple std::vector values at same key
  std::multimap<vuc, vuc> genepool;
  vuc vmyha = my_hash(memblock);
  genepool.insert(std::pair<vuc, vuc>(vmyha, memblock));

  bool has_not_reproduced = true;
  // main loop
  for(unsigned int iteration = 0; iteration < params.Niterations; iteration++){
    now.N = iteration;
    //if(iteration % 100 == 0){
      // output multimap to file for later reference
      // ideas:
      /*
        Got to make sure the program can run with very large number of programs in the genepool,
        e.g. with moving window (last N genes)
        or sending to database in background (and reading in most used genes)
        OR: not caring about full history, take any of the reproducing programs and start genepool from them
        this way, we can have several instances of nature.cpp run in parallel, each with a different starting program,
        and after N iterations, another instance is started instead
       */

      //std::multimap<vuc, vuc>::iterator it = genepool.first;
    //}

    std::cout << iteration << ": ";
    // determine number of bytes to get changed
    // unsigned int cycles = rand() % 10;
    // better: poisson distributed:
    // this allows for more aggressive changes (count>1), while still keeping most power on small count values,
    // thus guaranteeing
    //
    //    1. many easy changes (higher probability to succeed in compilation and reproduction)
    //    2. evasion of any minimum where more than one change is necessary to get out of
    unsigned int cycles = distribution(generator) + 1;
    DEBUG && std::cout << "cycles = " << cycles << std::endl;

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
    vuc vmyha = my_hash(loc_memblock);    // get hash of gene = vuc

    // find all programs with same hash
    std::pair <std::multimap<vuc,vuc>::iterator, std::multimap<vuc,vuc>::iterator> ret;
    ret = genepool.equal_range(vmyha);
    bool found_old = false;
    for (std::multimap<vuc,vuc>::iterator it=ret.first; it!=ret.second; ++it){
      // compare new vuc to all previous ones
      // if old found => continue
      if(loc_memblock == it->second){
        print_status(now);
        found_old = true;
        break;
      }
    }
    if(found_old) continue;

    // store new (hash, vuc) key-value-pair
    genepool.insert(std::pair<vuc, vuc>(vmyha, loc_memblock));
    //std::cout << "pool: " << genepool.size() << ", ";
    now.poolsize = genepool.size();


    /********************  write new cell         ********************/
    // write memblock to new file, byte-wise
    std::string filename ("/tmp/cell/cell_");
    filename = filename + my_timestamp() + musec();
    std::string filename_cp = filename;
    DEBUG && std::cout << "filename with timestamp = " << filename << std::endl;
    FILE * outfile;
    outfile = fopen (filename.c_str(), "wb");
    fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), outfile);
    fclose (outfile);
    // make file executable
    chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);


    /********************  execute with sample input text ********************/
    std::string erroutput = "";
    erroutput = my_system("timeout 1s " + filename + " < /tmp/cell/input > /tmp/cell/output; echo $?");

    std::cout << "erroutput: " << std::atoi(erroutput.c_str()) << " ";
    if(std::atoi(erroutput.c_str()) == 124){
      // TODO: store in status
      // std::cout << "##### program took too long, aborting ####\r" << std::flush;
      remove(filename.c_str());
      print_status(now);
      continue;
    }


    /********************  sorting out non-reproductive cells ********************/
    /*         od -An -tx1 singles/$nextfile > progcell */
    FILE * progcell;
    progcell = fopen ("/tmp/cell/progcell", "wb");
    // get char values one by one, in hex representation
    for(unsigned int k=0; k<loc_memblock.size(); k++){
      fprintf(progcell, " %02hhx", loc_memblock[k]);
    }
    fclose (progcell);


    /******************** check re-compilation of self ********************/
    my_system(filename + " < /tmp/cell/progcell > /tmp/cell/outcell");
    // in output of cell run on itself
    FILE * compfile;
    long outfsize = 0;
    compfile = fopen ( "/tmp/cell/outcell" , "rb" );
    if (compfile==NULL) {fputs ("File error\n", stderr); exit (1);}
    fseek (compfile , 0 , SEEK_END);
    outfsize = ftell (compfile);
    rewind (compfile);
    vuc outblock(outfsize);
    std::fread(&outblock[0], sizeof(unsigned char), outblock.size(), compfile);
    fclose(compfile);
    DEBUG && std::cout << "the cell output content is in memory" << std::endl;

    if(outblock == loc_memblock){
      has_not_reproduced = false;
      now.nu++;
      print_status(now);

      // store cell in reproduce set
      FILE * progcell;
      std::string fn_working = "/tmp/cell/reproduce/cell_" + my_timestamp() + musec();
      progcell = fopen(fn_working.c_str(), "wb");
      fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), progcell);
      chmod(fn_working.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);       // make file executable

      // determine how well output performs on training samples
      std::string fnout = "/tmp/cell/output";
      FILE * fout = open_file(fnout);
      long foutsize = find_filesize(fout);
      vuc foutmem(foutsize);
      std::fread(&foutmem[0], sizeof(unsigned char), foutmem.size(), fout);
      fclose(fout);

      std::cout << std::endl << " output is of size " << foutsize << std::endl;
      print_chars_v(foutmem);

      std::string fnexp = "/tmp/cell/expect";
      FILE * fexp = open_file(fnexp);
      long fexpsize = find_filesize(fexp);
      vuc fexpmem(fexpsize);
      std::fread(&fexpmem[0], sizeof(unsigned char), fexpmem.size(), fexp);
      fclose(fexp);

      std::cout << std::endl << " expect is of size " << fexpsize << std::endl;
      print_chars_v(fexpmem);
      std::cout << std::endl;

      double cosphi = calc_similarity(foutmem, foutsize, fexpmem, fexpsize);
      std::cout << "  similarity: " << cosphi << std::endl;

      // TODO store similarity = fitness output alongside the output, e.g. in its name
      filename_cp += "_" + std::to_string(cosphi);
      my_system("mv " + filename + " " + filename_cp);
    } else {
      print_status(now);
    }

    remove(filename.c_str());
    remove("/tmp/cell/outcell");
    remove("/tmp/cell/output");
  }
  if(has_not_reproduced){  // after N iterations
    // remove uncooperative cell
    std::cout << std::endl << "cell " << random_file << " failed to get reproductive offspring, deleting" << std::endl;
    remove(random_file.c_str());
  } else {
    std::cout << std::endl << "cell " << random_file << " got reproductive offspring, keeping it" << std::endl;
  }
}
