#ifndef __datastructures__
#define __datastructures__

#include <vector>

typedef std::vector<unsigned char> vuc;

struct param_struct {
  unsigned int N_MAX_ITERATIONS = 1000;
  unsigned int N_LITTERS = 10;
  unsigned int N_PER_LITTER = 6;
  unsigned int N_CONCURRENT = 1000;
  float pois_cycles;};

struct status_struct {
  unsigned litter;
  unsigned N;
  unsigned poolsize;
  unsigned nu = 0;
};

#endif
