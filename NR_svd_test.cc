#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Matrix.h"
#include "Heap.h"

#include "nr_include/nr3.h"
#include "nr_include/ran.h"
#include "nr_include/gamma.h"
#include "nr_include/deviates.h"

#include "nr_include/svd.h"

double diffclock(clock_t clock1,clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
} 

int main(int argc, char** argv){
  int nrows = atoi(argv[1]);
  int ncols = atoi(argv[2]);
  int seed = 12345;

  Normaldev normal(0,1,seed);
  matrix R(nrows,ncols);
  MatDoub R_nr(nrows,ncols);
  for(int i = 0;i < nrows;i++){
    for(int j = 0;j < ncols;j++)R_nr[i][j] = R(i,j) = normal.dev();
  }
  clock_t begin, end; 
  begin = clock();
  SVD A(R_nr);
  end = clock();
  cout << "NR SVD: " << diffclock(end,begin)<<" msec.\n";
  Svd S;
  begin = clock();
  S.reduce(R);
  end = clock();
  cout << "svd: " << diffclock(end,begin)<< " msec.\n";
}

