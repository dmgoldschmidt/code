#include "util.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

// quick and dirty printf-type format string adapter for c++ streams
// usage, e.g:  cout << format("Now we can do formatted output such as six = %d, pi = %f, etc",6, 3.14159)<<endl;
// set MAXCHARS appropriately 

std::string format(const char* fstr,...){
  char buf[MAXCHARS+1]; // will truncate after MAXCHARS characters -- no buffer overflows
  va_list ptr;
  int nchars;

  va_start(ptr,fstr);
  nchars = vsnprintf(buf,MAXCHARS,fstr,ptr);
  //  std::cout << "format: vsnprintf returns "<< nchars<< std::endl;
  va_end(ptr);
  std::string output(buf);
  return output;
}

//***************************

std::string str_time(double t){
  int it = (int)t;
  time_t tt = it;
  double ft = t-it;
  const char* fmt = "%.2d:%.2d:%7.4f";

  struct tm* pl = localtime(&tt);
  //std::cout << "localtime sets isdst to "<<pl->tm_isdst<<std::endl;
  return format(fmt,pl->tm_hour,pl->tm_min,pl->tm_sec + ft);
}

std::string str_date(double t){
  time_t tt = (int)t;
  struct tm* pl = localtime(&tt);
  std::ostringstream s;
  
  s <<  pl->tm_mon+1 <<"/"<<pl->tm_mday<<"/"<<pl->tm_year+1900;
  return s.str();
}

double get_time(int date, std::string time){
  std::stringstream ss;
  struct tm pl;
  char buf[18];


  ss.str("");
  ss <<date<<time<<":00";
  strncpy(buf,ss.str().c_str(),18);
  //std::cout << "calling strptime with "<<buf<<std::endl;

  strptime(buf,"%Y%m%d%H:%M:%S",&pl);
  //std::cout <<"tm: "<<pl.tm_sec<<" "<<pl.tm_min<<" "<<pl.tm_hour<<" "<<pl.tm_mday<<" "<<pl.tm_mon<<" "<<pl.tm_year<<" "<<pl.tm_wday<<" "<<pl.tm_yday<<" "<<pl.tm_isdst<<std::endl;
  //std::cout << "get_time has isdst = "<<pl.tm_isdst<<std::endl;
  pl.tm_isdst = -1; // fixing a bug (design defect?) in strptime
  strftime(buf,sizeof(buf), "%s", &pl);
  //std::cout << "mktime returns "<<mktime(&pl)<<" strftime has "<<buf<<std::endl;
  return atof(buf);
}
