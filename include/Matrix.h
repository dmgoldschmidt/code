#ifndef MATRIX_H
#define MATRIX_H
#include "Array.h"
#include "util.h"

#define COL_STRIDE (((int*)Base::buf)[2])
#define ENTRY (operator())
#define MDATA ((SCALAR*)(Base::buf+3*sizeof(int)/sizeof(SCALAR) + 1))


struct Givens{
  double sin_t;
  double cos_t;
  double h;
  double eps;

  Givens(double e = 1.0e-5) : eps(e){}
  
  bool reset(double& a0, double& b0){
    if(fabs(b0) < eps) return false;
    double a = a0*a0;
    double b = b0*b0;
    h = fabs(b < a? a0*sqrt(1+b/a) : b0*sqrt(1+a/b)); // numerical hygene for sqrt(a+b)
    cos_t = a0/h;
    sin_t = b0/h;
    a0 = h;
    b0 = 0;
    //    cout << "sin_t = "<<sin_t<<", cos_t = "<<cos_t<<endl;
    return true;
  }
   
  inline void rotate(double& x, double& y){
    double z = x;
    x = cos_t*z + sin_t*y;
    y = -sin_t*z + cos_t*y;
  }
};

template<typename SCALAR>
class Matrix : public Array<SCALAR,3> {
  typedef Array<SCALAR,3> Base;
  int offset;
  int _nrows;
  int _ncols;
public:
  Matrix(void) : Base(), offset(0), _nrows(0), _ncols(0) {}
  Matrix(int m, int n) : Base(m*n), offset(0), _nrows(m), _ncols(n) {COL_STRIDE = _ncols;}
  Matrix(int m, int n, SCALAR fill ): Base(m*n,fill), offset(0), _nrows(m), _ncols(n) {COL_STRIDE = _ncols;}
  Matrix(const Matrix& M) : Base(M), offset(M.offset), _nrows(M._nrows), _ncols(M._ncols) {} // shallow copy
  
  void reset(int m, int n){
    Base::reset(m*n);
    offset = 0;
    _nrows = m;
    COL_STRIDE = _ncols = n;
    
  }
  void reset(int m, int n, SCALAR fill){
    Base::reset(m*n,fill);
    offset = 0;
    _nrows = m;
    COL_STRIDE = _ncols = n;
  }
  Matrix copy(void) const{ // return a new deep copy
    Matrix M(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++) M(i,j) = ENTRY(i,j);
    }
    return M;
  }
  void copy(const Matrix& A){ // deep copy A to *this
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++) ENTRY(i,j) = A(i,j);
    }
  }
  Matrix submatrix(int i, int j, int m, int n) const{ // shallow copy mxn submatrix at (i,j)
    Matrix M(*this);
    M.offset = i*COL_STRIDE+j;
    M._nrows = m;
    M._ncols = n;
    return M;
  }
    
  SCALAR& operator()(int i, int j) const{return MDATA[offset + i*COL_STRIDE + j];}
  int nrows(void) const {return _nrows;}
  int ncols(void) const {return _ncols;}

  //*********

  Matrix operator+(const Matrix& A) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = ENTRY(i,j)+A(i,j);
    }
    return B;
  }
  Matrix operator-(const Matrix& A) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = ENTRY(i,j)-A(i,j);
    }
    return B;
  }
  Matrix operator*(const Matrix& A)const {
    Matrix B(_nrows,A._ncols);

    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < A._ncols;j++){
        B(i,j) = 0;
        for(int k = 0;k < _ncols;k++){
          B(i,j) += ENTRY(i,k)*A(k,j);
          //          cout << format("ENTRY(%d,%d) = %d, A(%d,%d) = %d, B(%d,%d) = %d\n",i,k,ENTRY(i,k),k,j,A(k,j),i,j,B(i,j));
        }
      }
    }
    return B;
  }
  Matrix operator-(void) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = -ENTRY(i,j);
    }
    return B;
  }
  Matrix T(void) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = ENTRY(j,i);
    }
    return B;
  }
        
          
 





#ifdef DEBUG
  void dump(void) const{
    cout << "nrows = "<<_nrows<<", ncols = "<<_ncols<<", offset = "<<offset<<endl<<
      "refcount = "<< (int)Base::buf[0]<<", col stride = "<<COL_STRIDE<<endl;
    cout << *this;
  }
#endif

}; // end Matrix template

template<typename SCALAR>
std::ostream& operator <<(std::ostream& os, const Matrix<SCALAR>& M){
  for(int i = 0;i < M.nrows();i++){
    for(int j = 0;j < M.ncols();j++)os << M(i,j)<<" ";
    os <<"\n";
  }
  return os;
}

// matrix routines for SCALAR=double

typedef Matrix<double> matrix;

int ut0(matrix& A, double eps = 1.0e-10){// upper-triangularize in place by Givens row rotations
  int nrot = 0;
  double cos_t, sin_t, h, a,b, tmp;
  for(int i = 1;i < A.nrows();i++){
    for(int j = 0;j < i;j++){
      if(A(i,j) > A(j,j)){ // numerical hygene for h = sqrt(A(i,j)^2 + A(j,j)^2
        a = A(i,j);
        b = A(j,j);
      }
      else{
        a = A(j,j);
        b = A(i,j);
      }
      if(a < eps) continue;
      h = a*sqrt(1+b*b/(a*a));
      nrot++;
      sin_t = A(j,j)/h;
      cos_t = A(i,j)/h;
      A(j,j) = h;
      A(i,j) = 0;
      for(int k = j+1;k < A.ncols();k++){
        tmp = sin_t*A(j,k) + cos_t*A(i,k);
        A(i,k) = -cos_t*A(j,k) + sin_t*A(i,k);
        A(j,k) = tmp;
      }
    }
  }
  return nrot;
}

int ut(matrix& A, double eps = 1.0e-10){// upper-triangularize in place by Givens row rotations
  int nrot = 0;
  Givens R;

  for(int i = 1;i < A.nrows();i++){
    for(int j = 0;j < i;j++){
      if(!R.reset(A(j,j),A(i,j))) continue;
      nrot++;
      for(int k = j+1;k < A.ncols();k++){
	R.rotate(A(j,k),A(i,k));
      }
    }
  }
  return nrot;
}



matrix qr(const matrix& A){
  matrix QR(A.nrows(),A.nrows()+A.ncols(),0);
  matrix Q(QR.submatrix(0,A.ncols(),A.nrows(),A.nrows()));
  matrix R(QR.submatrix(0,0,A.nrows(),A.ncols()));

  R.copy(A);
  for(int i = 0;i < Q.nrows();i++)Q(i,i) = 1.0; // initialize Q to I
  ut(QR);
  return QR;
}


void reduce(matrix& A, double eps = 1.0e-10){ // row-reduce upper-triangular A in place
  int n = A.nrows();
  double a,b;

  for(int i = 0;i < n;i++){
    if( (b = A(i,i)) < eps)throw "solve: Matrix is singular\n";
    A(i,i) = 1.0;
    for(int j = i+1;j < A.ncols();j++) A(i,j) /= b;
    for(int k = 0;k < i;k++){
      a = A(k,i);
      A(k,i) = 0;
      for(int j = i+1;j < A.ncols();j++) A(k,j) -= a*A(i,j);
      //      cout << format("\nk,i = (%d,%d) A:\n",k,i)<<A;
    }
  }
}

void solve(matrix& A, double eps = 1.0e-10){ // solve linear equations in-place 
  // A is an m x (m+k) matrix of m equations (in m unknows) with k right-hand sides
  ut(A); // rotate  to upper-triangular form;
  reduce(A,eps); // now row-reduce coefficients to identity.  Each rhs is now solved
}

matrix inv(const matrix& A, double eps = 1.0e-10){
  matrix QR = qr(A); // rotate to upper-triangular form
  // and append the rotation matrix (as extra columns)
  int n = QR.nrows();

  reduce(QR,eps); // now row-reduce
  return QR.submatrix(0,n,n,n); // return the left-most n columns
}


Array<matrix> svd(const matrix& A, double eps = 1.0e-10, int maxiters = 10){
  Array<matrix> QR(2);
  Array<matrix> R(2);
  int j,niters,n = min(A.nrows(),A.ncols());
  double delta;

  QR[0] = qr(A);
  R[0] = QR[0].submatrix(0,0,A.nrows(),A.ncols());
  QR[1] = qr(R[0].T());
  R[1] = QR[1].submatrix(0,0,A.ncols(),A.nrows());
  j = 0;
  niters = 0;
  do{
    R[j].copy(R[1-j].T());
    ut(QR[j]);
    delta = 0;
    for(int i = 0;i < n;i++){
      if(QR[j](i,i) < eps)continue;
      delta += fabs(QR[j](i,i) - QR[1-j](i,i))/QR[j](i,i);
    }
    delta /= n;
    //    cout << "iteration "<<niters<<": delta = "<<delta<<endl;
    //    cout << format("R[%d]:\n",j)<<R[j];
    j = 1-j;
  }while(delta > eps && ++niters < maxiters);
  return QR;
}

struct Svd{
  int ncols;
  int nrows;
  matrix AUV;
  matrix AU;
  matrix AV;
  matrix U;
  matrix V;
  matrix A;

  void reduce(const matrix& A0){
    Givens R;

    nrows = A0.nrows();
    ncols = A0.ncols();
    AUV.reset(nrows+ncols,nrows+ncols,0);
    // define the submatrices
    AU = AUV.submatrix(0,0,nrows,ncols+nrows);
    AV = AUV.submatrix(0,0,nrows+ncols,ncols);
    U = AUV.submatrix(0,ncols,nrows,nrows);
    V = AUV.submatrix(nrows,0,ncols,ncols);
    A = AUV.submatrix(0,0,nrows,ncols);

    A.copy(A0); // copy in the input
    for(int i = 0;i < nrows;i++)U(i,i) = 1.0; // set rotation matrices = identity
    for(int i = 0;i < ncols;i++)V(i,i) = 1.0;
    ut(AU); // row rotate to upper triangular form 
    for(int i = 0;i < nrows-1;i++){ // zero the ith row above the super-diagonal with column rotations
      for(int j = ncols-1;j > i+1;j--){ // rotate columns j and j-1 to zero out AV(i,j)
	if(!R.reset(AV(i,j-1),AV(i,j)))continue;
	AV(i,j-1) = R.h; AV(i,j) = 0;
	for(int k = i+1;k < nrows+ncols;k++)
	  R.rotate(AV(k,j-1),AV(k,j));
	if(!R.reset(AU(j-1,j-1),AU(j,j-1)))continue;
	// remove resulting lower-triangular "residue" with a row rotation
	AU(j-1,j-1) = R.h;
	AU(j,j-1) = 0;
	for(int k = j;k < nrows+ncols;k++)
	  R.rotate(AU(j-1,k),AU(j,k));
      }
    }
    // OK, A is now upper triangular with only the first super-diagonal non-zero
    int  maxiters = 1000;
    bool doit,not_done;
    for(int niters = 0;niters < maxiters;niters++){
      not_done = false;
      for(int j = 0;j < ncols-1;j++){
	if(doit = R.reset(A(j,j),A(j,j+1))){
	  for(int k = j+1;k < nrows+ncols;k++)
	    R.rotate(A(k,j),A(k,j+1));
	}
	not_done |= doit;
      }
      if(!not_done)break;
      not_done = false;
      for(int i = 0;i < ncols-1;i++){
	if(doit = R.reset(A(i,i),A(i+1,i))){
	  for(int k = i+1;k < nrows+ncols;k++)
	    R.rotate(A(i,k),A(i+1,k));
	}
	not_done |= doit;
      }
      if(!not_done)break;
      //      cout <<"iteration "<<niters<<"\n"<<A;
    }
  }
};
    

Array<matrix> svd1(const matrix& A, double eps = 1.0e-10, int maxiters = 10){
  Array<matrix> QR(2);
  Array<matrix> R(2);
  int j,niters,n = min(A.nrows(),A.ncols());
  double delta;

  QR[0] = qr(A);
  R[0] = QR[0].submatrix(0,0,A.nrows(),A.ncols());
  QR[1] = qr(R[0].T());
  R[1] = QR[1].submatrix(0,0,A.ncols(),A.nrows());
  j = 0;
  niters = 0;
  do{
    R[j].copy(R[1-j].T());
    ut(QR[j]);
    delta = 0;
    for(int i = 0;i < n;i++){
      if(QR[j](i,i) < eps)continue;
      delta += fabs(QR[j](i,i) - QR[1-j](i,i))/QR[j](i,i);
    }
    delta /= n;
    //    cout << "iteration "<<niters<<": delta = "<<delta<<endl;
    //    cout << format("R[%d]:\n",j)<<R[j];
    j = 1-j;
  }while(delta > eps && ++niters < maxiters);
  return QR;
}
                                

 

#endif
