#include <iostream>
#include <stdlib.h>
#include "DataSet.h"
//#include <sys/time.h>
using namespace std;
using namespace dataset;
int main(int argc, char *argv[])
{




  ShmAlloc  shmAlloc("/app/ebossdev/crm/src/acctmanm/src/tmp/testmm",4096*100);
  if (!shmAlloc.attach()){
   cout <<"share memory attach fail!" <<endl;
   return -1;}
DataSet ds(shmAlloc);



cout <<"reading ..." <<endl;
for (DataSet::iterator sit = ds.begin(); sit != ds.end(); ++sit)
;//	  cout << (*sit)[0] <<"," <<(*sit)[1]<<"," <<(*sit)[2]<<"," <<(*sit)[3]<<endl;
  
cout << "ds.size=" << ds.size() << "read ok!" <<endl;
 

  return 0;
}
