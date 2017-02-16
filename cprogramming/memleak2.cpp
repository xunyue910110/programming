#include <stdio.h>
#include <stdlib.h>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>
using namespace boost;

void getMem(boost::shared_array<int>& p){
  //p = boost::make_shared<int>(16);
  boost::shared_array<int> p1 ( new int[16]);
  //boost::shared_array<int> p1 = make_shared<int>( 16);
  p = p1;
  //p = new int[16];
}

int main(){
  boost::shared_array<int> p; 
  getMem(p);
  //free(p);
  //delete (int*)p;
  //delete [] (int*)p;
  //delete [] reinterpret_cast<int*>(p);
  //p = NULL;
  return 0;
}
