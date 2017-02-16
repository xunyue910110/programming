#include <unistd.h>
#include "mwci/LocalParseN.h"
#include "mwci/CContext.h"
#include "mwci/CNaming.h"
#include "mwci/CRemoteNameServiceImpl.h"
#include "mwci/CMutex.h"
CMutex gMutex;


//##ModelId=41B50F7F036C
int LocalParseN::GetLocalCI(CContext& theLocCI)
{
	if (!m_bReady)
		return LNS_NOT_READY;

	theLocCI.m_bNeedLocSvc=m_NSInfo.NeedLocSvc;
	theLocCI.m_nContextID=m_NSInfo.ContextID;
	theLocCI.m_strCityCode=m_NSInfo.CityCode;
	theLocCI.m_strEparchyCode=m_NSInfo.EparchyCode;
	theLocCI.m_strOriginCode=m_NSInfo.OriginCode;
	theLocCI.m_tmNamingUptTime=m_NSInfo.NamingUptTime;

	return LNS_OK;
}

//##ModelId=41B50F7F0394
int LocalParseN::GetRemoteNS(CNaming& theRemoteNS)
{
	if (!m_bReady)
		return LNS_NOT_READY;

	theRemoteNS.m_strMachineIP=m_NSInfo.IP;
	theRemoteNS.m_strMachineName=m_NSInfo.MachineName;
	theRemoteNS.m_strMachinePort=m_NSInfo.PORT;
	m_bIsUpdated=false;

	return LNS_OK;
}

//##ModelId=41B6A61D00D8
LocalParseN::LocalParseN()
{
	this->m_bReady=true;
	this->m_bIsUpdated=false;
	memset(&m_NSInfo,0x00,sizeof(CLNSInfo));
}

//##ModelId=41B50F7F03B2
LocalParseN::LocalParseN(const char *filename)
{

	CLNSNaming theBuf;
	string theXMLfile;
	m_sFileName=filename;

	///线程安全加锁
	gMutex.wait();

	//read file to memory
	this->m_bIsUpdated=false;
	this->m_fp=fopen(filename,"rb");

	m_listNaming.clear();

	m_bReady=true;

	if (m_fp==NULL)
	{
			this->m_bReady=false;gMutex.release();///线程安全加锁
			return;
	}

	//get file size
	fseek(m_fp,0,SEEK_END);
	const long fSize=ftell(m_fp);
	fseek(m_fp,0,SEEK_SET);

	//prepare buffer
	char *charbuf=NULL;
	charbuf=new char[fSize+1];

	if(charbuf==NULL)
	{
			this->m_bReady=false;
			fclose(m_fp);
			m_fp=NULL;gMutex.release();///线程安全加锁
			return;
	}

	//read file
	if(fread(charbuf,1,fSize,m_fp)!=fSize)
	{
		this->m_bReady=false;
		fclose(m_fp);
		m_fp=NULL;
		delete [] charbuf;gMutex.release();///线程安全加锁
		return;
	}

	fclose(m_fp);
	m_fp=NULL;

	//convert to string
	charbuf[fSize]=0;
	theXMLfile=charbuf;
	delete [] charbuf;

	//load CLNSInfo
	string theLocalContextInfo;
	if(!getValue(theXMLfile,"LOCALCONTEXTINFO",theLocalContextInfo))
	{
		this->m_bReady=false;gMutex.release();///线程安全加锁
		return;
	}
	if(!getLocalContextInfo(theLocalContextInfo, m_NSInfo))
	{
		this->m_bReady=false;gMutex.release();///线程安全加锁
		return;
	}

	string theRemoteNamingSrv;
	if(!getValue(theXMLfile,"REMOTENAMINGSRV",theRemoteNamingSrv))
	{
		this->m_bReady=false;gMutex.release();///线程安全加锁
		return;
	}
	if(!getRemoteNamingSrv(theRemoteNamingSrv, m_NSInfo))
	{
		this->m_bReady=false;gMutex.release();///线程安全加锁
		return;
	}

	//load Naming into memory
	int theOffset=0;
	string theNaming;
	while (	getNextValue(theXMLfile,"NAMING",theNaming,theOffset))
	{
		if(getNaming(theNaming, theBuf))
			m_listNaming.push_back(theBuf);
		else
		{
			this->m_bReady=false;
		}
	}

	///线程安全加锁
	gMutex.release();
}


//##ModelId=41B50F7F03DA
bool LocalParseN::IsReady() const
{
    return m_bReady;
}


//##ModelId=41B50F800006
string LocalParseN::GetFileName() const
{
    return m_sFileName;
}


//##ModelId=41B50F80001A
LocalParseN::~LocalParseN()
{

	if (m_bIsUpdated)
	{
		SaveToFile();
		//cout<<"Updated!"<<endl;
	}

	if(m_fp)
		fclose(m_fp);
}

//##ModelId=41B50F80002E
int LocalParseN::SaveToFile()
{
	if(m_sFileName.empty())
		return LNS_NO_FILE_NAME;

	///线程安全加锁
	gMutex.wait();

	//delete old .bak file
	unlink((m_sFileName+".bak").c_str());

	//backup file to .bak
	rename(m_sFileName.c_str(),(m_sFileName+".bak").c_str());

	m_fp=fopen(m_sFileName.c_str(),"wb");

	if (!m_fp)
	{
		gMutex.release();
		return LNS_OPEN_FAIL; // open fail
	}
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

	//write head
	fprintf(m_fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(m_fp,"<LOCALNAMESERVICE>\n");

	//write local context info
	fprintf(m_fp,"\t<LOCALCONTEXTINFO>\n");
	fprintf(m_fp,"\t\t<CONTEXTID>%d</CONTEXTID>\n",m_NSInfo.ContextID);
	fprintf(m_fp,"\t\t<EPARCHYCODE>%s</EPARCHYCODE>\n",m_NSInfo.EparchyCode);
	fprintf(m_fp,"\t\t<CITYCODE>%s</CITYCODE>\n",m_NSInfo.CityCode);
	fprintf(m_fp,"\t\t<ORIGINCODE>%s</ORIGINCODE>\n",m_NSInfo.OriginCode);
	fprintf(m_fp,"\t\t<NAMINGUPTTIME>%4d%02d%02d%02d%02d%02d</NAMINGUPTTIME>\n",
			m_NSInfo.NamingUptTime.tm_year+1900,m_NSInfo.NamingUptTime.tm_mon+1,m_NSInfo.NamingUptTime.tm_mday,
			m_NSInfo.NamingUptTime.tm_hour,m_NSInfo.NamingUptTime.tm_min,m_NSInfo.NamingUptTime.tm_sec);
	//fprintf(m_fp,"\t\t<NEEDLOCALSERVICE>%s</NEEDLOCALSERVICE>\n",m_NSInfo.NeedLocSvc?"True":"False");
	fprintf(m_fp,"\t</LOCALCONTEXTINFO>\n");

	//write remote Naming Service info
	fprintf(m_fp,"\t<REMOTENAMINGSRV>\n");
	fprintf(m_fp,"\t\t<IP>%s</IP>\n",m_NSInfo.IP);
	fprintf(m_fp,"\t\t<PORT>%s</PORT>\n",m_NSInfo.PORT);
	fprintf(m_fp,"\t\t<MACHINENAME>%s</MACHINENAME>\n",m_NSInfo.MachineName);
	fprintf(m_fp,"\t</REMOTENAMINGSRV>\n");

	//write Naming info
	theEnd=m_listNaming.end();

	for (itr=m_listNaming.begin();itr!=theEnd;itr++)
	{
		fprintf(m_fp,"\t<NAMING>\n");
		fprintf(m_fp,"\t\t<LOGICALNAME>%s</LOGICALNAME>\n",itr->LogicName);
		fprintf(m_fp,"\t\t<MACHINENAME>%s</MACHINENAME>\n",itr->MachineName);
		fprintf(m_fp,"\t\t<IP>%s</IP>\n",itr->IP);
		fprintf(m_fp,"\t\t<PORT>%s</PORT>\n",itr->PORT);
		fprintf(m_fp,"\t\t<PHYSICALNAME>%s</PHYSICALNAME>\n",itr->PhyName);
		fprintf(m_fp,"\t\t<TYPECODE>%c</TYPECODE>\n",itr->TypeCode);
		//printf("TYPECODE:%s:%d %d %c\n",__FILE__,__LINE__,itr->TypeCode,itr->TypeCode);
		fprintf(m_fp,"\t</NAMING>\n");
	}

	//file tail
	fprintf(m_fp,"</LOCALNAMESERVICE>\n");
	fflush(m_fp);
	fclose(m_fp);
	m_bIsUpdated=false;

	///线程安全解锁
	gMutex.release();

	return LNS_OK;
}

//##ModelId=41B50F800042
int LocalParseN::SetNameSvc(const string& LogicName, CNaming& theNaming, int iIndex)
{
	if (!m_bReady)
		return LNS_NOT_READY;

	int count=0; // count for the iIndex
	bool bIsFound=false; //if LogicName is found in memory

	///线程安全加锁
	gMutex.wait();

	theEnd=m_listNaming.end();

	for (itr=m_listNaming.begin();itr!=theEnd;itr++)
		{
			if ((*itr)==LogicName)
				{
					bIsFound=true;
					if (count++==iIndex)
						{

							if (fnChkNaming(theNaming)== -1 )
								{
									gMutex.release();	///线程安全加锁
									return LNS_BAD_NAME; // bad Naming
								}

							strcpy(itr->TypeCode,theNaming.m_cSvcTypeCode.c_str());
							strcpy(itr->IP,theNaming.m_strMachineIP.c_str());
							strcpy(itr->MachineName,theNaming.m_strMachineName.c_str());
							strcpy(itr->PORT,theNaming.m_strMachinePort.c_str());
							strcpy(itr->PhyName,theNaming.m_strPhysicalName.c_str());
							m_bIsUpdated=true;
							break;
						}
				}
		}

	///线程安全加锁
	gMutex.release();

	//no data found
	if (itr==theEnd)
		if(bIsFound)
			return LNS_NO_INDEX_FOUND; //no index found
		else
			return LNS_NO_DATA_FOUND; // no logicName found

	return LNS_OK;

}

//##ModelId=41B50F80007F
int LocalParseN::GetNameSvc(const string& LogicName, CNaming& theNaming, int iIndex)
{
	if (!m_bReady)
		return LNS_NOT_READY;

	int count=0; // count for the iIndex
	//cout<<"Get Naming:"<<LogicName<<endl;
	///线程安全加锁
	gMutex.wait();

	theEnd=m_listNaming.end();

	for (itr=m_listNaming.begin();itr!=theEnd;itr++)
		{
			if ((*itr)==LogicName)
				{
					if (count++==iIndex)
						{
							theNaming.m_cSvcTypeCode=itr->TypeCode;
							theNaming.m_strLogicName=LogicName;
							theNaming.m_strMachineIP=itr->IP;
							theNaming.m_strMachineName=itr->MachineName;
							theNaming.m_strMachinePort=itr->PORT;
							theNaming.m_strPhysicalName=itr->PhyName;
							break;
						}
				}
		}

	///线程安全加锁
	gMutex.release();

	//no data found
	if (itr==theEnd)
		return LNS_NO_DATA_FOUND;


	//get Context Info
	if (GetLocalCI(theNaming.m_cContext)!=LNS_OK)
		return LNS_NO_DATA_FOUND;

	// Get DateTime
	theNaming.m_tmReDeployTime=m_NSInfo.NamingUptTime;

	return LNS_OK;

}

//##ModelId=41B50F8000A7
int LocalParseN::SetLocalCI(const CContext& theLocCI)
{
	if (fnChkContext(theLocCI)==-1)
		return LNS_BAD_CONTEXT; // bad Context Info

	m_NSInfo.NeedLocSvc=theLocCI.m_bNeedLocSvc;
	m_NSInfo.ContextID=theLocCI.m_nContextID;
	strcpy(m_NSInfo.CityCode,theLocCI.m_strCityCode.c_str());
	strcpy(m_NSInfo.EparchyCode,theLocCI.m_strEparchyCode.c_str());
	strcpy(m_NSInfo.OriginCode,theLocCI.m_strOriginCode.c_str());
	m_NSInfo.NamingUptTime=theLocCI.m_tmNamingUptTime;
	m_bIsUpdated=true;
	return LNS_OK;
}

//##ModelId=41B50F8000ED
int LocalParseN::SetRemoteNS(CNaming& theRemoteNS)
{
	if (theRemoteNS.m_strMachineIP.length() > LNS_IP_LENGTH ||
	    theRemoteNS.m_strMachineName.length()>LNS_NAME_LENGTH ||
	    theRemoteNS.m_strMachinePort.length()>LNS_PORT_LENGTH)
	    return LNS_BAD_NAME; // bad Remote Naming

	strcpy(m_NSInfo.MachineName,theRemoteNS.m_strMachineName.c_str());
	strcpy(m_NSInfo.IP,theRemoteNS.m_strMachineIP.c_str());
	strcpy(m_NSInfo.PORT,theRemoteNS.m_strMachinePort.c_str());
	m_bIsUpdated=true;
	return LNS_OK;
}

//##ModelId=41B50F80011F
int LocalParseN::AddNameSvc(string LogicName, CNaming& theNaming)
{
	CLNSNaming theBuf;

	if (fnChkNaming(theNaming)==-1)
		return LNS_BAD_NAME; // bad Naming

	if (LogicName.length() >LNS_NAME_LENGTH)
		return LNS_BAD_NAME; // bad LogicName

	memset(&theBuf,0x00,sizeof(CLNSNaming));
	strcpy(theBuf.LogicName,LogicName.c_str());
	strcpy(theBuf.IP,theNaming.m_strMachineIP.c_str());
	strcpy(theBuf.MachineName,theNaming.m_strMachineName.c_str());
	strcpy(theBuf.PhyName,theNaming.m_strPhysicalName.c_str());
	strcpy(theBuf.PORT,theNaming.m_strMachinePort.c_str());
	strcpy(theBuf.TypeCode,theNaming.m_cSvcTypeCode.c_str());

	///线程安全加锁
	gMutex.wait();
	m_listNaming.push_back(theBuf);
	m_bIsUpdated=true;
	///线程安全加锁
	gMutex.release();

	return LNS_OK;
}

//##ModelId=41B50F800147
int LocalParseN::DelNameSvc(string LogicName, int iIndex)
{
	if (!m_bReady)
		return LNS_NOT_READY;
	CLNSNaming theNaming;
	int count=0; // count for the iIndex



	if(iIndex >=0)
		{
			///线程安全加锁
			gMutex.wait();
			for (itr=m_listNaming.begin();itr!=m_listNaming.end();itr++)
				{
					if ((*itr)==LogicName)
						if (count++==iIndex)
							{
								m_listNaming.erase(itr);
								break;
							}
				}
			///线程安全加锁
			gMutex.release();

			//no data found
			if (itr==theEnd)
			return LNS_NO_DATA_FOUND;
		}
	else
		{
			///线程安全加锁
			gMutex.wait();
			strcpy(theNaming.LogicName,LogicName.c_str());
			m_listNaming.remove (theNaming);
			///线程安全加锁
			gMutex.release();
		}
	m_bIsUpdated=true;

	return LNS_OK;

}

//##ModelId=41B51F8700F7
int LocalParseN::fnChkFile()
{
/*
	long lfilesize;
	char cfilehead[256];
	memset(cfilehead,0x00,strlen(LNSHEAD));
	// file not ready
	if(!m_fp)
		return -5;

	//get file size
	fseek(m_fp, 0, SEEK_END);
 	lfilesize = ftell(m_fp);

	//check file size
	if ( ((lfilesize - sizeof(CLNSInfo)-strlen(LNSHEAD))%sizeof(CLNSNaming))!=0)
		return -2; // bad file size

	//check file head
	fseek(m_fp,0,SEEK_SET);
	if(fread(cfilehead,1,strlen(LNSHEAD),m_fp)!=strlen(LNSHEAD))
		return -3; // bad head (missing)

	if(strncmp(cfilehead,LNSHEAD,strlen(LNSHEAD))!=0)
		return -4;// bad head (bad file format)
*/
	return LNS_OK;
}

//##ModelId=41B6A61D0100
int LocalParseN::fnChkNaming(CNaming& theNaming)
{
	if( theNaming.m_strLogicName.length() > LNS_NAME_LENGTH ||
	     theNaming.m_strMachineIP.length() > LNS_IP_LENGTH ||
	     theNaming.m_strMachineName.length() > LNS_NAME_LENGTH ||
	     theNaming.m_strMachinePort.length() > LNS_PORT_LENGTH ||
	     theNaming.m_strPhysicalName.length() > LNS_NAME_LENGTH )
	     	return -1;
	else
		return LNS_OK;

}

//##ModelId=41B6A61D0197
int LocalParseN::fnChkContext(const CContext& theContext)
{
	if (
		theContext.m_strCityCode.length() > LNS_CODE_LENGTH ||
		theContext.m_strEparchyCode.length() > LNS_CODE_LENGTH ||
		theContext.m_strOriginCode.length() > LNS_CODE_LENGTH
		)
		return -1;
	else
		return LNS_OK;
}

//##ModelId=41B6AC2A0072
int LocalParseN::ListNaming(vector<CNaming> &vtheNaming)
{
	if (!m_bReady)
		return LNS_NOT_READY;
	CNaming theNaming;

	///线程安全加锁
	gMutex.wait();

	theEnd=m_listNaming.end();

	for (itr=m_listNaming.begin();itr!=theEnd;itr++)
		{
			theNaming.m_cSvcTypeCode=itr->TypeCode;
			theNaming.m_strLogicName=itr->LogicName;
			theNaming.m_strMachineIP=itr->IP;
			theNaming.m_strMachineName=itr->MachineName;
			theNaming.m_strMachinePort=itr->PORT;
			theNaming.m_strPhysicalName=itr->PhyName;
			if (GetLocalCI(theNaming.m_cContext)!=LNS_OK)
				{
					///线程安全加锁
					gMutex.release();
					return LNS_NO_DATA_FOUND;
				}
			theNaming.m_tmReDeployTime=m_NSInfo.NamingUptTime;
			vtheNaming.push_back(theNaming);
		}

	///线程安全加锁
	gMutex.release();

	return LNS_OK;
}

//##ModelId=41BEA4E9029D
int LocalParseN::DeleteAllNaming()
{
	///线程安全加锁
	gMutex.wait();
	m_listNaming.clear();
	///线程安全加锁
	gMutex.release();
	return LNS_OK;
}

//##ModelId=41C77ED1025D
bool LocalParseN::getNextValue(const string& theAll,const string& theName,string& theValue,int& offset)
{
	string theStart,theEndoff;
	theStart="<"+theName+">";
	theEndoff="</"+theName+">";

	int length=theName.size();
	int start=theAll.find(theStart,offset);
	int end=theAll.find(theEndoff ,offset);
	if (start==-1 || end == -1 || length==0 )
		return false;
//	cout<<"name:"<<theName<<" start:"<<start<<" end:"<<end<<" length:"<<length<<endl;
	theValue=theAll.substr(start+length+2,end-start-length-2);
//	cout<<"result:\n"<<theValue<<"\n"<<"the offset:"<<end+length+3<<endl;
	offset=end+length+3;
	return true;

}

//##ModelId=41C77ED103B2
bool LocalParseN::getValue(const string& theAll,const string& theName,string& theValue)
{
	int theStart=0;
	return getNextValue(theAll,theName,theValue,theStart);

}

//##ModelId=41C77ED200D8
bool LocalParseN::getLocalContextInfo(const string& theLocalContextInfo,CLNSInfo& theValue)
{

	string theBuf;
	memset(&theValue,0x00,sizeof(theValue));

	if(!getValue(theLocalContextInfo,"CITYCODE",theBuf)) return false;
	strncpy(theValue.CityCode,theBuf.c_str(),LNS_CODE_LENGTH);

	if(!getValue(theLocalContextInfo,"CONTEXTID",theBuf)) return false;
	theValue.ContextID=atoi(theBuf.c_str());

	if(!getValue(theLocalContextInfo,"EPARCHYCODE",theBuf)) return false;
	strncpy(theValue.EparchyCode,theBuf.c_str(),LNS_CODE_LENGTH);

	if(!getValue(theLocalContextInfo,"ORIGINCODE",theBuf)) return false;
	strncpy(theValue.OriginCode,theBuf.c_str(),LNS_CODE_LENGTH);

	if(!getValue(theLocalContextInfo,"NAMINGUPTTIME",theBuf)) return false;
	CRemoteNameServiceImpl::charTOtm(theBuf.c_str(),theValue.NamingUptTime);

//	if(!getValue(theLocalContextInfo,"NEEDLOCALSERVICE",theBuf)) return false;
//	if(theBuf=="true" || theBuf=="TRUE" || theBuf=="True")
		theValue.NeedLocSvc=true;
//	else
//		theValue.NeedLocSvc=false;

	return true;

}

//##ModelId=41C77ED20196
bool LocalParseN::getRemoteNamingSrv(const string& theRemoteNamingSrv,CLNSInfo& theValue)
{
	string theBuf;
	if(!getValue(theRemoteNamingSrv,"IP",theBuf)) return false;
	strncpy(theValue.IP,theBuf.c_str(),LNS_IP_LENGTH);

	if(!getValue(theRemoteNamingSrv,"PORT",theBuf)) return false;
	strncpy(theValue.PORT,theBuf.c_str(),LNS_PORT_LENGTH);

	if(!getValue(theRemoteNamingSrv,"MACHINENAME",theBuf)) return false;
	strncpy(theValue.MachineName,theBuf.c_str(),LNS_NAME_LENGTH);

	return true;
}

//##ModelId=41C77ED2024B
bool LocalParseN::getNaming(const string& theNaming,CLNSNaming& theValue)
{
	string theBuf;

	if(!getValue(theNaming,"IP",theBuf)) return false;
	strncpy(theValue.IP,theBuf.c_str(),LNS_IP_LENGTH);

	if(!getValue(theNaming,"LOGICALNAME",theBuf)) return false;
	strncpy(theValue.LogicName,theBuf.c_str(),LNS_NAME_LENGTH);

	if(!getValue(theNaming,"MACHINENAME",theBuf)) return false;
	strncpy(theValue.MachineName,theBuf.c_str(),LNS_NAME_LENGTH);

	if(!getValue(theNaming,"PHYSICALNAME",theBuf)) return false;
	strncpy(theValue.PhyName,theBuf.c_str(),LNS_NAME_LENGTH);

	if(!getValue(theNaming,"PORT",theBuf)) return false;
	strncpy(theValue.PORT,theBuf.c_str(),LNS_PORT_LENGTH);

	if(!getValue(theNaming,"TYPECODE",theBuf)) return false;
	strncpy(theValue.TypeCode,theBuf.c_str(),1);

	return true;
}



