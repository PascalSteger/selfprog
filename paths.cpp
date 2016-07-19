#include "paths.hpp"
#include "mysystem.hpp"

void setup_dirs( void ){
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

void define_sample_input( void ){
  FILE * input;
  input = fopen ("/tmp/cell/input", "w");
  std::string strin ("111");
  fwrite (&(strin.c_str())[0] , sizeof(char), strin.size(), input);
  fclose (input);}
