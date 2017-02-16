#include "mwci/IMWCaller.h"

//##ModelId=4190B04101BF
IMWCaller::~IMWCaller()
{
}
//##ModelId=41D16E4C00E8
FILE* IMWCaller::m_LogFilefp=NULL;
//##ModelId=419314470316
bool IMWCaller::IsConnect()
{
    return m_bIsConn;
}


//##ModelId=419D92760392
string IMWCaller::GetCltname() const
{
    return m_sCltname;
}


//##ModelId=419D9277009A
void IMWCaller::SetCltname(string &left)
{
    m_sCltname = left;
}


//##ModelId=419D927701E5
string IMWCaller::GetGrpname() const
{
    return m_sGrpname;
}


//##ModelId=419D9277028F
void IMWCaller::SetGrpname(string &left)
{
    m_sGrpname = left;
}


//##ModelId=419D927703BB
string IMWCaller::GetPasswd() const
{
    return m_sPasswd;
}


//##ModelId=419D9278007E
void IMWCaller::SetPasswd(string &left)
{
    m_sPasswd = left;
}


//##ModelId=419D927801B4
string IMWCaller::GetSrvIP() const
{
    return m_sSrvIP;
}


//##ModelId=419D9278025E
void IMWCaller::SetSrvIP(string &left)
{
    m_sSrvIP = left;
}


//##ModelId=419D92780395
string IMWCaller::GetSrvPort() const
{
    return m_sSrvPort;
}


//##ModelId=419D92790057
void IMWCaller::SetSrvPort(string &left)
{
    m_sSrvPort = left;
}


//##ModelId=419D92790197
string IMWCaller::GetUsrname() const
{
    return m_sUsrname;
}


//##ModelId=419D92790242
void IMWCaller::SetUsrname(string &left)
{
    m_sUsrname = left;
}

 IMWCaller& IMWCaller::operator=(const IMWCaller& right)
 {
	if( this == &right ) return *this;

	m_sSrvIP=right.m_sSrvIP;
	m_sSrvPort=right.m_sSrvPort;
	m_bMTFlag=right.m_bMTFlag;
	m_bIsConn=false;
//	m_nameService=right.m_nameService;
//	m_LocalService=right.m_LocalService;
	m_sUsrname=right.m_sUsrname;
	m_sCltname=right.m_sCltname;
	m_sPasswd=right.m_sPasswd;
	m_sGrpname=right.m_sGrpname;
	m_sSvcName=right.m_sSvcName;
	return *this;
 }
