#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include "mwci.h"
#include "udbi.h"
#include "StaffManager.h"
#include "AreaManager.h"
#include "DepartManager.h"

using namespace std;
using namespace UDBI;

JConnect * pcon;

int main(void)
{
	int s_getmode;
	cout<<"please input getmodecode"<<endl;
	cin>>s_getmode;
	jdb::jinit("EBOSS", "scott", "tiger");
	pcon = jdb::getConnect();
	cout<<"---------1------------"<<endl;
	CFmlBuf inBuf,outBuf;
	cout<<"---------2------------"<<endl;

	inBuf.SetInt("GETMODECODE",s_getmode);
/*STAFF*/
	inBuf.SetString("STAFFID","SUPERUSR");
	inBuf.SetString("AREACODE","CS01");
	inBuf.SetString("DEPARTID","A0001");
	inBuf.SetString("STARTSTAFFID","A0000000");
	inBuf.SetString("ENDSTAFFID","A0000002");
	inBuf.SetString("TRADEEPARCHYCODE","CS01");
	cout<<"-----/*STAFF*/----3------------"<<endl;
	GetStaffInfo(inBuf,outBuf);
	cout<<"-----/*STAFF*/----4------------"<<endl;
	
	if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK-----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<outBuf.GetInt("RESULTRECORDS")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetInt("RESULTCODE");
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<"---------5------------"<<endl;
  }
  cout<<"---------7------------"<<endl;
  
  
/*
	inBuf.SetInt(INRECORDS,1);
	
	inBuf.SetString(STAFFID,"LIUFTEST");
	inBuf.SetString(DEPARTID,"A0002");
	inBuf.SetString(STAFFNAME,"LIUFEI");
	inBuf.SetString(JOBINFO,"**********");
	inBuf.SetString(MANAGERINFO,"!!!!!!!!!!");
	inBuf.SetString(SEX,"0");
	inBuf.SetString(EMAIL,"sdfsfsfsafdaf");
	inBuf.SetString(USERPID,"11111111");
	inBuf.SetString(SERIALNUMBER,"11111111");
	inBuf.SetString(CUSTID,"11111111111");
	inBuf.SetString(DIMISSIONTAG,"0");
	inBuf.SetString(BIRTHDAY,"20041231");
	inBuf.SetInt(STAFFGROUPID,00000001);
	inBuf.SetString(CUSTHOBYY,"-------------");
	inBuf.SetString(REMARK,"0000000");
	inBuf.SetString(RSVALUE1,"-------");
	inBuf.SetString(RSVALUE2,"********");
	inBuf.SetString(UPDATETIME,"20050101");
	inBuf.SetString(TRADESTAFFID,"SYSOPER1");
	inBuf.SetString(TRADEDEPARTID,"A0001");
	inBuf.SetString(DEPARTNAME,"test");
	inBuf.SetString(B_SEX,"男");
	inBuf.SetString(B_DIMISSIONTAG,"有效");
	
	inBuf.SetInt(SAVEMODECODE,s_getmode);
	
	SaveStaffInfo(inBuf,outBuf);*/
	
/*AREA*/
/*	inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
	inBuf.SetInt("GETMODECODE",s_getmode);
	inBuf.SetString(VALIDFLAG,"0");
	inBuf.SetInt(AREALEVEL,30);
	inBuf.SetString(AREACODE,"CS01");
	
	GetAreaInfo(inBuf,outBuf);
	
	if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK-----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<outBuf.GetInt("RESULTRECORDS")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetInt("RESULTCODE");
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<"---------5------------"<<endl;
  }
  cout<<"---------7------------"<<endl;
  */
  
/*DEPART*/
/*	inBuf.ClearFmlValue();
  outBuf.ClearFmlValue();
	inBuf.SetInt("GETMODECODE",s_getmode);
	inBuf.SetString(VALIDFLAG,"0");
	inBuf.SetInt(DEPARTLEVEL,1);
	inBuf.SetString(AREACODE,"CS01");
	inBuf.SetString(DEPARTKINDCODE,"1");
	inBuf.SetString(DEPARTID,"A0001");
	inBuf.SetString(USERDEPARTCODE,"CHANGSHA001");
	inBuf.SetString(TRADEEPARCHYCODE,"CS01");
	inBuf.SetString(DEPARTCODE,"CS000001");
	inBuf.Print();
	GetDepartInfo(inBuf,outBuf);
	if(outBuf.GetInt("RESULTCODE") == 0)
  {
  	cout<<"---------OK-----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<outBuf.GetInt("RESULTRECORDS")<<endl;
  }
  else
  {
  	cout<<"---------ERROR----------"<<endl;
  	cout<<"RESULTINFO:"<<endl;
  	cout<<outBuf.GetInt("RESULTCODE");
  	cout<<outBuf.GetString("RESULTERRINFO")<<endl;
  	cout<<"---------5------------"<<endl;
  }*/
  cout<<"---------7------------"<<endl;
  return 0;
}
