#include "Array.h"
#include <iostream>

using namespace std;
void printit(Array<double> A){
  A.dump("printit");
}
int main(int argc, char** argv){
  Array<double> A(10);
  for(int i = 0;i < A.len();i++)A[i] = i;
  A.dump("A");
  printit(A);
  Array<double> B(A),C;
  B.dump("B");
  C = B;
  C.dump("C");
  Array<double> D = C.copy();
  A.dump("A");
  B.dump("B");
  C.dump("C");
  D.dump("D");
  Array<double> E;
  E.reset(5);
  E.dump("E blank");
  E = D.copy();
  E.dump("E = D.copy()");
  C.reset(5,-1.0);
  C.dump("C.reset(5)");
  B.dump("B refcount = 2?");
}
