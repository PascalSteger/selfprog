#include <fstream>
#include "paths.hpp"
#include "mysystem.hpp"

void setup_dirs( void ){
  my_mkdir(PATH_CELL);
  my_mkdir(PATH_REPRODUCE);
  // clean directory to contain no previous files
  my_system("rm " + PATH_CELL + "cell_*");
  // seed new directory with at least one reproducing program
  my_system("cp " + PATH_PROG + "cell "+ PATH_REPRODUCE);
  // TODO copy meaningful files input_111 and output, after debugging
  my_system("cp "+PATH_PROG+"input_111 "+PATH_CELL+"input");
  my_system("cp "+PATH_PROG+"expect_111 "+PATH_CELL+"expect");
  my_mkdir(PATH_CELL+"backup");}

void define_sample_input( void ){
  FILE * input;
  input = fopen( (PATH_CELL+"input").c_str(), "w" );
  std::string strin ( "111" );
  fwrite (&(strin.c_str())[0] , sizeof(char), strin.size(), input);
  fclose (input);}
