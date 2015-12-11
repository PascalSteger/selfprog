// nature.cpp: program to emulate evolution of digital life
// aim: produce super intelligence through natural selection

#include <stdio.h>
#include <iostream>     /* cin, cout */
#include <fstream>      /* ifstream */
#include <stdlib.h>     /* exit, EXIT_FAILURE, srand, rand */
#include <pstream.h>    /* redi */
#include <time.h>       /* time */
#include <sys/time.h>   /* gettimeofday */
#include <limits.h>     /* UCHAR_MAX */
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h> /* SHA-1 */
#include <functional>    /* hash */
#include <string>
#include <sstream>       /* stringstream, for conversion long unsigned => string */

#include "tests.hpp"
#include "intelligence.hpp"
#include "paths.hpp"
#include "timestamp.hpp"

bool DEBUG = false;

int my_mkdir(std::string dirname) {
  struct stat st = {0};
  if (stat(dirname.c_str(), &st) == -1) {
    mkdir(dirname.c_str(), 0700);
  }
  return 0;
}

std::string my_timestamp( void ) {
  char * s;
  s = timestring();
  std::string ss(s);
  DEBUG && std::cout << "my_timestamp: ss = " << ss <<std::endl;
  return ss;
}

std::string my_system( std::string command) {
  // run a process and create a streambuf that reads its stdout and stderr
  redi::ipstream proc(command, redi::pstreams::pstderr|redi::pstreams::pstdout);
  std::string line;
  // read child's stderr
  while (proc >> line)
    std::cout << "output: " << line << std::endl;
}

static void show_usage( std::string name ) {
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-t,--test\t\tTest functionality\n"
              << std::endl;
}

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
    }
  }

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

  std::vector<char> memblock(fsize);
  std::fread(&memblock[0], sizeof(char), memblock.size(), file);
  fclose(file);
  //print_chars(memblock, fsize);
  DEBUG && std::cout << "the entire cell content is in memory" << std::endl;

  // TODO repeat indefinitely in a later step
  for(int i = 0; i<5; i++){
    // determine number of bytes to get changed
    int cycles = 1; // TODO: rand() % 10;
    DEBUG && std::cout << "cycles = " << cycles << std::endl;

    // TODO set starting point to any of the active cells
    std::vector<char> loc_memblock = memblock;

    for(unsigned j=1; j <= cycles; j++){
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
        printf("delete byte at position %d\n", pos_del);
        loc_memblock.erase( loc_memblock.begin()+pos_del);
        //print_chars(loc_memblock, fsize);
      } else if (option == 1) {
        DEBUG && std::cout << "add random byte at random position" << std::endl;
        unsigned pos_add = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        printf("add byte %02hhx at position %d\n", val, pos_add);
        loc_memblock.insert( loc_memblock.begin()+pos_add, val);

        // unsigned size_add = int(fsize)+1;
        // std::vector<char> memblock3 (size_add);
        // for(unsigned i=0; i<size_add; i++){
        //   if(i < pos_add){
        //     memblock3[i] = loc_memblock[i];
        //   } else if(i == pos_add) {
        //     memblock3[i] = val;
        //   } else if(i > pos_add) {
        //     memblock3[i] = loc_memblock[i-1];
        //   }
        // }
        // fsize = size_add;
        // loc_memblock = memblock3;
        //print_chars(loc_memblock, fsize);
      } else if (option == 2) {
        DEBUG && std::cout << "change random byte" << std::endl;
        unsigned pos_change = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        printf("change byte at position %d to %02hhx\n", pos_change, val);
        loc_memblock[pos_change] = val;
        //print_chars(loc_memblock, fsize);
      } else {
        DEBUG && std::cout << "option must be one of 1,2,3, got " << option << std::endl;
        exit(1);
      }
    }
    DEBUG && std::cout << "finished changes" << std::endl;
    // write new cell: write memblock to new file, byte-wise
    std::string filename ("/tmp/cell/cell_");

    timeval tv;
    gettimeofday(&tv, NULL);
    // tv.tv_usec is long unsigned int
    // std::cout << "musec: " << tv.tv_usec << std::endl;
    std::string musec;
    std::stringstream strstream;
    strstream << tv.tv_usec;
    strstream >> musec;

    filename = filename + my_timestamp() + musec;
    DEBUG && std::cout << "filename with timestamp = " << filename << std::endl;

    FILE * outfile;
    outfile = fopen (filename.c_str(), "wb");
    fwrite (&loc_memblock[0] , sizeof(char), memblock.size(), outfile);
    fclose (outfile);

    /*     chmod 700 newcell */
    /*     # check whether cell was already tried, before trying to execute it */
    /*     hashi=$(md5sum newcell | cut -d' ' -f1) */
    /*     donetried=$(grep -c $hashi history) */
    /*     if (( $donetried > 0 )) */
    /*     then */
    /*         echo "tried already" */
    /*         continue */
    /*     fi */
    /*     echo '$hashi' >> history */
    /*     $(./newcell < input > output &> /dev/null; echo $? > exitcode ) & */
    /*     success=999 */
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

    /* # single out the new ones */
    /* cd backup */
    /* mkdir -p singles */
    /* ls cell* > filelist */
    /* cd /tmp/cell */
    /* cat backup/filelist | */
    /*     while read nextfile */
    /*     do */
    /*         hashi=$(md5sum backup/$nextfile | sort | cut -d' ' -f1) */
    /*         nof=$(grep -c $hashi recompiles/msum) */
    /*         if (( $nof > 0 )) */
    /*         then */
    /*             echo backup/$nextfile " already exists" */
    /*             rm backup/$nextfile */
    /*         else */
    /*             #echo backup/$nextfile " is different from all others!" */
    /*             mv backup/$nextfile singles */
    /*         fi */
    /*     done */

    /* cd /tmp/cell */
    /* mkdir -p recompiles */
    /* cd singles */
    /* ls cell* > filelist */
    /* cd /tmp/cell */
    /* echo "sorting out non-reproductive cells" */
    /* cat singles/filelist | */
    /*     while read nextfile */
    /*     do */
    /*         rm -f outcell */
    /*         touch outcell */
    /*         od -An -tx1 singles/$nextfile > progcell */
    /*         ./singles/$nextfile < progcell > outcell */
    /*         if diff singles/$nextfile outcell >/dev/null */
    /*         then */
    /*             echo "new recompiling: " $nextfile */
    /*             mv singles/$nextfile recompiles */
    /*         else */
    /*             rm singles/$nextfile */
    /*         fi */
    /*     done */

    /* cd /tmp/cell/recompiles */
    /* md5sum cell* | cut -d' ' -f1 > msum */
    /* cp /tmp/cell/recompiles/\* /home/psteger/dev/cell/recompiles */
    /* cp /tmp/cell/history /home/psteger/dev/cell/ */
  }
}
