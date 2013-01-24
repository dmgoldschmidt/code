#include "String.h"
#include <iostream>

int main(int argc, char** argv){
  String s1("cba");
  String s2;

  s2 = "a";
  cout <<"s1: "<<s1<<", s2: "<<s2<<endl;
  if(s1 <= s2)cout << "s1 <= s2\n";
  else cout << "s1 >  s2\n";

}
