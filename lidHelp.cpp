#include <iostream> 
#include <string>
#include <cstdlib>
#include "libHelp.h"
using namespace std;
void getHelp(){ 
  cout << "HEHEHEHEHLPLPPPP!!! \n";
  const char* author_ptr = getenv ("AUTHOR");
  const char* version_ptr = getenv ("VERSION");
  string author = " ";
  string version = " ";
  if (author_ptr){
    string author = author_ptr;
  }else{
    std::cerr<<"iinnnooo AUTHOR not get!!1!"<<"\n";
  }
  if (version_ptr){
    std::string version = version_ptr;
  }else{
    cerr<<"iinnnooo VERSION not get!!1!"<<"\n";
  }

  }