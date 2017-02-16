#include <iostream>
#include <stdlib.h>
#include "DataSet.h"
//#include <sys/time.h>
using namespace std;
using namespace dataset;
int main(int argc, char *argv[])
{

bool ok = true;
if (argc > 1)
   if (strcmp(argv[1],"0")==0) ok = false;

  ShmAlloc  shmAlloc("/app/ebossdev/crm/src/acctmanm/src/tmp/testmm",4096*5,ok);
  if (!shmAlloc.allocate()){ //如果已经分配自动attach.
   cout <<"share memory alloc fail!" <<endl;
   return -1;}
DataSet ds(shmAlloc);

  ds.addField(Field("field1",FTSTRING,20));
  ds.addField(Field("field2",FTINT,16));
  ds.addField(Field("field3",FTSTRING,10));
  ds.addField(Field("field4",FTDOUBLE,12));

  int COUNT= 100;
 // ds.reserve(COUNT);



 cout << "Inserting data ..." <<endl;
  DataSet::iterator it;
  for (int i = 0; i < COUNT; ++i)
  {
      it = ds.pushBack();
      if (it == ds.end()) break;
      char buftmp[20] = "\0";
      sprintf(buftmp,"songzj%d",random());
      (*it)[0] = buftmp;
      (*it)[1] =i; 
      char buffer[10]="\0";
     sprintf(buffer, "%d", random());
       (*it)[2] =  buffer;
	(*it)[3] = (double)(i/12345);
  }
cout <<"DataSet size=" << ds.size() <<endl;
 
 //gettimeofday(&t1,NULL);
//cout <<"start sort ..." <<endl;
//ds.sort(1,"field1",ASC);
//cout <<"sort ok!" <<endl; 
if(ds.buildIndex("index1","field1",ASC))
  cout << "buildIndex ok " <<endl;

for (DataSet::index_iterator sit = ds.getIndexBegin(); sit != ds.getIndexEnd(); ++sit)
;//	  cout << (*sit)[0] <<"," <<(*sit)[1]<<"," <<(*sit)[2]<<"," <<(*sit)[3]<<endl;
	 
DataSet::index_iterator it3,it4;
	 ds.locate(it3,it4,"index1","songzj523904750");
	 for (DataSet::index_iterator it = it3 ; it != it4; ++it)
	 {
	 cout <<"locate OK" <<endl;
 cout << ((*it)[0]) <<"," << ((*it)[1]) <<","<< ((*it)[2]) <<","<< ((*it)[3]) <<endl;
	 }





cout << "build complex index ... " <<endl;
if (ds.buildIndex("index2",2,"field1",ASC,"field2",ASC))
  cout << "build complex index ok" <<endl;


DataSet::index_iterator it5,it6;
	 ds.locate(it5,it6,"index2","songzj523904750",90);
	 for (DataSet::index_iterator it = it5 ; it != it6; ++it)
	 {
	 cout <<"index 1 locate OK" <<endl;
 cout << ((*it)[0]) <<"," << ((*it)[1]) <<","<< ((*it)[2]) <<","<< ((*it)[3]) <<endl;
	 }




for (DataSet::index_iterator sit = ds.getIndexBegin(1); sit != ds.getIndexEnd(1); ++sit)
;//	  cout << (*sit)[0] <<"," <<(*sit)[1]<<"," <<(*sit)[2]<<"," <<(*sit)[3]<<endl;

//for (DataSet::iterator sit = ds.begin(); sit != ds.end(); ++sit)
//	  cout << (*sit)[0] <<"," <<(*sit)[1]<<"," <<(*sit)[2]<<"," <<(*sit)[3]<<endl;

 

  return 0;
}
