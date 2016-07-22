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
  std::cout << ".. starting gene: " << random_file << std::endl;

  FILE * file = open_file(random_file);
  long fsize = find_filesize(file);
  assert(fsize>0);
  vuc memblock(fsize);
  std::fread(&memblock[0], sizeof(unsigned char), memblock.size(), file);
  fclose(file);

  print_debug( "the entire cell content is in memory" );
  sleep(1);

  /********************  define genepool  ********************/
  // need std::multimap for storing multiple std::vector values at same key
  std::multimap<vuc, vuc> genepool;
  vuc vmyha = my_hash(memblock);
  genepool.insert(std::pair<vuc, vuc>(vmyha, memblock));

  for(unsigned int litter = 0; litter < params.Nlitters; litter++){
    now.litter = litter;
    unsigned int successful_reproduction = 0;
    for(unsigned int iteration = 0; iteration < params.Niterations; iteration++){
      now.N = iteration;
      unsigned int cycles = get_cycles( params.pois_cycles );

      vuc loc_memblock = memblock;
      for(unsigned int j=1; j <= cycles; j++){
        int option = rand()%3;
        if(option == 0){
          print_debug( "delete random byte" );
          if(loc_memblock.size() == 1){
            std::cerr << "Too short program: cannot delete byte" << std::endl;
            continue;
          }
          unsigned pos_del = rand()%loc_memblock.size();
          if(DEBUG) printf("delete byte at position %d\n", pos_del);
          loc_memblock.erase( loc_memblock.begin()+pos_del);
        } else if (option == 1) {
          print_debug( "add random byte at random position" );
          unsigned pos_add = rand()%loc_memblock.size();
          char val = rand()%UCHAR_MAX;
          if(DEBUG) printf("add byte %02hhx at position %d\n", val, pos_add);
          loc_memblock.insert( loc_memblock.begin()+pos_add, val);
        } else if (option == 2) {
          print_debug( "change random byte" );
          unsigned pos_change = rand()%loc_memblock.size();
          char val = rand()%UCHAR_MAX;
          if(DEBUG) printf("change byte at position %d to %02hhx\n", pos_change, val);
          loc_memblock[pos_change] = val;
        } else {
          std::cerr << "option must be one of 1,2,3, got " << option << std::endl;
          exit(1);
        }    }
      print_debug( "finished changes" );

      /********************  compare to old genes   ********************/
      vuc vmyha = my_hash(loc_memblock);    // get hash of gene = vuc
      std::pair <std::multimap<vuc,vuc>::iterator, std::multimap<vuc,vuc>::iterator> ret;
      ret = genepool.equal_range(vmyha);
      bool found_old = false;
      for (std::multimap<vuc,vuc>::iterator it=ret.first; it!=ret.second; ++it){
        // compare new vuc to all previous ones. if old found => continue
        if(loc_memblock == it->second){
          print_status(now);
          found_old = true;
          break;
        }
      }
      if(found_old) continue;
      genepool.insert(std::pair<vuc, vuc>(vmyha, loc_memblock));
      now.poolsize = genepool.size();

      std::string filename = write_new_cell(loc_memblock);
      std::string filename_cp = filename;

      /********************  execute with sample input text ********************/
      std::string erroutput = my_system("timeout 1s " + filename + " < "
                                        + PATH_CELL + "input > "+ PATH_CELL + "output; echo $?");
      if(std::atoi(erroutput.c_str()) == 124){
        std::cout << "######## program took too long, aborting #######\r" << std::flush;
        remove(filename.c_str());
        print_status(now);
        continue;
      }

      /******************** check re-compilation of self ********************/
      write_progcell(loc_memblock);
      my_system(filename + " < " + PATH_CELL + "progcell > " + PATH_CELL + "outcell");
      vuc outblock = check_reproductive();

      if(check_reproduction_performance(loc_memblock, outblock) > 0.95){
        successful_reproduction += 1;
        now.nu++;
        print_status(now);

        double cosphi = check_training_performance();
        std::cout << "  similarity: " << cosphi << std::endl;

        // store fitness = similarity alongside the output, e.g. in its name
        filename_cp += "_" + std::to_string(cosphi);
        my_system("mv " + filename + " " + filename_cp);
        chmod(filename_cp.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
        my_system("mv " + filename_cp + " " + PATH_REPRODUCE);
      } else {
        print_status(now);
      }

      remove(filename.c_str());
      remove((PATH_CELL + "outcell").c_str());
      remove((PATH_CELL + "output").c_str());
      remove((PATH_CELL + "progcell").c_str());

      if(successful_reproduction > N_LITTER){
        break;
      }
    } // end for loop on iteration

    /************************ cleanup ****************************************/
    if(successful_reproduction == 0){
      remove(random_file.c_str());
      std::cout << std::endl << "cell " << random_file << " failed to get reproductive offspring, deleting" << std::endl;
    }
  } // end for loop on litter

  restrict_cell_population();
}
