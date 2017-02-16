#ifndef MDBPARAMMANAGER_H_INCLUDED
#define MDBPARAMMANAGER_H_INCLUDED

#include <vector>
#include "Session.h"
#include "DataBase.h"
#include "base/supdebug.h"
#include "InputParams.h"
#include "mdb/Mdb_Exception.h"
#include "InstanceFactory.h"

#include "Mdb_Property.h"
#include "MdbInfo.h"

class MdbParamManager
{
	public:
		MdbParamManager();
		virtual ~MdbParamManager();
		bool initialization(const MdbInfo& mdbInfo);
		bool end();
		
		template<class T>
		int search(const T& record, T& result);
		
		template<class T>
		int searchMulti(const T& record, vector<T>& result);
		
		void getMdbErrMsg(int& m_errno,string& m_errMsg);
		
		bool getMdbNotify(string& r_errMsg);

	private :
		MdbInfo        m_mdbInfo;
		DataBase       *m_db;
		Session        *m_session;
		bool           m_connected;
		int            m_errno;
		string         m_errMsg;
};


template <class T>
int MdbParamManager::search(const T& record, T& result)
{
  int         iRet=0;
	Mdb_Property property;
	record.getProperty(property);
	InputParams *pInputParamsseIndexs = NULL;
	InputParams *pInputParamsseOthers = NULL;
	m_errno = 0;
	
	if (property.indexParams > 0)
	{
	  pInputParamsseIndexs = new InputParams(property.indexParams);
	}
	if (property.otherParams > 0)
	{
	  pInputParamsseOthers = new InputParams(property.otherParams);
	}
	try
	{
		if (!m_connected)
		{
			m_db = InstanceFactory::getDBInstance(m_mdbInfo.m_dbType,m_mdbInfo.m_dbServer.c_str(),m_mdbInfo.m_hostName.c_str(),m_mdbInfo.m_port,m_mdbInfo.m_user.c_str(),m_mdbInfo.m_passwd.c_str());
		  m_session = m_db->connect();
			m_connected = true;
		}
		
		if (pInputParamsseIndexs != NULL)
		{
		  record.setKeyParams(pInputParamsseIndexs);
		}
		if (pInputParamsseOthers != NULL)
		{
		  record.setOtherParams(pInputParamsseOthers);
		}
		m_session->select(property.tableName.c_str(),property.indexName.c_str(),pInputParamsseIndexs,property.whereCaluse.c_str(),pInputParamsseOthers);
		
		if (m_session->getNextRec())
		{
		  result.convert(m_session);
		  iRet=1;
		}
		else
		{
			iRet = 0;
		}
	}
	catch(LINKAGE_MDB::Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		end();
		iRet = -1;
		m_errno = iRet;
		#ifdef _DEBUG_
		  cout<<"m_errMsg = "<<m_errMsg<<" "<<__FILE__<<__LINE__<<endl;
		#endif
	}
	if (pInputParamsseIndexs != NULL)
	{
	  delete pInputParamsseIndexs;
	  pInputParamsseIndexs=NULL;
	}
	if (pInputParamsseOthers != NULL)
	{
	  delete pInputParamsseOthers;
	  pInputParamsseOthers=NULL;
	}
  return iRet;
}

template <class T>
int MdbParamManager::searchMulti(const T& record, vector<T>& result)
{
	Mdb_Property property;
	record.getProperty(property);
	InputParams *pInputParamsseIndexs = NULL;
	InputParams *pInputParamsseOthers = NULL;
	m_errno = 0;
	T temp;
  int         iRet=0;
	
	if (property.indexParams > 0)
	{
	  pInputParamsseIndexs = new InputParams(property.indexParams);
	}
	if (property.otherParams > 0)
	{
	  pInputParamsseOthers = new InputParams(property.otherParams);
	}
	try
	{
		if (!m_connected)
		{
			m_db = InstanceFactory::getDBInstance(m_mdbInfo.m_dbType,m_mdbInfo.m_dbServer.c_str(),m_mdbInfo.m_hostName.c_str(),m_mdbInfo.m_port,m_mdbInfo.m_user.c_str(),m_mdbInfo.m_passwd.c_str());
		  m_session = m_db->connect();
			m_connected = true;
		}
		
		if (pInputParamsseIndexs != NULL)
		{
		  record.setKeyParams(pInputParamsseIndexs);
		}
		if (pInputParamsseOthers != NULL)
		{
		  record.setOtherParams(pInputParamsseOthers);
		}
		m_session->select(property.tableName.c_str(),property.indexName.c_str(),pInputParamsseIndexs,property.whereCaluse.c_str(),pInputParamsseOthers);
		while(m_session->getNextRec())
		{
			temp.convert(m_session);
		  result.push_back(temp);
		}
		iRet = result.size();
	}
	catch(LINKAGE_MDB::Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		end();
		iRet = -1;
		m_errno = iRet;
		#ifdef _DEBUG_
		  cout<<"m_errMsg = "<<m_errMsg<<" "<<__FILE__<<__LINE__<<endl;
		#endif
	}
	if (pInputParamsseIndexs != NULL)
	{
	  delete pInputParamsseIndexs;
	  pInputParamsseIndexs=NULL;
	}
	if (pInputParamsseOthers != NULL)
	{
	  delete pInputParamsseOthers;
	  pInputParamsseOthers=NULL;
	}
	return iRet;
}

#endif

