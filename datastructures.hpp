#ifndef __datastructures__
#define __datastructures__

#include <vector>

typedef std::vector<unsigned char> vuc;

struct param_struct {
  unsigned int Niterations;
  unsigned int Nlitters = 3;
  float pois_cycles;};

struct status_struct {
  unsigned litter;
  unsigned N;
  unsigned poolsize;
  unsigned nu = 0;
};

#endif
