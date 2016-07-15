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

#include "paths.hpp"
#include "tests.hpp"
#include "intelligence.hpp"
#include "timestamp.hpp"

struct param_struct {
  unsigned Niterations;
  float pois_cycles;};
struct status_struct {
  unsigned N;
  unsigned poolsize;
  unsigned nu = 0;
};

typedef std::vector<unsigned char> vuc;
bool DEBUG = false;

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

int         my_mkdir(std::string dirname) {
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
  //std::cout << "         my_system:  " << command << std::endl;
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
    std::cerr << "Usage: " << name << " <option(s)>"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-t,--test\t\tTest functionality\n"
              << std::endl;}

int         test_my_mkdir( void ) {
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

int         test_equalFiles( void){
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

vuc         my_hash(vuc mem) {
  unsigned char ha[20];
  SHA1(&mem[0], mem.size(), ha);
  std::string myha = reinterpret_cast<const char*>(ha);
  std::vector<unsigned char> vmyha (myha.begin(), myha.end());
  //print_chars_v(vmyha);
  return vmyha;}

void        setup_dirs(){
  my_mkdir("/tmp/cell/");
  my_system("rm /tmp/cell/cell_*");
  my_mkdir("/tmp/cell/reproduce");
  // seed new directory with at least one reproducing program
  my_system("cp /home/au/dev/selfprog/cell /tmp/cell/reproduce/");
  my_system("cp /home/au/dev/selfprog/input_111 /tmp/cell/input");
  my_system("cp /home/au/dev/selfprog/expect_111 /tmp/cell/expect");
  my_mkdir("/tmp/cell/backup");}
void        define_sample_input(){
  FILE * input;
  input = fopen ("/tmp/cell/input", "w");
  std::string strin ("111");
  fwrite (&(strin.c_str())[0] , sizeof(char), strin.size(), input);
  fclose (input);}
char*       read_output(){
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
      DEBUG && std::cout << "the entire file content is in memory" << std::endl;
      return memblock;
      delete[] memblock;
    }
  return new char [0];};
char*       read_expect(){
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
std::string find_random_starting_cell(){
  glob_t glob_result;
  glob("/tmp/cell/reproduce/*",GLOB_TILDE,NULL,&glob_result);
  std::string random_file = glob_result.gl_pathv[rand()%glob_result.gl_pathc];
  return random_file;}
FILE*       set_starting_file(std::string random_file){
  FILE * file;
  //random_file = "/home/au/dev/selfprog/cell"; // override cell name with known good one
  std::cout << ".. starting gene: " << random_file << std::endl;
  file = fopen ( random_file.c_str(), "rb" );
  if (file==NULL) {fputs ("File error", stderr); exit (1);}
  return file;}
long        find_filesize(FILE* file){
  fseek (file , 0 , SEEK_END);
  long fsize = ftell (file);
  rewind (file);
  return fsize;}
void        initialize_random(){
  int stime = time(NULL);
  std::cout << "random seed: " << stime << std::endl;
  srand(stime);
  // debug option: initialize to always the same random number
  //srand (1900); // for DEBUGGING. if whole progam is working, use srand(time(NULL)) to get a new starting point with each call to nature.exe
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
  FILE * file = set_starting_file(random_file);
  long fsize = find_filesize(file);
  // FILE * file;

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
    DEBUG && std::cout << "filename with timestamp = " << filename << std::endl;
    FILE * outfile;
    outfile = fopen (filename.c_str(), "wb");
    fwrite (&loc_memblock[0] , sizeof(char), loc_memblock.size(), outfile);
    fclose (outfile);
    // make file executable
    chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);


    /********************  execute with sample input text ********************/
    std::string erroutput = "";
    erroutput = my_system("timeout 1s " + filename + " < /tmp/cell/input > /tmp/cell/output &> /dev/null; echo $?");

    std::cout << "erroutput: " << std::atoi(erroutput.c_str()) << " ";
    if(std::atoi(erroutput.c_str()) == 124){
      //TODO: store in status std::cout << "##### program took too long, aborting ####\r" << std::flush;
      remove(filename.c_str());
      remove("/tmp/cell/outcell");
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

      // check how well it performed on sample input
      char* out = read_output();
      char* exp = read_expect();
      std::cout << " .. output vs expectation: " << out << " " << exp << std::endl;

    } else {
      print_status(now);
    }

    remove(filename.c_str());
    remove("/tmp/cell/outcell");
  }
  if(has_not_reproduced){  // after N iterations
    // remove uncooperative cell
    std::cout << std::endl << "cell " << random_file << " failed to get reproductive offspring, deleting" << std::endl;
    remove(random_file.c_str());
  } else {
    std::cout << std::endl << "cell " << random_file << " got reproductive offspring, keeping it" << std::endl;
  }
}
