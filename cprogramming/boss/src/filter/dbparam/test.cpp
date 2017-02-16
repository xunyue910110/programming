#include "PM_Filter.h"
#include "Mdb_UserInfo_Crm.h"
#include "base/supdebug.h"

#include "CdrProperty.h"
#include "testCdr.h"

PM_Filter m_pmmanager;
const int DEALTIME = 100000;
vector<int> spcode_rpo;
  FilterCdr t_mcdr;
  PCDR t_pcdr;



int MDBlink()
{
	//MDB单元测试
  MdbInfo   m_mdbinfo;
  m_mdbinfo.m_dbType = MDB;
  m_mdbinfo.m_dbServer = "testmdb";
  m_mdbinfo.m_hostName = "";
  m_mdbinfo.m_port = 0;

  if(!m_pmmanager.setMdbInfo(m_mdbinfo))
  {
  	cout << "初始化内存数据库失败!" << endl;
  	return -1;
  }

  return 0;
}

int MDBtest()
{
  int i_result = 0;
  vector<UserServInfo> m_rpoVec;

  T_MSISDN r_msisdn = "13752666977";
FP_BEGIN(getUserServInfoAll)
  //i_result = m_pmmanager.getUserServInfoAll(r_msisdn,m_rpoVec);
FP_END(getUserServInfoAll)
  //for(int i=0;i<10000;i++)
  //{
  //FP_BEGIN(getUserServInfoAll)
  //  i_result = m_pmmanager.getUserServInfoAll(r_msisdn,m_rpoVec);
  //FP_END(getUserServInfoAll)
  //}
  if(i_result > 0)
  {
     cout << "记录条数: " <<i_result<<endl;
     for(vector<UserServInfo>::iterator r_Itr = m_rpoVec.begin();
               r_Itr != m_rpoVec.end();r_Itr++)
    {
      cout << "r_Itr:"<< *r_Itr <<" "<<__FILE__<<__LINE__<<endl;
    }
  }
  else if(i_result < 0)
  {
     cout << "内存数据库异常 " <<__FILE__<<__LINE__<<endl;
  }
  else
  {
     cout << "无记录 " <<__FILE__<<__LINE__<<endl;

  }

  return i_result;
}



int MDBImportPhone()
{
  int i_result = 0;
  UserServInfo m_rpo;

  T_MSISDN r_msisdn = "13752610181";
  T_DATE_TIME r_begintime = "20080101120910";
  T_DATE_TIME r_endtime = "20080101121010";
  T_ERROR_CODE error;

FP_BEGIN(getUserServInfoByMsisdn)
  //i_result = m_pmmanager.getUserServInfoByMsisdn("11",r_msisdn,r_begintime,r_endtime,m_rpo,error);
FP_END(getUserServInfoByMsisdn)

  if(i_result > 0)
  {
     cout << "记录条数: " <<i_result<<endl;
  }
  else if(i_result < 0)
  {
     cout << "内存数据库异常 " <<__FILE__<<__LINE__<<endl;
  }
  else
  {
     cout << "无记录 " <<__FILE__<<__LINE__<<endl;

  }

  return i_result;

  return 0;
}


int MDBImportImsi()
{
  int i_result = 0;
  UserServInfo m_rpo;

  T_IMSI r_imsi = "460002612500264";
  T_DATE_TIME r_begintime = "20080101120910";
  T_DATE_TIME r_endtime = "20080101121010";
  T_ERROR_CODE error;
  
FP_BEGIN(getUserServInfoByImsi)
  //i_result = m_pmmanager.getUserServInfoByImsi("11",r_imsi,r_begintime,r_endtime,m_rpo,error);
FP_END(getUserServInfoByImsi)

  if(i_result > 0)
  {
     cout << "记录条数: " <<i_result<<endl;
  }
  else if(i_result < 0)
  {
     cout << "内存数据库异常 " <<__FILE__<<__LINE__<<endl;
  }
  else
  {
     cout << "无记录 " <<__FILE__<<__LINE__<<endl;

  }

  return i_result;

  return 0;
}

int MDBTestSpservCode(const int &sp)
{
	
  int i_result = 0;
  UserServInfo m_rpo;

  SpProperty r_spproperty;
  T_SP_CODE r_sp_code;
   r_sp_code= sp;


	FP_BEGIN(test_mdb)
  i_result = m_pmmanager.testgetSpServCode(r_sp_code,r_spproperty);
  FP_END(test_mdb)


  if(i_result > 0)
  {
     //cout << "记录条数: " <<i_result<<endl;
  }
  else if(i_result < 0)
  {
     //cout << "内存数据库异常 " <<__FILE__<<__LINE__<<endl;
  }
  else
  {
     //cout << "无记录 " <<__FILE__<<__LINE__<<endl;

  }

  return i_result;

	return 0;
}

int DBTestSpservCode(const int &sp)
{
  SpProperty  r_SpProperty;
  r_SpProperty.clear();
  bool r_flag;

  T_SP_CODE r_sp_code;
   r_sp_code= sp;


  FP_BEGIN(test_db)
 	r_flag = (m_pmmanager.test_getSpServCode(r_sp_code,"*",r_SpProperty));
  FP_END(test_db)


  if(r_flag)
  {
  	//cout<<"success:   "<<endl;
  }
	return 0;
}

/*---------------------------------------
-----------------------------------------*/

int DBlink()
{
	m_pmmanager.setHostName("");
	m_pmmanager.setAppName("test");
	m_pmmanager.setChannelNo(0);

  m_pmmanager.setDbConnInfo("ucr_param","ucrparamcba","BILLING");

  if(!m_pmmanager.initialization())
  {
     #ifdef _DEBUG_
       cout<<"initial dbparam false!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return -1;
  }
  if(!m_pmmanager.getData())
  {
     #ifdef _DEBUG_
       cout<<"getData dbparam false!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return -1;
  }
  return 0;
}

int DBtest()
{
  SmProperty  r_SmProperty;
  r_SmProperty.clear();

  string r_stdnumber = "13602120003";
 	if(m_pmmanager.getSmServCode(r_stdnumber,r_SmProperty))
	{
		 cout<<"match_type:"<<r_SmProperty.match_type<<endl;
		 cout<<"serv_type:"<<r_SmProperty.serv_type<<endl;
		 cout<<"serv_code_type:"<<r_SmProperty.serv_code_type<<endl;
		 cout<<"home_type:"<<r_SmProperty.home_type<<endl;
		 cout<<"asp:"<<r_SmProperty.asp<<endl;
		 cout<<"call_type:"<<r_SmProperty.call_type<<endl;
		 cout<<"begin_time:"<<r_SmProperty.begin_time<<endl;
     cout<<"end_time:"<<r_SmProperty.end_time<<endl;
		 cout<<"sm_typestat:"<<r_SmProperty.sm_typestat<<endl;
	}

  r_stdnumber = "52503";
 	//if(m_pmmanager.isImsiI(r_stdnumber,"20080808200000"))
	{
		cout<<"success: "<<endl;
	}
	
	return 0;
}

int DBUserChannel()
{
	  string r_userid = "1104070100802680";
	  string r_eparchyCode = "0022";
	  int channel = -1;
	  if(!m_pmmanager.getChannelNoByUserId(r_userid,r_eparchyCode,channel))
	  {
		  cout<<"channel: "<<channel<<__FILE__<<__LINE__<<endl;
	  	return -1;
	  }
		  cout<<"channel: "<<channel<<__FILE__<<__LINE__<<endl;
		return 0;
}

int DB_CompCond()
{
	  int r_number = 50000001;
	  FilterCdr m_cdr;
	  PCDR m_pcdr = &m_cdr;
	  int error_code = 0;
	  
	  m_pcdr[103] = "50";  //50000003    ()
	  m_pcdr[104] = "200";  //50000004   (]
	  m_pcdr[106] = "0";  //50000006     [)
	  m_pcdr[107] = "50";  //50000007    []
	  m_pcdr[108] = "100";  //50000008    IN
	  m_pcdr[109] = "107"; //50000009    IN

	  if(!m_pmmanager.getActIDGroupByConID(r_number))
	  {
	  	cout<<"false:  "<<__FILE__<<__LINE__<<endl;
	  }
	  else
	  {
	  	cout<<"true:  "<<__FILE__<<__LINE__<<endl;
	  }

	  if(!m_pmmanager.isConditionValid(m_pcdr, r_number, error_code))
	  { 
	  	cout<<"error_code: "<<error_code<<"  "<<__FILE__<<__LINE__<<endl;
	  	return -1;
	  }
	  
	  cout<<"success:      "<<error_code<<"  "<<__FILE__<<__LINE__<<endl;

		return 0;
}


int main()
{
	spcode_rpo.clear();
	vector<int>::iterator itr;
	spcode_rpo.push_back(909143);
	spcode_rpo.push_back(910057);
	spcode_rpo.push_back(901079);
	spcode_rpo.push_back(902178);
	spcode_rpo.push_back(902079);

	
  if(DBlink()<0)
  {
  	 return -1;
  }
  
  DB_CompCond();


	//DBtest();
	//DBUserChannel();
/*
  cout<<"db test begin!"<<__FILE__<<__LINE__<<endl;
	for(itr=spcode_rpo.begin();itr!=spcode_rpo.end();itr++)
	{
		DBTestSpservCode(*itr);
	}
	
*/
/*
cout<<"\n\n"<<endl;

  if(MDBlink()<0)
  {
  	 return -1;
  }
	//MDBtest();
	MDBImportPhone();
	MDBImportImsi();
cout<<"mdb test begin!"<<__FILE__<<__LINE__<<endl;
	for(itr=spcode_rpo.begin();itr!=spcode_rpo.end();itr++)
	{
		MDBTestSpservCode(*itr);
	}

  m_pmmanager.closeMdbInfo();
*/

  
  
//  t_pcdr = &t_mcdr;
//  t_pcdr[100] = "123123";
//  cout<<"t_pcr[100]"<<t_pcdr[100]<<endl;


  return 0;
}
