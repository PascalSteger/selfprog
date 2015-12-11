#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

bool equalFiles(std::ifstream& in1, std::ifstream& in2)
{
  std::ifstream::pos_type size1, size2;
  size1 = in1.seekg(0, std::ifstream::end).tellg();
  in1.seekg(0, std::ifstream::beg);
  size2 = in2.seekg(0, std::ifstream::end).tellg();
  in2.seekg(0, std::ifstream::beg);
  if(size1 != size2)
    return false;
  static const size_t BLOCKSIZE = 4096;
  size_t remaining = size1;
  while(remaining)
    {
      char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
      size_t size = std::min(BLOCKSIZE, remaining);
      in1.read(buffer1, size);
      in2.read(buffer2, size);
      if(0 != memcmp(buffer1, buffer2, size))
        return false;
      remaining -= size;
    }
  return true;
}

// int compare_files() {
//   FILE *fp1, *fp2;
//   int ch1, ch2;
//   char fname1[40], fname2[40];
//   printf("Enter name of first file :");
//   gets(fname1);
//   printf("Enter name of second file:");
//   gets(fname2);
//   fp1 = fopen(fname1, "r");
//   fp2 = fopen(fname2, "r");
//   if (fp1 == NULL) {
//     printf("Cannot open %s for reading ", fname1);
//     exit(1);
//   } else if (fp2 == NULL) {
//     printf("Cannot open %s for reading ", fname2);
//     exit(1);
//   } else {
//     ch1 = getc(fp1);
//     ch2 = getc(fp2);
//     while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
//       ch1 = getc(fp1);
//       ch2 = getc(fp2);
//     }
//     if (ch1 == ch2)
//       printf("Files are identical n");
//     else if (ch1 != ch2)
//       printf("Files are Not identical n");
//     fclose(fp1);
//     fclose(fp2);
//   }
//   return (0);
// }

int intelligence()
{
  int intel = 0;
  // produce input files
  /// first step: create random number of bytes in file
  std::ofstream infile;
  infile.open ("input.dat");
  infile << "37" << std::endl;
  infile << "42" << std::endl;
  infile.close();
  // produce output file
  std::ofstream outfile;
  outfile.open ("output.dat");
  outfile << "79" << std::endl;
  outfile.close();
  // run program on input file
  int outcode = system("./cell < input.dat > output.dat");
  // compare with output file
  try {
    std::ifstream in1("input.dat", std::ios::binary);
    std::ifstream in2("output.dat", std::ios::binary);
    if(equalFiles(in1, in2)) {
      intel = 100;
    }
    else {
      return 0;
    }
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    exit(-2);
  }
  // return intelligence
  return intel;
}
