#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>
#include <cassert>
#include "util.h"
//#define DEBUG

#ifdef DEBUG
#define DBG(x) std::cout << x;
	 //#include <string>
	 //using namespace std;
#else
#define DBG(x)
#endif



template <typename ITEM, size_t N>
class Array;

template <typename ITEM, size_t N>
class ArrayBlock{ // linked list of data blocks and control info
  friend class Array<ITEM,N>;
  ITEM data[N];
  //  uint start;
  ArrayBlock* next;
  uint refcount;
  ITEM fill;
  bool filling;

  ArrayBlock(void) : next(nullptr), refcount(0), filling(false) {}
  ArrayBlock(const ITEM& f) : fill(f), next(nullptr), refcount(0), filling(true) {
    for(uint i = 0;i < N;i++)data[i] = fill;
  }
  ArrayBlock(const ArrayBlock&); // no copy
  ArrayBlock& operator=(const ArrayBlock&); //no assignment

  ~ArrayBlock(void){
    if(next != nullptr)delete next;
  }

  uint len(void){
    return N + (next == nullptr? 0: next->len());
  }

  ITEM& operator[](uint i){
    DBG(format("ArrayBlock %x: operator [%d]\n",this,i))
    if(i >= N){ // on to next block
      DBG(format("index %d exceeds block length\n",i))
      fflush(stdout);
      if(next == nullptr){
	next = filling? new ArrayBlock(fill): new ArrayBlock();
	DBG(format("new ArrayBlock at %x\n",next))
	fflush(stdout);
      }
      return next->operator[](i-N);
      
    }
    return data[i];
  }
};

template<typename ITEM, size_t N> 
class Array {
  //  friend ostream& operator <<(ostream& os, const Array& A);
protected:
  ArrayBlock<ITEM,N>* first;
public:
  Array(void) : first(new ArrayBlock<ITEM,N>()){
    DBG(format("Array: first block at %x\n",first))
  }

  Array(const ITEM& fill) : first(new ArrayBlock<ITEM,N>(fill)){
    DBG(format("Array: first block at %x, filled with ",first) << str(fill))
  }


  Array(const Array& A): first(A.first) {first->refcount++;} // shallow copy
  ~Array(void){
    //    DBG("Array destructor: first =  %x, refcount = %d\n",first,first->refcount) 
    if( first->refcount-- <= 1)delete first;
  }
  Array& operator=(const Array& A){ // shallow copy -- switch pointer to new first block
    if(first->refcount-- <= 1)delete first;
    first = A.first;
    first-> refcount++;
    return *this;
  }
  Array copy(void) const{ // deep copy returning object
    Array<ITEM,N> A;
    int n = len(); // only call recursive len once
    for(int i = 0;i < n;i++) A[i] = this->operator[](i);
    return A;
  }
  int len(void) const {return first->len();}
  ITEM& operator[](int i)const {return first->operator[](i);}
  bool operator==(Array& A) const{
    int n = len();
    if(n != A.len())return false;
    for(int i = 0;i < n;i++) if(this->operator[](i) != A[i])return false;
    return true;
  }
  bool operator!=(Array& A) const{return !operator==(A);}
};

template<typename ITEM, size_t N>
std::ostream& operator <<(std::ostream& os, Array<ITEM,N>& A){
  int n = A.len();
  for(int i = 0;i < n;i++)os << A[i]<<" ";
  return os;
} 

#endif
/*#define REFCOUNT (((int*)buf)[0])
#define N (((int*)buf)[1])
#define DATA ((ITEM*)(buf+Hlen*sizeof(int)/sizeof(ITEM) + 1)) // Note: is this right?
*/
  
  // void reset(const ITEM* p){first->reset(p);}
  // void reset(const ITEM& fill){first->reset(fill);}
  // void reset(void){first->reset();}
  //   if(first != nullptr){
  //     if(n == N) return; //NOP
  //     if(--REFCOUNT <= 0)delete[] buf;
  //   }
  //   int nitems = n + Hlen*sizeof(int)/sizeof(ITEM) + 1;
  //   buf = new ITEM[nitems];
  //   DBG << format("allocated Array of %d items of length %d at %x\n",nitems,sizeof(ITEM),buf); 
  //   REFCOUNT = 1;
  //   N = n;
  // }
