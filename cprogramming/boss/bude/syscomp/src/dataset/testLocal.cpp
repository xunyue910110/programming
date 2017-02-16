#include <iostream>
#include <stdlib.h>
#include "DataSet.h"
//#include <sys/time.h>
using namespace std;
using namespace dataset;
int main(int argc, char *argv[])
{


DataSet ds;

  ds.addField(Field("field1",FTINT,20));
  ds.addField(Field("field2",FTINT,16));
  ds.addField(Field("field3",FTSTRING,10));
  ds.addField(Field("field4",FTDOUBLE,12));
         
  int COUNT= 1000000;
  ds.reserve(COUNT);
        

             

	 vector< pair< string ,dataset::SORT_TYPE> > groupbyname;
	groupbyname.push_back( pair<string,dataset::SORT_TYPE>("field1", ASC));
	groupbyname.push_back( pair<string,dataset::SORT_TYPE>("field2", ASC));
		

	
	  
 cout << "Inserting data ..." <<endl;
  DataSet::iterator it; 
  for (int i = 0; i < COUNT; ++i)
  { 
      it = ds.pushBack();
      if (it == ds.end()) break;

      (*it)[0] = (int)random();
      (*it)[1] =i;  
      char buffer[10]="\0";
     sprintf(buffer, "%d", random());
       (*it)[2] =  buffer;
        (*it)[3] = (double)(i/12345);
      //  ds.refreshIndexs(it);
  }
cout <<"DataSet size=" << ds.size() <<endl;
	 
if(ds.buildIndex("index",groupbyname))
	  cout << "buildIndex ok " <<endl;	 
	 
	  
	  
	
	//for (DataSet::index_iterator sit = ds.getIndexBegin(); sit != ds.getIndexEnd(); ++sit)
	//          cout << (*sit)[0] <<"," <<(*sit)[1]<<"," <<(*sit)[2]<<"," <<(*sit)[3]<<endl;
	
	DataSet::index_iterator sit = (ds.getIndexBegin() +1);
	vector<dataset::Data> datas;
	datas.push_back((*sit)[0]);	
	datas.push_back((*sit)[1]);

	DataSet::index_iterator it3 = ds.getIndexBegin();
	DataSet::index_iterator it4 = ds.getIndexEnd();

	if (ds.locate(it3,it4,"index",datas))
	{
		cout <<"locate ok" <<endl;
	}
     for (DataSet::index_iterator it = it3 ; it != it4; ++it)
      {
    	  cout << (*it)[0] <<"," <<(*it)[1]<<"," <<(*it)[2]<<"," <<(*it)[3]<<endl;

      }



  return 0;
}