#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Matrix.h"
#include "Heap.h"

#include "nr_include/nr3.h"
#include "nr_include/ran.h"
#include "nr_include/gamma.h"
#include "nr_include/deviates.h"

int main(int argc, char** argv){
  int nrows = atoi(argv[1]);
  int ncols = atoi(argv[2]);
  int seed = 12345;
  Matrix<int> M(nrows,ncols);
  for(int i = 0;i < nrows;i++){
    for(int j = 0;j < ncols;j++) M(i,j) = 10*i + j;
  }
  cout << "M:\n"<<M;
  Matrix<int> A,B;
  A = M;
  cout <<"A:\n"<<A;
  B = A.copy();
  B(1,1) = -1;
  cout << "A after copy:\n"<<A;
  cout <<"B:\n"<<B;
  Matrix<int> C = B.submatrix(1,1,2,2);
  cout << "B(1,1,2,2):\n";
  C.dump();
  M.reset(2,2);
  M(0,0) = 1;M(0,1) = 2;
  M(1,0) = 3;M(1,1) = 4;
  cout << "M:\n"<<M;
  //Matrix<int> M1(M+M);
  cout << "M+M:\n"<<(M+M);
  cout << "M*M:\n"<<(M*M);
  Matrix<double> D(nrows,nrows);
  for(int i = 0;i < nrows;i++){
    for(int j = 0;j < nrows;j++)D(i,j) = 1.0/(i+j+1);
  }
  cout <<"D:\n"<<D;
  Matrix<double> QR = qr(D);
  cout << "QR:\n"<<QR;
  Matrix<double> Q,R;
  Q = QR.submatrix(0,nrows,nrows,nrows);
  R = QR.submatrix(0,0,nrows,nrows);
  cout << "Q*D:\n"<<Q*D<<"Q^T*Q:\n"<<Q.T()*Q;


  try{
    cout << "D^{-1}:\n"<<inv(D);
    cout << "D^{-1}*D:\n"<<inv(D)*D;
  }
  catch(char const* s){
    cout <<"Caught: "<< s;
  }
  fflush(stdout);
  Array<matrix > S = svd(D);
  cout << "svd(D):\n"<<S[0]<<"*****\n"<<S[1];
  fflush(stdout);
  //  cout << "S[0].dump():\n"; S[0].dump();
  matrix U = S[0].submatrix(0,nrows,nrows,nrows);
  matrix V = S[1].submatrix(0,nrows,nrows,nrows);
  cout << "U:\n"<<U<<"V:\n"<<V<<"U*D*V^T:\n"<<U*D*V.T()<<"\n**********************\n";




  for(int i = 0;i < nrows;i++){
    for(int j = 0;j < nrows;j++)D(nrows-i-1,nrows-j-1) = 1.0/(i+j+1);
  }
  cout <<"D:\n"<<D;
  QR = qr(D);
  cout << "QR:\n"<<QR;
  Q = QR.submatrix(0,nrows,nrows,nrows);
  R = QR.submatrix(0,0,nrows,nrows);
  cout << "Q*D:\n"<<Q*D<<"Q^T*Q:\n"<<Q.T()*Q;
  try{
    cout << "D^{-1}:\n"<<inv(D);
    cout << "D^{-1}*D:\n"<<inv(D)*D;
  }
  catch(char const* s){
    cout <<"Caught: "<< s;
  }
  fflush(stdout);
  S = svd(D);
  cout << "svd(D):\n"<<S[0]<<"*****\n"<<S[1];
  fflush(stdout);
  //  cout << "S[0].dump():\n"; S[0].dump();
  U = S[0].submatrix(0,nrows,nrows,nrows);
  V = S[1].submatrix(0,nrows,nrows,nrows);
  cout << "U:\n"<<U<<"V:\n"<<V<<"U*D*V^T:\n"<<U*D*V.T()<<"\n**********************\n";

  Normaldev normal(0,1,seed);
  nrows = ncols = 10;
  R.reset(nrows,ncols);
  for(int i = 0;i < nrows;i++){
    for(int j = 0;j < ncols;j++)R(i,j) = normal.dev();
  }
  cout << "begin SVD:\n";
  Svd S1;
  S1.reduce(R);
  cout << "S1.A:\n"<<S1.A;
  cout << "U:\n"<<S1.U<<"V:\n"<<S1.V<<"U*R*V:\n"<<S1.U*R*S1.V<<endl;
  exit(0);
  
  S = svd(R,1.0e-6,100);
  Heap<double> sing_vals(100);
  for(int i = 0;i < 100;i++)sing_vals.add(S[0](i,i));
  sing_vals.sort();
  for(int i = 0;i < 100;i++) cout << format("sing. val[%d]: %f\n",i,sing_vals[i]);
  
}
