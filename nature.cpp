// nature.cpp: program to emulate evolution of digital life
// aim: produce super intelligence through natural selection

#include <iostream>      /* cin, cout */
#include <limits.h>      /* UCHAR_MAX, char_to_bin */
#include <map>           /* std::multimap */
#include <sys/stat.h>    /* S_IRUSR, S_IWUSR, S_IXUSR*/
#include <unistd.h>
#include <random>        /* for rand() */
#include <assert.h>

#include "paths.hpp"
#include "tests.hpp"
#include "intelligence.hpp"
#include "datastructures.hpp" /* typedefs, vuc*/
#include "similarity.hpp"     /* calc_similarity */
#include "mysystem.hpp"
#include "debug.hpp"
#include "myfiles.hpp"
#include "myrandom.hpp"
#include "myoutput.hpp"
#include "timestamp.hpp"
#include "myparams.hpp"
#include "md5.hpp"

int main(int argc, char* argv[]) {
  param_struct params = parse_params(argc, argv);
  status_struct now;

  initialize_random();

  setup_dirs();
  std::string random_file = find_random_starting_cell(); // find random file from directory with reproducing cells
  // random_file = "/home/psteger/dev/selfprog/cell";    // override cell name with known good one
  std::cout << ".. starting gene: " << random_file << std::endl;

  FILE * file = open_file(random_file);
  long fsize = find_filesize(file);
  assert(fsize>0);
  vuc memblock(fsize);
  std::fread(&memblock[0], sizeof(unsigned char), memblock.size(), file);
  fclose(file);

  print_debug( "the entire cell content is in memory" );
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
  for(unsigned int iteration = 0; iteration < params.Niterations; iteration++){
    now.N = iteration;
    std::cout << iteration << ": ";

    unsigned int cycles = get_cycles( params.pois_cycles );
    DEBUG && std::cout << "cycles = " << cycles << std::endl;

    vuc loc_memblock = memblock;
    assert(loc_memblock.size() > 0);
    for(unsigned int j=1; j <= cycles; j++){

      // choose one of three options:
      //  0:    add random byte at random position,
      //  1:    change byte at random position
      //  2:    delete byte from random position
      int option = rand()%3;
      if(option == 0){
        print_debug( "delete random byte" );
        if(loc_memblock.size() == 1){
          std::cerr << "Too short program: cannot delete byte" << std::endl;
          continue;
        }
        unsigned pos_del = rand()%loc_memblock.size();
        DEBUG && printf("delete byte at position %d\n", pos_del);
        loc_memblock.erase( loc_memblock.begin()+pos_del);
        //print_chars(loc_memblock, fsize);
      } else if (option == 1) {
        print_debug( "add random byte at random position" );
        unsigned pos_add = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        DEBUG && printf("add byte %02hhx at position %d\n", val, pos_add);
        loc_memblock.insert( loc_memblock.begin()+pos_add, val);
      } else if (option == 2) {
        print_debug( "change random byte" );
        unsigned pos_change = rand()%loc_memblock.size();
        char val = rand()%UCHAR_MAX;
        DEBUG && printf("change byte at position %d to %02hhx\n", pos_change, val);
        loc_memblock[pos_change] = val;
        //print_chars(loc_memblock, fsize);
      } else {
        std::cerr << "option must be one of 1,2,3, got " << option << std::endl;
        exit(1);
      }    }
    print_debug( "finished changes" );


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
    std::string filename = write_new_cell(loc_memblock);
    std::string filename_cp = filename;

    /********************  execute with sample input text ********************/
    std::string erroutput = "";
    erroutput = my_system("timeout 1s " + filename + " < "
                          + PATH_CELL+"input > "+ PATH_CELL + "output; echo $?");

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
    progcell = fopen ((PATH_CELL+"progcell").c_str(), "wb");
    // get char values one by one, in hex representation
    for(unsigned int k=0; k<loc_memblock.size(); k++){
      fprintf(progcell, " %02hhx", loc_memblock[k]);
    }
    fclose (progcell);


    /******************** check re-compilation of self ********************/
    my_system(filename + " < "+PATH_CELL+"progcell > "+PATH_CELL+"outcell");
    // in output of cell run on itself
    FILE * compfile;
    long outfsize = 0;
    compfile = fopen ( (PATH_CELL+"outcell").c_str() , "rb" );
    if (compfile==NULL) {fputs ("File error\n", stderr); exit (1);}
    fseek (compfile , 0 , SEEK_END);
    outfsize = ftell (compfile);
    rewind (compfile);
    vuc outblock(outfsize);
    std::fread(&outblock[0], sizeof(unsigned char), outblock.size(), compfile);
    fclose(compfile);
    print_debug( "the cell output content is in memory" );

    if(outblock == loc_memblock){
      has_not_reproduced = false;
      now.nu++;
      print_status(now);

      store_cell_in_reproduce_set(loc_memblock);

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
      std::cout << "  similarity: " << cosphi << std::endl;

      // TODO store similarity = fitness output alongside the output, e.g. in its name
      filename_cp += "_" + std::to_string(cosphi);
      my_system("mv " + filename + " " + filename_cp);
    } else {
      print_status(now);
    }

    remove(filename.c_str());
    remove((PATH_CELL + "outcell").c_str());
    remove((PATH_CELL + "output").c_str());
  }
  if(has_not_reproduced){  // after N iterations
    // remove uncooperative cell
    std::cout << std::endl << "cell " << random_file << " failed to get reproductive offspring, deleting" << std::endl;
    remove(random_file.c_str());
  } else {
    std::cout << std::endl << "cell " << random_file << " got reproductive offspring, keeping it" << std::endl;
  }
}
