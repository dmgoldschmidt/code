#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>
#include <cassert>
#define DEBUG

#ifdef DEBUG
#include <string>
using namespace std;
#endif

#define REFCOUNT (((int*)buf)[0])
#define N (((int*)buf)[1])
#define DATA ((ITEM*)(buf+Hlen*sizeof(int)/sizeof(ITEM) + 1))
template<typename ITEM, int Hlen = 2> // Hlen is the no. of integer header items
class Array {
  //  friend ostream& operator <<(ostream& os, const Array& A);
protected:
  ITEM* buf;
public:
  Array(void) : buf(NULL){}
  Array(int n) : buf(NULL){reset(n);}
  Array(int n, ITEM fill) : buf(NULL){reset(n,fill);}
  Array(int n, const ITEM* p): buf(NULL){
    reset(n);
    for(int i = 0;i < n;i++) DATA[i] = p[i];
  }
  void reset(int n, ITEM fill){
    reset(n);
    for(int i = 0;i < n;i++)DATA[i] = fill;
  }
  void reset(int n){
#ifdef ARRAY_BOUNDS
    if(n < 0) throw "Array: negative length\n";
#endif
    if(buf != NULL){
      if(n == N) return; //NOP
      if(--REFCOUNT <= 0)delete[] buf;
    }
    buf = new ITEM[n + Hlen*sizeof(int)/sizeof(ITEM) + 1];
    //    cout << format("allocated %d items of length %d at %x\n",n,sizeof(ITEM),buf); 
    REFCOUNT = 1;
    N = n;
  }
  Array(const Array& A): buf(A.buf) {if(buf != NULL)REFCOUNT++;} // shallow copy
  ~Array(void){
    if(buf != NULL && --REFCOUNT <= 0)delete[] buf;
    //    cout << format("deleted %x\n",buf); 
  }
  Array& operator=(const Array& A){ // shallow copy
    if(buf != NULL && --REFCOUNT <= 0)delete[] buf;
    buf = A.buf;
    if(buf != NULL) REFCOUNT++;
    return *this;
  }
  Array copy(void) const{ // deep copy returning object
    if(buf == NULL){
      Array A;
      return A;
    }
    Array A(N);
    for(int i = 0;i < N;i++) A[i] = DATA[i];
    return A;
  }
  ITEM& operator[](int i){
#ifdef ARRAY_BOUNDS
    if(buf == NULL) throw "Array: can't access uninitialized Array\n";
    if(i < 0 || i >= N) throw "Array: index out of bounds\n";
#endif
    return DATA[i];
  }
  bool operator==(Array& A) const{
    if(N != ((int*)(A.buf))[1])return false;
    for(int i = 0;i < N;i++) if(DATA[i] != A[i])return false;
    return true;
  }
  bool operator!=(Array& A) const{return !operator==(A);}
  int len(void) const {
    if(buf == NULL) return 0;
    return N;
  }

#ifdef DEBUG
  void dump(string s = ""){
    cout << s<<": refcount: "<<REFCOUNT<<" N: "<<N<<" DATA: ";
    for(int i = 0;i < N;i++)cout << DATA[i]<<" ";
    cout << endl;
  }
#endif

};
template<typename ITEM>
std::ostream& operator <<(std::ostream& os, Array<ITEM>& A){
  for(int i = 0;i < A.len();i++)os << A[i]<<" ";
  return os;
} 


#endif
  
