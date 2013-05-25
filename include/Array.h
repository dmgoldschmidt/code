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



template <typename ITEM>
class Array;

// template <typename ITEM, size_t N>
// class ArrayBlock{ // linked list of data blocks and control info
//   friend class Array<ITEM,N>;
//   ITEM data[N];
//   //  uint start;
//   ArrayBlock* next;
//   uint refcount;
//   ITEM fill;
//   bool filling;

//   ArrayBlock(void) : next(nullptr), refcount(0), filling(false) {}
//   ArrayBlock(const ITEM& f) : fill(f), next(nullptr), refcount(0), filling(true) {
//     for(uint i = 0;i < N;i++)data[i] = fill;
//   }
//   ArrayBlock(const ArrayBlock&); // no copy
//   ArrayBlock& operator=(const ArrayBlock&); //no assignment

//   ~ArrayBlock(void){
//     if(next != nullptr)delete next;
//   }

//   uint len(void){
//     return N + (next == nullptr? 0: next->len());
//   }

//   ITEM& operator[](uint i){
//     DBG(format("ArrayBlock %x: operator [%d]\n",this,i))
//     if(i >= N){ // on to next block
//       DBG(format("index %d exceeds block length\n",i))
//       fflush(stdout);
//       if(next == nullptr){
// 	next = filling? new ArrayBlock(fill): new ArrayBlock();
// 	DBG(format("new ArrayBlock at %x\n",next))
// 	fflush(stdout);
//       }
//       return next->operator[](i-N);
      
//     }
//     return data[i];
//   }
// };

template <typename ITEM>
class ArrayBlock{ 
  friend class Array<ITEM>; // private helper class for Array
  size_t n;
  ITEM *data;
  ArrayBlock* next;
  uint refcount;
  const ITEM* fill;

  ArrayBlock(size_t nn, const ITEM* f) : n(nn), next(nullptr), refcount(0), fill(f) {
    data = new ITEM[n];
    if(fill != nullptr)
      for(uint i = 0;i < n;i++)data[i] = *fill;
  }
  ArrayBlock(const ArrayBlock&); // no copy
  ArrayBlock& operator=(const ArrayBlock&); //no assignment

  ~ArrayBlock(void){
    delete[] data;
    if(next != nullptr)delete next;
  }

  uint len(void){
    return n + (next == nullptr? 0: next->len());
  }

  ITEM& operator[](uint i){
    DBG(format("ArrayBlock %x: operator [%d]\n",this,i))
    if(i >= n){ // on to the next block
      DBG(format("index %d exceeds block length\n",i))
      fflush(stdout);
      if(next == nullptr){
	next = new ArrayBlock(n,fill);
	DBG(format("new ArrayBlock at %x\n",next))
      }
      return next->operator[](i-n);
      
    }
    return data[i];
  }
};

template<typename ITEM> 
class Array {
  //  friend ostream& operator <<(ostream& os, const Array& A);
protected:
  ArrayBlock<ITEM>* first; // first (and possibly only) block of data and control info
public:
  Array(void) : first(nullptr){}
  Array(size_t n, const ITEM* fill = nullptr) : first(new ArrayBlock<ITEM>(n,fill)){
    DBG(format("Array: first block at %x",first))
  }
  Array(const Array& A): first(A.first) {if(first != nullptr) first->refcount++;} // shallow copy
  ~Array(void){
    //    DBG("Array destructor: first =  %x, refcount = %d\n",first,first->refcount) 
    if( first != nullptr && first->refcount-- <= 1)delete first;
  }
  void reset(size_t n, const ITEM* fill = nullptr){
    if(first != nullptr && first->refcount-- <= 1)delete first;
    first = new ArrayBlock<ITEM>(n,fill);
  }

  Array& operator=(const Array& A){ // shallow copy -- switch pointer to new first block
    if(first != nullptr && first->refcount-- <= 1)delete first;
    first = A.first;
    first-> refcount++;
    return *this;
  }
  Array copy(void) const{ // deep copy returning object 
    Array<ITEM> A;
    if(first != nullptr){
      A.reset(first->n,first->fill);
      int n = len(); // only call recursive len once
      for(int i = 0;i < n;i++) A[i] = this->operator[](i);
    }
    return A;
  }
  int len(void) const {
    if(first == nullptr) return 0;
    return first->len();
  }
  ITEM& operator[](int i)const {
    if(first == nullptr)throw "Array bounds error\n";
    return first->operator[](i);
  }
  bool operator==(Array& A) const{
    int n = len();
    if(n != A.len())return false;
    for(int i = 0;i < n;i++) if(this->operator[](i) != A[i])return false;
    return true;
  }
  bool operator!=(Array& A) const{return !operator==(A);}
};

template<typename ITEM>
std::ostream& operator <<(std::ostream& os, Array<ITEM>& A){
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
