#include <string>
#include <iostream>
#include "mwlcu/mwlcu.h"
#include "mwci/mwci.h"
#include "udbi/udbi.h"
#include <strstream>
using namespace std;
        
        
        
        
int testInt64(JConnect* pcon){
	
	 JTable *rst = pcon->CreateJTable();
 
 
   try {
   	  
	//	string sql = "select v.ID, v.NAME from (";
	//	sql += "select distinct(t2.name) as NAME, max(t2.id) as ID ";
	//	sql += " from TD_BPM_DEFINITION t2  group by t2.name) v ,";
	//	sql += " TD_BPM_EXT_ATTRIBUTE ex ";
	//	sql += " where ex.name = 'category' and ex.value='lcu' and ex.element= v.ID ";
	//	sql += " order by 2";
        string sql = "select  11 as ID, 'abc' as NAME from dual    ";
		rst->AddSql(sql);
		rst->SetPrefetchRecs(1000);
		rst->ExecSelect();

        int64_t id;
        string name;
		while(rst->Fetch()){
			id = rst->GetInt64("ID");
			name = rst->GetString("NAME");
			cout << " id=" << id << " name=" <<  name << endl;
        }
        delete rst;
     } catch(...){
           delete rst;
           throw;
     }
}
int teststr(){
    strstream	ss;
    ss << "1234567890123456789";
    int64_t id;
    ss >> id;
    cout << id  << endl;      
}

int teststring(){
    stringstream	ss;
    ss << "1234567890123456789";
    int64_t id;
    ss >> id;
    cout << "stringstream:"<< id  << endl;
}

int check(const char* data){
	int64_t id,val,val2; 
	stringstream	ss;
    ss << data;     
    ss >> id;
    cout << "id:" << id  << endl;
    
    if (data) {
		stringstream s;
		if (!(s << data) && s >> val)
			cout <<"结果转int64_t操作失败！"<<endl;
		cout <<"val:" <<  val <<endl;	
	} else {
		cout <<"结果集数据为空！"<<endl;		
	}
	
	if (data) {
		stringstream cc;
		if(!(cc << data) ){			
			cout <<"结果转int64_t操作失败  !(cc << data)"<<endl;
		}
		if(!(cc >> val2))
			cout <<"结果转int64_t操作失败 !cc >> val2"<<endl;
		cout <<"val2:" <<  val2 <<endl;	
	} 
}


int main()
{
    cout << "1111"  << endl;
    try{

        MWLCU&  mwlcu = MWLCU::getInstance();
      //  if(mwlcu.isInTransXA())
      //      cout << "1111isInTransXA" << endl;
      //  else
      //      cout << "1111notInTransXA" << endl;
      //
      //  mwlcu.transBegin(3);
      //
      //  if(mwlcu.isInTransXA())
      //      cout << "2222isInTransXA" << endl;
      //  else
      //      cout << "2222notInTransXA" << endl;
      //      
		jdb::jinit("boss15", "uop_yy01_kf", "uopyy01kf123");    
	    //jdb::jinit("boss15", "ucr_tyjk_kf", "ucrtyjkkftjabc123"); 
	    	            
		// ucr_tyjk_kf/ucrtyjkkftjabc123
        JConnect* pcon = jdb::getConnect();
        testInt64(pcon);
      // JTable * rst = pcon->CreateJTable();
      // try{
      //     rst->AddSql("insert into TD_S_ERRORCODE values('12345','info1234') ");
      //
      //     rst->ExecInsert();
      //     delete rst;
      // } catch(EUdbiException & e ){
      //     cout << "[LCU]EUdbiException异常:" << e <<endl;
      //     delete rst;
      // }
       
      // cout << "please input commit" << endl;
      // char i[10];
      // cin >> i ;
      // pcon->TransCommit();
      //  if(tpcommit(0) == -1)
      //      cout << "commit fail" << endl;
      //  else
      //      cout << "commit success" << endl;
            
        //mwlcu.transCommit();
    }catch(Exception &e){
        cout << "Exception:" << e.what() << endl;
    }
    
    //cout << "please input exit"  << endl;
    //char j[10];
    //cin >> j ;
	cout << "---teststr();-------"  << endl;
	teststr(); 
	cout  << endl << "---teststring(); -------" << endl;
	teststring(); 
	cout  << endl<< "---check('111');-------"  << endl;
	check("111");  
	cout  << endl<< "---check('1234567890123456789');-------"  << endl;
	check("1234567890123456789");	
	cout << endl<< "----------"  << endl;
	cout << "exit" << endl;

    return 0;
}
