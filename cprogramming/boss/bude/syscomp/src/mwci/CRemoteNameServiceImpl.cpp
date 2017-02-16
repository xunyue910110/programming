#include "mwci/CRemoteNameServiceImpl.h"
#include "mwci/mwci.h"
#include "mwci/CFmlBuf.h"

//##ModelId=419184A10308
bool CRemoteNameServiceImpl::BatChgNaming(const vector<CNaming>& vtheNamings,IMWCaller* theCaller)
{
	//IMWCaller *theCaller;

	char theTimeBuf[14+1];
	CNaming theLoNaming;
	string tempbuf;
	int asyid;
	try
	{
		CFmlBuf theBuf(SIZE_32K),OutBuf(SIZE_4K);

		//构造输入缓冲区
		const int theAllRes=vtheNamings.size();
		for (int i=0;i<theAllRes;i++)
		{
//28		cout<<__LINE__<<'\t'<<"CONTEXTID"<<vtheNamings[i].m_cContext.m_nContextID<<endl;
			theBuf.SetInt("X_CONTEXT_ID",vtheNamings[i].m_cContext.m_nContextID,i);

			theBuf.SetString("X_LOGICAL_NAME",vtheNamings[i].m_strLogicName,i);
			tmTOchar(vtheNamings[i].m_tmReDeployTime , theTimeBuf);
			theBuf.SetString("REDEPLOY_TIME",theTimeBuf,i);
			theBuf.SetString("X_MACHINE_IP",vtheNamings[i].m_strMachineIP,i);
			theBuf.SetString("X_MACHINE_PORT",vtheNamings[i].m_strMachinePort,i);
			theBuf.SetString("X_MACHINE_NAME",vtheNamings[i].m_strMachineName,i);
			theBuf.SetString("X_SERVICE_NAME",vtheNamings[i].m_strPhysicalName,i);
			theBuf.SetString("X_SERVICE_TYPE_CODE",vtheNamings[i].m_cSvcTypeCode,i);
		}

		//创建连接器
		//theCaller=CCallerFactory::CreateCaller(0);
		//theCaller->Connect(m_ConnectString,"");

		//调用服务
		asyid=theCaller->AsyCall("BATCH_UPDATE",theBuf,0L,false);
		theCaller->AsyGetReply(OutBuf,asyid,0L);

//		delete theCaller;
	}catch( MWException &ex)
	{
		cerr<<__FILE__<<':'<<__LINE__<<' '<<ex.getCode()<<':'<<ex.getMessage()<<endl;
	//	delete theCaller;
		return false;
	}

	return true;
}

//##ModelId=4194AAC803C2
bool CRemoteNameServiceImpl::ListNaming(vector<CNaming>& vtheList,IMWCaller* theCaller)
{
	//IMWCaller *theCaller;

	int asyid;
	CNaming theLoNaming;

	try
	{
		CFmlBuf theBuf(SIZE_4K),OutBuf(SIZE_64K);

		//创建连接器
		//theCaller=CCallerFactory::CreateCaller(0);
		//theCaller->Connect(m_ConnectString,"");

		//调用服务
		asyid=theCaller->AsyCall("LIST",theBuf,0L,false);
		theCaller->AsyGetReply(OutBuf,asyid,0L);

//		delete theCaller;

		//填写结果
		const int iAllRes=OutBuf.GetCount("X_LOGICAL_NAME");

		if (iAllRes>0)
			vtheList.clear();

		for (int i=0;i<iAllRes;i++)
		{
			theLoNaming.m_cContext.m_nContextID=atoi(OutBuf.GetStr("X_CONTEXT_ID",i));
			theLoNaming.m_cSvcTypeCode=OutBuf.GetStr("X_SERVICE_TYPE_CODE",i);
			theLoNaming.m_strLogicName=OutBuf.GetStr("X_LOGICAL_NAME",i);
			theLoNaming.m_strMachineIP=OutBuf.GetStr("X_MACHINE_IP",i);
			theLoNaming.m_strMachineName=OutBuf.GetStr("X_MACHINE_NAME",i);
			theLoNaming.m_strMachinePort=OutBuf.GetStr("X_MACHINE_PORT",i);
			theLoNaming.m_strPhysicalName=OutBuf.GetStr("X_SERVICE_NAME",i);
			string theStrTime=OutBuf.GetStr("REDEPLOY_TIME",i);charTOtm(theStrTime.c_str(),theLoNaming.m_tmReDeployTime);
			vtheList.push_back(theLoNaming);
		}


	}catch( MWException &ex)
	{
		cerr<<__FILE__<<':'<<__LINE__<<' '<<ex.getCode()<<':'<<ex.getMessage()<<endl;
//		delete theCaller;
		return false;
	}

	return true;

}

//##ModelId=4194AAC803CC
bool CRemoteNameServiceImpl::FindUptNaming(long  lContextid, const STD tm& tmUptTime, vector<CNaming>& vtheNaming,IMWCaller* theCaller)
{
	//IMWCaller *theCaller;
	char theTimeBuf[14+1];
	CNaming theLoNaming;
	int i;
	int asyid;
	try
	{
		CFmlBuf theBuf=CFmlBuf(SIZE_4K),OutBuf(SIZE_4K);

		//创建连接器
		//theCaller=CCallerFactory::CreateCaller(0);
		//theCaller->Connect(m_ConnectString,"");


		theBuf.AppFldValue("X_CONTEXT_ID",&lContextid);

		if (!tmTOchar(tmUptTime, theTimeBuf))
			return false;

		theBuf.AppFldValue("REDEPLOY_TIME",theTimeBuf);

		//调用服务
		asyid=theCaller->AsyCall("GET_UPDATED",theBuf,0L,false);
		theCaller->AsyGetReply(OutBuf,asyid,0L);

	//	delete theCaller;

		//填写结果
		const int iAllRes=OutBuf.GetCount("X_LOGICAL_NAME");

		if (iAllRes>0)
			vtheNaming.clear();

		for (i=0;i<iAllRes;i++)
		{
			theLoNaming.m_cSvcTypeCode=OutBuf.GetStr("X_SERVICE_TYPE_CODE",i);
			theLoNaming.m_strLogicName=OutBuf.GetStr("X_LOGICAL_NAME",i);
			theLoNaming.m_strMachineIP=OutBuf.GetStr("X_MACHINE_IP",i);
			theLoNaming.m_strMachineName=OutBuf.GetStr("X_MACHINE_NAME",i);
			theLoNaming.m_strMachinePort=OutBuf.GetStr("X_MACHINE_PORT",i);
			theLoNaming.m_strPhysicalName=OutBuf.GetStr("X_SERVICE_NAME",i);
			string theStrTime=OutBuf.GetStr("REDEPLOY_TIME",i);
			charTOtm(theStrTime.c_str(),theLoNaming.m_tmReDeployTime);
			vtheNaming.push_back(theLoNaming);
		}


	}catch( MWException &ex)
	{
		cerr<<__FILE__<<':'<<__LINE__<<' '<<ex.getCode()<<':'<<ex.getMessage()<<endl;
	//	delete theCaller;
		return false;
	}

	return true;

}

//##ModelId=41BD39D3035C
bool CRemoteNameServiceImpl::SetNamingSrvInfo(string SrvInfo)
{
	m_ConnectString=SrvInfo;
	return true;
}

//##ModelId=4194ADA2038F
bool CRemoteNameServiceImpl::LookUpNaming(long  lContextid, string strLogicalName, CNaming& theNaming,IMWCaller* theCaller,int iIdx)
{
	//IMWCaller *theCaller;
	int asyid;
	try
	{
		CFmlBuf theBuf=CFmlBuf(SIZE_4K),OutBuf(SIZE_4K);

		//创建连接器
		//theCaller=CCallerFactory::CreateCaller(0);
		//theCaller->Connect(m_ConnectString,"");

		theBuf.AppFldValue("X_CONTEXT_ID",&lContextid);
		theBuf.AppFldValue("X_LOGICAL_NAME",(char *)strLogicalName.c_str());
//28	cout<<"\n-----Remote Naming InBuf  -----"<<endl;
//28	theBuf.Print();
//28	cout<<"-------------------------------"<<endl;
		//调用服务
		asyid=theCaller->AsyCall("LOOK_UP",theBuf,0L,false);
		theCaller->AsyGetReply(OutBuf,asyid,0L);
//28	cout<<"\n-----Remote Naming OutBuf -----"<<endl;
//28	OutBuf.Print();
//28	cout<<"-------------------------------"<<endl;
		theNaming.m_cSvcTypeCode=OutBuf.GetStr("X_SERVICE_TYPE_CODE",iIdx);
		theNaming.m_strLogicName=strLogicalName;
		theNaming.m_strMachineIP=OutBuf.GetStr("X_MACHINE_IP",iIdx);
		theNaming.m_strMachineName=OutBuf.GetStr("X_MACHINE_NAME",iIdx);
		theNaming.m_strMachinePort=OutBuf.GetStr("X_MACHINE_PORT",iIdx);
		theNaming.m_strPhysicalName=OutBuf.GetStr("X_SERVICE_NAME",iIdx);

		string theStrTime=OutBuf.GetStr("REDEPLOY_TIME",iIdx);
		charTOtm(theStrTime.c_str(),theNaming.m_tmReDeployTime);

		//delete theCaller;
	}catch( MWException &ex)
	{
		cerr<<__FILE__<<':'<<__LINE__<<' '<<ex.getCode()<<':'<<ex.getMessage()<<endl;
	//	delete theCaller;
		return false;
	}

	return true;
}

//##ModelId=41BD5A950228
bool CRemoteNameServiceImpl::charTOtm(const char *theString, STD tm &theTime)
{
	 //struct tm {
        //int tm_sec;     /* seconds after the minute - [0,59] */
        //int tm_min;     /* minutes after the hour - [0,59] */
        //int tm_hour;    /* hours since midnight - [0,23] */
        //int tm_mday;    /* day of the month - [1,31] */
        //int tm_mon;     /* months since January - [0,11] */
        //int tm_year;    /* years since 1900 */
        //int tm_wday;    /* days since Sunday - [0,6] */
        //int tm_yday;    /* days since January 1 - [0,365] */
        //int tm_isdst;   /* daylight savings time flag */
        //};
	char myStrTime[14+1];
	char theBuf[4+1];
	strncpy(myStrTime,theString,14);

	//get the year
	strncpy(theBuf,myStrTime,4);
	theBuf[5]=0;
	theTime.tm_year= atoi(theBuf)-1900;

	//get the month
	strncpy(theBuf,myStrTime+4,2);
	theBuf[2]=0;
	theTime.tm_mon= atoi(theBuf)-1;

	//get the month-day
	strncpy(theBuf,myStrTime+6,4);
	theBuf[2]=0;
	theTime.tm_mday= atoi(theBuf);

	//get the month-day
	strncpy(theBuf,myStrTime+8,4);
	theBuf[2]=0;
	theTime.tm_hour= atoi(theBuf);

	//get the month-day
	strncpy(theBuf,myStrTime+10,4);
	theBuf[2]=0;
	theTime.tm_min= atoi(theBuf);

	//get the month-day
	strncpy(theBuf,myStrTime+12,4);
	theBuf[2]=0;
	theTime.tm_sec= atoi(theBuf);

	return true;

}

//##ModelId=41BD91510321
bool CRemoteNameServiceImpl::tmTOchar(const STD tm &theTime, char *theString)
{
	//check the value of time
	if (theTime.tm_year<0 || theTime.tm_year >200 ||
	     theTime.tm_mon<0 || theTime.tm_mon>11 ||
	     theTime.tm_mday<0 || theTime.tm_mday>31 ||
	     theTime.tm_hour<0 || theTime.tm_hour>24 ||
	     theTime.tm_min<0 || theTime.tm_min>60 ||
	     theTime.tm_sec<0 || theTime.tm_sec>60)
	     return false;


	sprintf(theString,"%4d%02d%02d%02d%02d%02d",
		   theTime.tm_year+1900,theTime.tm_mon+1,theTime.tm_mday,
		   theTime.tm_hour,theTime.tm_min,theTime.tm_sec);
	return true;
}

//##ModelId=41BED44200ED
bool CRemoteNameServiceImpl::ListNaming(long lContextID, vector<CNaming>& vtheList,IMWCaller *theCaller)
{

	CNaming theLoNaming;
	int asyid;
	try
	{
		CFmlBuf theBuf=CFmlBuf(SIZE_4K),OutBuf(SIZE_64K);

		//创建连接器
		//theCaller=CCallerFactory::CreateCaller(0);
		//theCaller->Connect(m_ConnectString,"");

		//指定contextid
		theBuf.AppFldValue("X_CONTEXT_ID",&lContextID);

		//调用服务
		//theBuf.Print();
		//cout<<"\n"<<
		asyid=theCaller->AsyCall("GET_UPDATED",theBuf,0L,false);

		theCaller->AsyGetReply(OutBuf,asyid,0L);
		OutBuf.Print();
		//delete theCaller;
		theCaller=NULL;

		//填写结果
		const int iAllRes=OutBuf.GetCount("X_LOGICAL_NAME");

		if (iAllRes>0)
			vtheList.clear();

		for (int i=0;i<iAllRes;i++)
		{
			theLoNaming.m_cContext.m_nContextID=atoi(OutBuf.GetStr("X_CONTEXT_ID",i));
			theLoNaming.m_cSvcTypeCode=OutBuf.GetStr("X_SERVICE_TYPE_CODE",i);
			theLoNaming.m_strLogicName=OutBuf.GetStr("X_LOGICAL_NAME",i);
			theLoNaming.m_strMachineIP=OutBuf.GetStr("X_MACHINE_IP",i);
			theLoNaming.m_strMachineName=OutBuf.GetStr("X_MACHINE_NAME",i);
			theLoNaming.m_strMachinePort=OutBuf.GetStr("X_MACHINE_PORT",i);
			theLoNaming.m_strPhysicalName=OutBuf.GetStr("X_SERVICE_NAME",i);
			string theStrTime=OutBuf.GetStr("REDEPLOY_TIME",i);
			charTOtm(theStrTime.c_str(),theLoNaming.m_tmReDeployTime);
			vtheList.push_back(theLoNaming);
		}


	}catch( MWException &ex)
	{
		cerr<<__FILE__<<':'<<__LINE__<<' '<<ex.getCode()<<':'<<ex.getMessage()<<endl;
	//	if(theCaller) delete theCaller;
		return false;
	}

	return true;

}

