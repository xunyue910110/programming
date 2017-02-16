#include "MdbParamManager.h"

MdbParamManager::MdbParamManager()
{
	m_db        = NULL;
	m_session   = NULL;
	m_connected = false;
	m_errno     = 0;
	m_errMsg    = "";
}

MdbParamManager::~MdbParamManager()
{
  if(m_connected==true)
  {
    end();
  }
}

bool MdbParamManager::initialization(const MdbInfo& mdbInfo)
{
	m_mdbInfo = mdbInfo;
	try
	{
		m_db = InstanceFactory::getDBInstance(
		                          m_mdbInfo.m_dbType
		                          ,m_mdbInfo.m_dbServer.c_str()						      
                              ,m_mdbInfo.m_hostName.c_str()
                              ,m_mdbInfo.m_port						       
                              ,m_mdbInfo.m_user.c_str()  						       
                              ,m_mdbInfo.m_passwd.c_str());
		m_session = m_db->connect();
	  m_connected = true;
	  return true;
	}
	catch(LINKAGE_MDB::Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		end();
		#ifdef _DEBUG_
		  cout<<"m_errMsg = "<<m_errMsg<<" "<<__FILE__<<__LINE__<<endl;
		#endif
		return false;
	}
	return true;
}
bool MdbParamManager::end()
{
	if( m_db != NULL )
	{
		if( m_session != NULL )
		{
			m_db->disconnect(m_session);
			m_connected = false;
		}
		m_session=NULL;
		delete m_db;
		m_db=NULL;
	}
  return true;
}

void MdbParamManager::getMdbErrMsg(int& r_errno,string& r_errMsg)
{
	r_errno = m_errno;
	r_errMsg = m_errMsg;
}

bool MdbParamManager::getMdbNotify(string& r_errMsg)
{
	if(m_errno<0)
	{
	  r_errMsg = m_errMsg;
		return true;
	}
  else
  {
	  r_errMsg = "";
		return false;
  }
}
