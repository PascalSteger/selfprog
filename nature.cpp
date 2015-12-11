// nature.cpp: program to emulate evolution of digital life
// aim: produce super intelligence through natural selection

# include <stdio.h>
# include <iostream>     /* cin, cout */
# include <fstream>      /* ifstream */
# include <stdlib.h>     /* exit, EXIT_FAILURE, srand, rand */
# include <time.h>       /* time */
# include <limits.h>     /* UCHAR_MAX */

# include "tests.hpp"
# include "helpers.hpp"
# include "intelligence.hpp"

static void show_usage( std::string name ) {
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-t,--test\t\tTest functionality\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
  // initialize to always the same random number
  srand (1836); // TODO: if working, use srand(time(NULL))

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

  // TODO repeat indefinitely in a later step
  for(int i = 0; i<200; i++){
    /*     cp recompiles/$(ls recompiles|shuf|tail -n1) cell */
    /*     cp cell newcell */
    /*     cp cell tmpcell */
    /*     echo "" > output */
    /*     rm -f exitcode */
    /*     success=999 */

    // determine number of bytes to get changed
    int cycles = rand() % 10;
    if(cycles > 2) {
      cycles = 1;
    } else {
      //int len = $(wc -c tmpcell | cut -d" " -f1);
      cycles = (rand() % 10) + 1;
    }

    // start from given cell
    std::streampos size;
    char * memblock;

    // ios::ate (pointer at end) allows tellg() to directly give filesize
    std::ifstream file ("cell", std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open())
      {
        std::cout << "Unable to open file" << std::endl;
        exit(EXIT_FAILURE);
      }
    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, std::ios::beg);
    file.read (memblock, size);
    file.close();
    std::cout << "the entire cell content is in memory" << std::endl;
    //print_chars(memblock, size);

    for(int j=1; j <= cycles; j++){
      // choose of three options: add random byte at random position, */
      // change byte at random position
      // delete byte from random position
      int option = rand()%3;
      if(option == 0){
        std::cout << "delete random byte" << std::endl;
        if(size == 1){
          std::cerr << "Too short program" << std::endl;
          continue;
        }
        unsigned pos_del = rand()%size;
        printf("delete byte at position %d\n", pos_del);
        unsigned size_del = int(size)-1;
        char * memblock2;
        memblock2 = new char [size_del];
        for(int i=0; i<size; i++){
          if(i < pos_del){
            memblock2[i] = memblock[i];
          } else {
            memblock2[i] = memblock[i+1];
          }
        }
        size = size_del;
        memblock = new char [size_del];
        memblock = memblock2;
        //print_chars(memblock, size);
      } else if (option == 1) {
        // add random byte at random position
        unsigned pos_add = rand()%size;
        char val = rand()%UCHAR_MAX;
        printf("add byte %02hhx at position %d\n", val, pos_add);
        unsigned size_add = int(size)+1;
        char * memblock3;
        memblock3 = new char [size_add];
        for(int i=0; i<size_add; i++){
          if(i < pos_add){
            memblock3[i] = memblock[i];
          } else if(i == pos_add) {
            memblock3[i] = val;
          } else if(i > pos_add) {
            memblock3[i] = memblock[i-1];
          }
        }
        size = size_add;
        memblock = new char [size_add];
        memblock = memblock3;
        //print_chars(memblock, size);
      } else if (option == 2) {
        std::cout << "change random byte" << std::endl;
        unsigned pos_change = rand()%size;
        char val = rand()%UCHAR_MAX;
        printf("change byte at position %d to %02hhx\n", pos_change, val);
        memblock[pos_change] = val;
        //print_chars(memblock, size);
      } else {
        std::cout << "option must be one of 1,2,3, got " << option << std::endl;
        exit(1);
      }
      /*         #cp newcell tmpcell */
    }

    // write new cell
    // write memblock to new file, byte-wise
    std::ofstream outfile ("/tmp/cell/newcell", std::ios::out|std::ios::binary);
    if (!outfile.is_open())
      {
        std::cout << "Unable to open file /tmp/cell/newcell" << std::endl;
        exit(EXIT_FAILURE);
      }
    outfile.write (memblock, size);
    outfile.close();
    std::cout << "newcell written" << std::endl;
    delete[] memblock;

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
