#ifndef UTIL_H
#define UTIL_H
#include<cstdarg>
#include<string>
#include<sstream>
#include<sys/time.h>
#include<time.h>
#define MAXCHARS 1000
#include <cmath>

std::string format(const char* fstr,...);
std::string str_time(double t);
std::string str_date(double t);
double get_time(int date, std::string time);
class InternalTime { // floating pt seconds <-> 38-bit integer microseconds (3.18 days = 2^38 microseconds)
  int a;
  char b;
  inline void convert(double delta){
    double n = delta*1000000/128;
    a = (int) n; // high-order signed 31 bits
    b = (char) ((n-a)*128); // low-order signed 7 bits
  }
public:
  InternalTime(double delta = 0){ 
    if(fabs(delta) > 259200) throw format("InternalTime: bad input %lf\n",delta); // abs(delta) >= 3 days
    convert(delta);
  }
  inline InternalTime& operator=(double delta){convert(delta); return *this;}
  inline InternalTime& operator=(const InternalTime& t){a = t.a;b = t.b; return *this;}
  inline operator double() const{return (a*128.0 + b)/1000000.0;}
};
#endif
