#include <cmath>
#include "datastructures.hpp"

double calc_similarity(vuc foutmem, long foutsize, vuc fexpmem, long fexpsize){
      // calculate similarity as squared differences
      unsigned int len_max = std::max(foutsize, fexpsize);
      foutmem.resize(len_max);
      fexpmem.resize(len_max);
      double top=0.0;
      double suma=0.0;
      double sumb=0.0;
      for(unsigned int cl=0; cl<len_max; ++cl){
        top += foutmem[cl]*fexpmem[cl];
        suma += foutmem[cl]*foutmem[cl];
        sumb += fexpmem[cl]*fexpmem[cl];
      }
      double cosphi = top/(std::sqrt(suma)*std::sqrt(sumb));
      return cosphi;
}
