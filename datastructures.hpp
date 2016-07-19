#ifndef __datastructures__
#define __datastructures__

#include <vector>

typedef std::vector<unsigned char> vuc;

struct param_struct {
  unsigned Niterations;
  float pois_cycles;};

struct status_struct {
  unsigned N;
  unsigned poolsize;
  unsigned nu = 0;
};

#endif
