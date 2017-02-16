#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include "mwci.h"
#include "udbi.h"

#include "ValidStaff.h"
#include "StaffPasswdCHG.h"
#include "SysLogManager.h"
#include "GetSeqID.h"
#include "StaffRight.h"

using namespace std;
using namespace UDBI;

//ValidStaff * VS;
//StaffPasswdCHG * SPC;
JConnect * pcon;

int main(void)
{
  bool ValidStaffResurt = false;
  string str_staffid;
	string str_passwd;
	string str_IP;
	
	short s_getmode;
	cout<<"please input getmodecode"<<endl;
	cin>>s_getmode;
  jdb::init();
	pcon = jdb::Connect("EBOSS", "scott", "tiger");
  
	CFmlBuf inBuf,outBuf;


/*	inBuf.SetString("TRADESTAFFID","SYSOPER1");
	inBuf.SetInt("GETMODE",s_getmode);
	inBuf.SetString("SUBSYSCODE","BRN");
	
	cout<<"---------begin---------"<<endl;
	cout<<inBuf.GetInt("GETMODE")  <<endl;
	GetStaffRight(inBuf,outBuf);
  
  if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK----GetStaffRight-------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<outBuf.GetInt("RESULTRECORDS")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----GetStaffRight------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<outBuf.GetInt("RESULTRECORDS")<<endl;
  }*/

  inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
  inBuf.SetString("STAFFID","SYSOPER1");
  inBuf.SetString("PASSWORD","q8GYiw");
  inBuf.SetString("D1","192.168.8.110");
  inBuf.SetString("TRADEEPARCHYCODE","CSHA");

  cout<<"---------begin----ValidStaff-----"<<endl;

  CheckLogStaff(inBuf,outBuf);
  
  if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK----ValidStaff-------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----ValidStaff------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
  inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
  
  inBuf.SetString("STAFFID","A0000001");
  inBuf.SetString("PASSWORD","222222");
  
  cout<<"---------begin----StaffpasswdCHG-----"<<endl;
  
  pcon->TransBegin();
  StaffPasswdChange(inBuf,outBuf);
	pcon->TransCommit();
  
  if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK----StaffpasswdCHG-------"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----StaffpasswdCHG------"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }  


  inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
  int optypecode = 0;
  inBuf.SetInt("OPTYPECODE", 0 );
  inBuf.SetString("TRADESTAFFID","A0000001");
  inBuf.SetString("SUBSYSCODE","CS");
  inBuf.SetString("INIP","192.168.8.110");
  inBuf.SetString("REMARK","LIUFEITEST");
  
  cout<<"---------begin----SysLogManager-----"<<endl;
  
  pcon->TransBegin();
  SysOperLog(inBuf,outBuf);
	pcon->TransCommit();
  if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK----SysLogManager-------"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----SysLogManager------"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }  
  
  
  inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
  optypecode = 1;
  inBuf.SetInt("OPTYPECODE",1);
  inBuf.SetString("TRADESTAFFID","A0000001");
  inBuf.SetString("SUBSYSCODE","CS ");
  inBuf.SetString("INIP","192.168.8.110");
  inBuf.SetString("REMARK","LIUFEITEST");
  
  cout<<"---------begin----SysLogManager--q---"<<endl;
  
  pcon->TransBegin();
  SysOperLog(inBuf,outBuf);
	pcon->TransCommit();
  if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK----SysLogManager---q----"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----SysLogManager--q----"<<endl;
  	  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  }
//  delete(VS);
  return 0;
}
