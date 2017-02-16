#include "config-all.h"
#include "InfoMMApp.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "CException.h"
#ifndef _PSW_FROM_FILE_//江苏模式,采用统一密码管理
  #include "pass/DBPassOper.h"  // modified by jiaoll 2010-1-20 16:22
#endif

//批量部分 wangfc@lianchuang.com
#define MAX_BATCH_COMMIT_RECORD_SIZE 1000

InfoMMApp g_application;
Application * const theApp = &g_application;


InfoMMApp::InfoMMApp()
{
	m_dbDescMap.clear();
	m_mmDescMap.clear();

	m_dbiMap.clear();
	m_mmiMap.clear();

	m_destRuleVector.clear();
	//add by xueyf@lianchuang.com
  m_dbcrmDescMap.clear();
  m_dbcrmMap.clear();
  m_crmnum=0;
  m_actnum = 0 ;
	//end
}


InfoMMApp::~InfoMMApp()
{
	m_dbDescMap.clear();
	m_mmDescMap.clear();

	for (map<string, DbInterface*>::iterator iterd=m_dbiMap.begin(); iterd!=m_dbiMap.end(); ++iterd)
	{
		delete iterd->second;
	}
	m_dbiMap.clear();
	for (map<string, MmInterface*>::iterator iterm=m_mmiMap.begin(); iterm!=m_mmiMap.end(); ++iterm)
	{
		delete iterm->second;
	}
	m_mmiMap.clear();
	//增加全量倒换 add by xueyf@lianchuang.com
	for (map<string, DbInterface*>::iterator iterc=m_dbcrmMap.begin(); iterc!=m_dbcrmMap.end(); ++iterc)
	{
		delete iterc->second;
	}
	m_dbcrmMap.clear();
	//end

	m_destRuleVector.clear();
}


bool InfoMMApp::initialization()
{
	setoptstr("c:");

	// normally, call standard initialization first
  if (!FileApplication::initialization())
    return false;

#ifndef _DEBUG_
	set_asdaemon(true);
	set_onlyone(true);
	set_runinbkg(true);
#endif

	//-----------------------------------------------------------------------//
	// ADD YOUR INITIALIZATION CODE HERE
	//-----------------------------------------------------------------------//
	char channelNo[4];
	string root,sPath,sParamName,sParamValue;
	root=SECTDELIM+string(m_name);
	sprintf(channelNo, "%d", m_channelNo);
	sPath = root + SECTDELIM + channelNo ;

	//(1)获取系统默认数据库信息
	sParamName="sys_db_username";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL, sPath + SECTDELIM + sParamValue);
		return false;
	}
	m_sysDbDesc.m_username = sParamValue;
#ifdef _PSW_FROM_FILE_	// modified by jiaoll 2010-1-20 16:22
	sParamName="sys_db_password";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL, sPath + SECTDELIM + sParamValue);
		return false;
	}
	m_sysDbDesc.m_password = sParamValue;
	sParamName="sys_db_servname";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL, sPath + SECTDELIM + sParamValue);
		return false;
	}
	m_sysDbDesc.m_servname = sParamValue;
#else
	string cDbCode1;
	sParamName="sys_db_DBCODE";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL, sPath + SECTDELIM + sParamValue);
		return false;
	}
	cDbCode1 = sParamValue;
	
	DBPass dbpass1;		
	DBPassOper* dbPassOper1 = DBPassOper::getOper();	
	if( ! dbPassOper1->GetDBPass(dbpass1, cDbCode1, m_sysDbDesc.m_username))
  	{
    	m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:GetDBPass error");
    	return false;
  	}
	m_sysDbDesc.m_password = dbpass1.DecPasswd;
  m_sysDbDesc.m_servname = dbpass1.ServerName;
  //printf("DB--PassWord:%s,  ServName:%s\n", m_sysDbDesc.m_password,m_sysDbDesc.m_servname);
  delete dbPassOper1; 
#endif 
	//(2)获取系统定义的所有数据库信息
	//(2.1)所有物理库信息
	int dbnum = 0;
	sParamName="obj_db_num";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	dbnum = atoi(sParamValue.c_str());
  m_actnum = dbnum ; //物理数目 add by xueyf@lianchuang.com
	for (int i=0; i<dbnum; ++i)
	{
		char ci[4];
		sprintf(ci, "%d", i);
		string si = ci;
		DbDesc dbDesc;

		sParamName = "obj_db_name_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		string objdbname = sParamValue;
		sParamName = "obj_db_username_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbDesc.m_username = sParamValue;
// modified by jiaoll 2010-1-20 16:22
#ifdef _PSW_FROM_FILE_		
		sParamName = "obj_db_password_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbDesc.m_password = sParamValue;
		sParamName = "obj_db_servname_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbDesc.m_servname = sParamValue;
#else
		string cDbCode2;
		sParamName = "obj_db_DBCODE_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		cDbCode2 = sParamValue;
		
		DBPass dbpass2;		
		DBPassOper* dbPassOper2 = DBPassOper::getOper();
		if( ! dbPassOper2->GetDBPass(dbpass2, cDbCode2, dbDesc.m_username))
  		{
    		m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:GetDBPass error");
    		return false;
  		}
		dbDesc.m_password = dbpass2.DecPasswd;
  	dbDesc.m_servname = dbpass2.ServerName;
  	//printf("DB--PassWord:%s,  ServName:%s\n", dbDesc.m_password,dbDesc.m_servname);
  	delete dbPassOper2; 
#endif 		
		m_dbDescMap.insert(map<string, DbDesc>::value_type(objdbname, dbDesc));

		DbInterface *dbi = new DbInterface();
		//记录下来名称 wangfc@lianchuang.com 2010.03.10
		dbi->setobjalisename( objdbname );
		m_dbiMap.insert(map<string, DbInterface*>::value_type(objdbname, dbi));
	}

	//(2.2)所有内存库信息
	int mmnum = 0;
	sParamName="obj_mm_num";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	mmnum = atoi(sParamValue.c_str());

	for (int i=0; i<mmnum; ++i)
	{
		char ci[4];
		sprintf(ci, "%d", i);
		string si = ci;
		MmDesc mmDesc;
		
		sParamName = "obj_mm_type_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		int tmp = atoi(sParamValue.c_str());
		if(tmp==0) mmDesc.m_dbtype = MDB;
		else if(tmp==1) mmDesc.m_dbtype = ODBCMDB;
		else if(tmp==2) mmDesc.m_dbtype = RMDB;

		sParamName = "obj_mm_alianame_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_alianame = sParamValue;

		sParamName = "obj_mm_name_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_dbname = sParamValue;
		sParamName = "obj_mm_hostname_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_hostname = sParamValue;
		sParamName = "obj_mm_port_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_port = atoi(sParamValue.c_str());

	  //modify by chenxt 2008-12-09
		sParamName = "obj_mm_username_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_username = sParamValue;
		sParamName = "obj_mm_password_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		mmDesc.m_password = sParamValue;
		
		m_mmDescMap.insert(map<string, MmDesc>::value_type(mmDesc.m_alianame, mmDesc));

		MmInterface *mmi = new MmInterface();
		//记录下来名称 wangfc@lianchuang.com 2010.04.07
		mmi->setalisname ( mmDesc.m_alianame );
		m_mmiMap.insert(map<string, MmInterface*>::value_type(mmDesc.m_alianame, mmi));
	}
// 读取所有CRM库信息 add by xueyf@lianchuang.com
  int crmnum = 0;
	sParamName="obj_crmdb_num";
	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
	{
		crmnum = 0;
	}
	crmnum = atoi(sParamValue.c_str());
	m_crmnum=crmnum;
  for(int i=0;i<crmnum;i++)
	{
		char ci[4];
		sprintf(ci, "%d", i);
		string si = ci;
		DbDesc dbcrmDesc;

		sParamName = "obj_crmdb_name_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		string objdbname = sParamValue;
		sParamName = "obj_crmdb_username_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbcrmDesc.m_username = sParamValue;
// modified by wangfc 2010-5-19 10:22
#ifdef _PSW_FROM_FILE_		
		sParamName = "obj_crmdb_password_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbcrmDesc.m_password = sParamValue;
		sParamName = "obj_crmdb_servname_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		dbcrmDesc.m_servname = sParamValue;
#else
		string cDbCode2;
		sParamName = "obj_crmdb_DBCODE_"+si;
		if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
			return false;
		}
		cDbCode2 = sParamValue;
		
		DBPass dbpass2;		
		DBPassOper* dbPassOper2 = DBPassOper::getOper();
		if( ! dbPassOper2->GetDBPass(dbpass2, cDbCode2, dbcrmDesc.m_username))
  		{
    		m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:GetDBPass error");
    		return false;
  		}
		dbcrmDesc.m_password = dbpass2.DecPasswd;
  	dbcrmDesc.m_servname = dbpass2.ServerName;
  	//printf("DB--PassWord:%s,  ServName:%s\n", dbDesc.m_password,dbDesc.m_servname);
  	delete dbPassOper2; 
#endif 	
		
		m_dbcrmDescMap.insert(map<string, DbDesc>::value_type(objdbname, dbcrmDesc));

		DbInterface *dbicrm = new DbInterface();
		m_dbcrmMap.insert(map<string, DbInterface*>::value_type(objdbname, dbicrm));
	}
//---------end	

	//(3)获取文件路径、休眠时间等信息
	//add by xueyf@lianchuang.com 增量方式和ACT-->MDB有效
	if ( m_crmnum  == 0 )
	{
  	sParamName="file_path";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
  		return false;
  	}
  	m_filepath = sParamValue;
  	sParamName="bak_file_path";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
  		return false;
  	}
  	m_bakfilepath = sParamValue;
  	sParamName="err_file_path";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
  		return false;
  	}
  	m_errfilepath = sParamValue;
  	sParamName="allow_max_errnum";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_errnum = -1;
  	}
  	else
  	{
  		m_errnum = atoi(sParamValue.c_str());
  	}
  	sParamName="separator_sign";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
  		return false;
  	}
  	
  	char c1[2];
  	char c2[2];
  	c1[0] = sParamValue[2];
  	c1[1] = '\0';
  	c2[0] = sParamValue[3];
  	c2[1] = '\0';
  	char c=atoi(c1)*16+atoi(c2);
  	m_seperator = c;
  	
  	sParamName="sleep_time";
  	if ( !m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
  		return false;
  	}
  	m_sleeptime = atoi(sParamValue.c_str());
  	//  2010.03.16 增加特殊对td_b_info_destrule判断(兼容广州模式)，可以是MDB ,ACT分离入库
  	sParamName  = "HOSTNAME";
  	sParamValue = "";
  	m_hostName  = "";
  	if ( m_theSysParam->getValue(sPath, sParamName, sParamValue) )
  	{
  		m_hostName= sParamValue;
  	}
    //-------end    
  	//(4)获取当前时间
  	//getcurrtime();
  
  	//(5)从默认数据库获取:最后更新时间,最后一次处理错误的文件名称,各类信息长度,各类信息划分规则
  	//(5.1)连接默认数据库
  	//DbInterface sysdbi;
  	try
  	{
  		m_sysdbi.connect(m_sysDbDesc.m_username, m_sysDbDesc.m_password, m_sysDbDesc.m_servname);
  	}
  	catch (CException e)
  	{
  		m_theErrorMessages->insert(DB_ERROR_CONNECT, e.GetString());
  		return false;
  	}
  
  	//(5.11)获取省中心入库的数据名称
  	string dbNames = "";
  	string mmNames = "";
  	char sql[2000] = "\0";
  	string	strSql = "";
  	sprintf(sql, "SELECT LTRIM(RTRIM(PRE_VALUE_S1)),LTRIM(RTRIM(PRE_VALUE_S2)) FROM TD_B_SYSPARAM WHERE SYS_PARAM_CODE='DATALOAD_PROVINCEINFO_DESTDB' AND USE_TAG='1' AND ROWNUM<2 ");
  	strSql = sql;
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);
  		while(m_sysdbi.getNext())
  		{
  			dbNames = m_sysdbi.getColumnStringValue(1);
  			mmNames = m_sysdbi.getColumnStringValue(2);
  		}
  	}
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  		return false;
  	}
  	//一个目标规则涉及的所有的目标物理库
  	vector<string> tmpobjdbnameVector;
  	vector<string> tmpobjmmnameVector;
  	tmpobjdbnameVector.clear();
  	tmpobjmmnameVector.clear();	
  	StringUtil::split(dbNames, ";", tmpobjdbnameVector, false);
  	StringUtil::split(mmNames, ";", tmpobjmmnameVector, false);
  	for (vector<string>::iterator itert=tmpobjdbnameVector.begin(); itert!=tmpobjdbnameVector.end(); ++itert)
  	{
  		if (m_dbiMap.find(*itert) != m_dbiMap.end())
  		{
  			m_dbisPro.push_back(m_dbiMap[*itert]);
  		}
  		else
  		{
  			m_theErrorMessages->insert(DB_ERROR_NOCFG, "物理库"+(*itert)+"未配置,请核查!");
  			return false;
  		}
  	}
  	for (vector<string>::iterator itert=tmpobjmmnameVector.begin(); itert!=tmpobjmmnameVector.end(); ++itert)
  	{
  		if (m_mmiMap.find(*itert) != m_mmiMap.end())
  		{
  			m_mmisPro.push_back(m_mmiMap[*itert]);
  		}
  		else
  		{
  			m_theErrorMessages->insert(MM_ERROR_NOCFG, "内存库"+(*itert)+"未配置,请核查!");
  			return false;
  		}
  	}
  			
  	//(5.2)获取最后一次处理的文件名称
  	m_lastdealfilename = "NULL";
  	sprintf(sql, "SELECT LTRIM(RTRIM(VALUE_S)) FROM TD_B_SYSPARAM WHERE SYS_PARAM_CODE='DATALOAD_LAST_FILENAME' AND CHANNEL_NO=%d AND USE_TAG='1' AND ROWNUM<2 ", m_channelNo);
  	strSql = sql;
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);
  		while(m_sysdbi.getNext())
  		{
  			m_lastdealfilename = m_sysdbi.getColumnStringValue(1);
  		}
  	}
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  		return false;
  	}
  	
  	if (m_lastdealfilename == "NULL")
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(ERROR_DBPARAM_CFG, "最后更新文件名称在TD_B_SYSPARAM 表未配置,请核查!");
  		return false;
  	}
  
  	//(5.4)获取各类信息长度
  	memset(m_infolens, 0x0, (MAX_INFOTYPE_VALUE+1)*sizeof(int));
  	memset(m_loadtables, 0x0, (MAX_INFOTYPE_VALUE+1)*sizeof(LoadTable));
  	strSql =  "	SELECT	INFO_TYPE,INFO_FIELD_NUM,INFO_KEY_FIELDPOS,DB_TABLE_NAME,DB_COLUMN_NUM,DB_KEY_COLUMNPOS,DB_COLUMN_DESC,		";
  	strSql += "			MM_TABLE_NAME,MM_COLUMN_NUM,MM_KEY_INDEXNAME,MM_KEY_COLUMNPOS,MM_INDEXNAME_PK,MM_COLUMN_DESC,TRIGGER_FLAG	";
  	strSql += "	FROM	TD_B_INFO_LOADRULE			";
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);
  		while(m_sysdbi.getNext())
  		{			
  			int infotype = m_sysdbi.getColumnIntValue(1);
  			int infolen = m_sysdbi.getColumnIntValue(2);
  
  			LoadTable loadTable;
  			loadTable.m_infotype = m_sysdbi.getColumnIntValue(1);
  			loadTable.m_infofieldnum = m_sysdbi.getColumnIntValue(2);
  			loadTable.parseinfokeyfieldpos(m_sysdbi.getColumnStringValue(3));
  			loadTable.m_dbtablename = m_sysdbi.getColumnStringValue(4);
  			loadTable.m_dbcolumnnum = m_sysdbi.getColumnIntValue(5);
  			loadTable.parsedbkeyfieldpos(m_sysdbi.getColumnStringValue(6));
  			loadTable.parseDbColumns(m_sysdbi.getColumnStringValue(7));
  			loadTable.m_mmtablename = m_sysdbi.getColumnStringValue(8);
  			loadTable.m_mmcolumnnum = m_sysdbi.getColumnIntValue(9);
  			loadTable.m_mmkeyindexname = m_sysdbi.getColumnStringValue(10);
  			loadTable.parsemmkeyfieldpos(m_sysdbi.getColumnStringValue(11));
  			loadTable.m_mmindexnamepk = m_sysdbi.getColumnStringValue(12);
  			loadTable.parseMmColumns(m_sysdbi.getColumnStringValue(13));
  			loadTable.m_triggerflag = (m_sysdbi.getColumnStringValue(14))[0];
  			
  			m_infolens[infotype] = infolen;
  			m_loadtables[infotype] = loadTable;
  		}
  	}
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  		return false;
  	}
  	
  	//(5.5)获取目标分拣规则,增加批量入库配置和MDB ACT分离 兼容广东模式 wangfc@lianchuang.com 2010.04.22
  	m_destRuleVector.clear();
  	//strSql =  " SELECT INFO_TYPE,EPARCHY_CODE,OBJ_DB_NAMES,OBJ_MM_NAMES FROM TD_B_INFO_DESTRULE ORDER BY 1,2 ";
  	strSql =  " SELECT INFO_TYPE,EPARCHY_CODE,OBJ_DB_NAMES,OBJ_MM_NAMES,OBJ_DB_DEALTAG,OBJ_DB_IN_TABLENAMES,OBJ_DB_IN_PROCS,";
  	strSql += " OBJ_PROVDB_IN_TABLENAMES,OBJ_PROVDB_IN_PROCS FROM TD_B_INFO_DESTRULE  ";//order by 1,2
  	if (m_hostName.size() > 0 )
	  {
	    strSql += " where hostname = '";
	    strSql += m_hostName;
	    strSql += "' ";
  	}
	  strSql += " ORDER BY 1,2 ";
    m_Batchmaxrecordsize               = MAX_BATCH_COMMIT_RECORD_SIZE ;//默认写死。如有需要，可以参数配置 wangfc@lianchuang.com 2010-03.19
  	map<int, DestRule>                 m_DisctntInfoTypeDescMap;
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);	
  		int destinfotype = 0 ;
  		while(m_sysdbi.getNext())
  		{		
  			DestRule destRule;
  			destRule.m_infotype = m_sysdbi.getColumnIntValue(1);
  			
  			destRule.m_srcdbnum = atoi((m_sysdbi.getColumnStringValue(2)).c_str());
  			destRule.m_objdbnames = m_sysdbi.getColumnStringValue(3);
  			destRule.m_objmmnames = m_sysdbi.getColumnStringValue(4);
        /*
           批量增加部分 wangfc@lianchuang.com 2010.03.29
        */
        destRule.m_objdeal               =  m_sysdbi.getColumnIntValue(5);
  	  	destRule.m_obj_db_tablenames     =  m_sysdbi.getColumnStringValue(6);
  		  destRule.m_obj_db_procs          =  m_sysdbi.getColumnStringValue(7);
  		  destRule.m_obj_provdb_tablenames =  m_sysdbi.getColumnStringValue(8);
  		  destRule.m_obj_provdb_procs      =  m_sysdbi.getColumnStringValue(9);		 
  		  destinfotype                     =  destRule.m_infotype; 
  		  if ( destRule.m_objdeal == 1 )
  		  {
  		   /*
  		     一个增量通道目前的设计模式，之可能是处理一个库的增量文件，而此段代码
  		     将分配了多次的内存（同一个infotype）
  		   */
  #ifdef _DEBUG_		   
          printf("读取批量入库参数并加载\n");
  #endif  
  		    LoadTable destloadTable          = m_loadtables[destinfotype] ;
    		  destRule.m_MaxRecordCount        = m_Batchmaxrecordsize ;
    		  //如果没有分配的话，直接分配  2010-6-23 12:21:25 修正多次分配内存BUG
//    		  if ( m_DisctntInfoTypeDescMap.find(destinfotype) ==  m_DisctntInfoTypeDescMap.end() )
    		  {
  #ifdef _DEBUG_	    		    
    		      printf("读取批量入库参数并加载 infotype=%d\n",destinfotype); 
  #endif        		     
        		  destRule.setBatchParam    ( destloadTable.m_dbcolumns  ,destloadTable.m_dbcolumnnum,m_Batchmaxrecordsize );
        		  destRule.setBatchProvParam( destloadTable.m_dbcolumns  ,destloadTable.m_dbcolumnnum,m_Batchmaxrecordsize );
        		  destRule.ResetBatchParam ();	
              //2010-6-23 12:21:25 修正多次分配内存BUG  
//            m_DisctntInfoTypeDescMap.insert(map<int, DestRule>::value_type(destinfotype, destRule));        		  
    		  }
//   		  else
//    		  {
//    		      destRule.m_CurBatchRecordcount       =  m_DisctntInfoTypeDescMap[destinfotype].m_CurBatchRecordcount;
//		          destRule.m_provdata                  =  m_DisctntInfoTypeDescMap[destinfotype].m_provdata ;
//		          destRule.m_CurBatchProvRecordcount   =  m_DisctntInfoTypeDescMap[destinfotype].m_CurBatchProvRecordcount;
//		          destRule.m_MaxRecordCount            =  m_DisctntInfoTypeDescMap[destinfotype].m_MaxRecordCount ; 
//		          destRule.ResetBatchParam ();
//    		  }
  #ifdef _DEBUG_		   
          printf("读取批量入库参数并加载(完)\n");
  #endif  		
 
  		  }		
  			//----------end
  			m_destRuleVector.push_back(destRule);
  		}
  		//m_DisctntInfoTypeDescMap.clear() ;//2010-6-23 12:21:25 修正多次分配内存BUG
  	}
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  		return false;
  	}
  	::partial_sort(m_destRuleVector.begin(), m_destRuleVector.end(), m_destRuleVector.end());
  
  	//(6)关闭系统数据库连接
  	try
  	{
  		m_sysdbi.disconnect();
  	}
  	catch (CException e)
  	{
  		m_theErrorMessages->insert(DB_ERROR_DISCONNECT, e.GetString());
  		return false;
  	}
  
  	//(7)初始化对应的类成员
  	m_fileDeal.setfilepath(m_filepath.c_str());
  	m_fileDeal.setbakfilepath(m_bakfilepath.c_str());
  	m_fileDeal.setinfolen(m_infolens);
  	m_fileDeal.setseperator(m_seperator.c_str());
  	m_fileDeal.setloadtables(m_loadtables);
  }
	m_tag = 1;
	
	return true;
}


bool InfoMMApp::loopProcess()
{
	while(!run())
	{
		m_stop = true;
		return false;
	}

	return true;
}


bool InfoMMApp::end()
{
	m_dbDescMap.clear();
	m_mmDescMap.clear();
	// add by xueyf@lianchuang.com
	m_dbcrmDescMap.clear();
	//(6)关闭系统数据库连接
	try
	{
		m_sysdbi.disconnect();
	}
	catch (CException e)
	{
		m_theErrorMessages->insert(DB_ERROR_DISCONNECT, e.GetString());
		return false;
	}
	
	for (map<string, DbInterface*>::iterator iterd=m_dbiMap.begin(); iterd!=m_dbiMap.end(); ++iterd)
	{
		delete iterd->second;
	}
	m_dbiMap.clear();
	for (map<string, MmInterface*>::iterator iterm=m_mmiMap.begin(); iterm!=m_mmiMap.end(); ++iterm)
	{
		delete iterm->second;
	}
	m_mmiMap.clear();
	//全量OEC倒换 释放 add by xueyf@lianchuang.com
	for (map<string, DbInterface*>::iterator iterc=m_dbcrmMap.begin(); iterc!=m_dbcrmMap.end(); ++iterc)
	{
		delete iterc->second;
	}
	m_dbcrmMap.clear();	//end
  /*
    批量增加部分 wangfc@lianchuang.com 2010-3-29 15:11:32内存释放
  */	
  map<int, DestRule>                 m_DisctntInfoTypeDescMap;
  m_DisctntInfoTypeDescMap.clear();
	for( int j = 0 ; j < m_destRuleVector.size() ; ++j )
	{
	  if ( m_destRuleVector[j].m_objdeal == 1 )
	  {
	     // 2010-6-23 12:21:25 修正多次分配内存BUG  
	    // if (  m_DisctntInfoTypeDescMap.find( m_destRuleVector[j].m_infotype) == m_DisctntInfoTypeDescMap.end())
	     { 	      
	       m_destRuleVector[j].BatchParamclear();
	      // m_DisctntInfoTypeDescMap.insert(map<int, DestRule>::value_type(m_destRuleVector[j].m_infotype, m_destRuleVector[j]));	        
	     }
	  }
	}
	m_DisctntInfoTypeDescMap.clear();
	m_destRuleVector.clear();

	return Application::end();
}

// 批量计算器复位 wangfc@lianchuang.com 2010-5-19 15:11:14
void InfoMMApp::ResetBatchdestRuleParam()
{
	for( int j = 0 ; j < m_destRuleVector.size() ; ++j )
	{
	  if (  m_destRuleVector[j].m_objdeal == 1 )
	    m_destRuleVector[j].ResetBatchParam();
	}
	
}


bool InfoMMApp::run()
{
	if (m_tag==1)
	{
		//(1)连接上所有数据库
		//(1.1)打开所有物理数据库连接
		for (map<string, DbInterface*>::iterator iter=m_dbiMap.begin(); iter!=m_dbiMap.end(); ++iter)
		{
			string dbname = iter->first;
			DbInterface *dbi = iter->second;
			DbDesc dbDesc = m_dbDescMap[dbname];
	
			try
			{
				dbi->connect(dbDesc.m_username, dbDesc.m_password, dbDesc.m_servname);
			}
			catch(CException e)
			{
				m_theErrorMessages->insert(DB_ERROR_CONNECT, e.GetString());
				return false;
			}
		}
		
		//(1.2)打开所有的内存数据库连接
		for (map<string, MmInterface*>::iterator iter=m_mmiMap.begin(); iter!=m_mmiMap.end(); ++iter)
		{
			string mmname = iter->first;
			MmInterface *mmi = iter->second;
			MmDesc mmDesc = m_mmDescMap[mmname];
	
			try
			{
				mmi->connect(mmDesc.m_dbtype, mmDesc.m_dbname, mmDesc.m_hostname, mmDesc.m_port, mmDesc.m_username, mmDesc.m_password);
			}
			catch(CException e)
			{
				m_theErrorMessages->insert(MM_ERROR_CONNECT, e.GetString());
				return false;
			}
		}
		//(1.3)连接默认数据库
		try
		{
			m_sysdbi.connect(m_sysDbDesc.m_username, m_sysDbDesc.m_password, m_sysDbDesc.m_servname);
		}
		catch (CException e)
		{
			m_theErrorMessages->insert(DB_ERROR_CONNECT, e.GetString());
			return false;
		}
	  // (1.4)增加打开所有的crm库连接  xueyf@lianchuang.com
	  for (map<string, DbInterface*>::iterator iterc=m_dbcrmMap.begin(); iterc!=m_dbcrmMap.end(); ++iterc)
		{
			string dbcname = iterc->first;
			DbInterface *dbic = iterc->second;
			DbDesc dbcrmDesc = m_dbcrmDescMap[dbcname];
	
			try
			{
				dbic->connect(dbcrmDesc.m_username, dbcrmDesc.m_password, dbcrmDesc.m_servname);
			}
			catch(CException e)
			{
				m_theErrorMessages->insert(DB_ERROR_CONNECT, e.GetString());
				return false;
			}
		}			
		//end
		++m_tag;
	}
	
	//(2)获取当前时间
	getcurrtime();
  char   sql[2000] = "\0";
  string strSql  = "";
  //增量和ACT->MDB有效 wangfc@lianchuang.com 2010-4-9 15:11:48
  if ( m_crmnum == 0 )
  {
  	//(3)全量(MDB ACT)参数导入处理
  	//(3.1)获取装载参数
  	vector<TableLoadRule> tableLoadRuleVector;
  	tableLoadRuleVector.clear();
  	sprintf(sql,"SELECT LOAD_TYPE,SRC_DB_NAME,SRC_SEL_SQL,OBJ_MM_NAMES,TABLE_NAME,OBJ_UPDATE_SQL,TRIGGER_METHOD,TRIGGER_TAG,INTERVAL_TIME,UPDATE_TIME,LOADRULE_ID FROM TD_B_TABLE_LOADRULE WHERE CHANNEL_NO=%d ORDER BY LOADRULE_ID",m_channelNo);
  	strSql = sql;
  	try
  	{
  		m_sysdbi.execStaticSql(strSql);
  		while(m_sysdbi.getNext())
  		{
  			//进行资料装载
  			TableLoadRule tableLoadRule;
  			
  			tableLoadRule.m_loadtype = (m_sysdbi.getColumnStringValue(1))[0];
  			tableLoadRule.m_srcdbname = m_sysdbi.getColumnStringValue(2);
  			tableLoadRule.m_srcselsql = m_sysdbi.getColumnStringValue(3);
  			tableLoadRule.m_objmmnames = m_sysdbi.getColumnStringValue(4);
  			tableLoadRule.m_tablename = m_sysdbi.getColumnStringValue(5);
  			tableLoadRule.m_objupdatesql = m_sysdbi.getColumnStringValue(6);
  			tableLoadRule.m_triggermethod = (m_sysdbi.getColumnStringValue(7))[0];
  			tableLoadRule.m_triggertag = (m_sysdbi.getColumnStringValue(8))[0];
  			tableLoadRule.m_intervaltime = m_sysdbi.getColumnIntValue(9);
  			tableLoadRule.m_updatetime = m_sysdbi.getColumnDateValue(10);
  			tableLoadRule.m_addupdatetime = m_sysdbi.getColumnDateValue(10);
  			tableLoadRule.m_loadruleid = m_sysdbi.getColumnIntValue(11);
  			addTime(tableLoadRule.m_addupdatetime, tableLoadRule.m_intervaltime);
  			
  			tableLoadRuleVector.push_back(tableLoadRule);
  		}
  	}		
  	catch(CException e)
  	{
  		m_sysdbi.disconnect();
  		
  		m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  		return false;
  	}
  	
  	//(3.2)进行数据装载
  	for (vector<TableLoadRule>::iterator iterx=tableLoadRuleVector.begin(); iterx!=tableLoadRuleVector.end(); ++iterx)
  	{
  		if ((iterx->m_triggermethod=='1' && iterx->m_triggertag=='0') ||
  			(iterx->m_triggermethod=='2' && iterx->m_addupdatetime<m_currtime))
  		{
  			//获取目标内存数据库操作类指针集合
  			vector<string>			objmmnameVector;
  			StringUtil::split(iterx->m_objmmnames, ";", objmmnameVector, false);
  			
  			vector<MmInterface*>	mmiVector;
  			mmiVector.clear();
  			for(vector<string>::iterator iter=objmmnameVector.begin(); iter!=objmmnameVector.end(); ++iter)
  			{
  				//存在,
  				if (m_mmiMap.find(*iter) != m_mmiMap.end())
  				{
  					mmiVector.push_back(m_mmiMap[*iter]);
  				}
  				else
  				{
  					char tmp[200];
  					sprintf(tmp,"内存数据库%s没有配置,请核查!",(*iter).c_str());
  					m_theErrorMessages->insert(MM_ERROR_NOCFG, tmp);
  					return false;
  				}
  			}
  			
  			try
  			{
  				m_paramProcess.init(m_dbiMap[iterx->m_srcdbname], &mmiVector);
  				// writeStatus("全量加载内存库.....",PROC_RUN);
  				m_paramProcess.process(iterx->m_loadtype, iterx->m_srcselsql, iterx->m_tablename,
  							   		   iterx->m_objupdatesql, iterx->m_updatetime, m_currtime);
         // writeStatus("全量加载内存库完毕!",PROC_RUN);  							   		   
  			}
  			catch(CException e)
  			{
  				m_sysdbi.disconnect();
  				
  				m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  				return false;
  			}
  			
  			//更新装载标志为成功装载
  			m_sysdbi.beginTrans();
  			sprintf(sql, "UPDATE TD_B_TABLE_LOADRULE SET TRIGGER_TAG='1',UPDATE_TIME=to_date('%s','yyyymmddhh24miss') WHERE LOADRULE_ID=%d AND CHANNEL_NO=%d", 
  					m_currtime.c_str(),iterx->m_loadruleid, m_channelNo);
  			strSql = sql;
  			try
  			{
  				m_sysdbi.execStaticSql(strSql);
  				m_sysdbi.commitTrans();
  			}
  			catch(CException e)
  			{
  				m_sysdbi.rollbackTrans();
  				m_sysdbi.disconnect();
  				
  				m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
  				return false;
  			}
  		}
  	}
  }
  //(3.3)增加crm全量倒换处理 add by xueyf
  else 
  {  
    if ( m_actnum != 0 ) //无缝全量同步部分
    {
        // 获取配置信息
        vector<TabCrmLoadRule> tabCrmLoadRuleVector;
        tabCrmLoadRuleVector.clear();
        char csql[2000] = "\0";
      	sprintf(csql,"SELECT INFO_TYPE,SRC_DB_NAME,SRC_SEL_SQL,OBJ_ACT_NAMES,USE_TABLE,TABLE_NAME_AB,OBJ_UPDATE_SQL,BEGP_NAME,ENDP_NAME,TRIGGER_TAG ,LOADRULE_ID  FROM TD_B_TABLE_LOADCRMRULE WHERE CHANNEL_NO=%d ORDER BY LOADRULE_ID",m_channelNo);
      	string cstrSql = csql;
      	vector<string> tablename;      	
      	try
      	{
      	 	m_sysdbi.execStaticSql(cstrSql);
      	 	while(m_sysdbi.getNext())
      	 	{ 
      	 	  //读取参数
      	 		TabCrmLoadRule tabrule;
      	 		tabrule.m_infotype = m_sysdbi.getColumnIntValue(1);
      	 		tabrule.m_srcdbcrmname = m_sysdbi.getColumnStringValue(2);
      	 		tabrule.m_srcselsql = m_sysdbi.getColumnStringValue(3);
      	 		tabrule.m_objactnames = m_sysdbi.getColumnStringValue(4);
      	 		tabrule.m_use=(m_sysdbi.getColumnStringValue(5));
      	 		tablename.clear();
      	 		StringUtil::split(m_sysdbi.getColumnStringValue(6), ";", tablename, false);
      	 		for(vector<string>::iterator itert=tablename.begin();itert!=tablename.end();++itert)
            {
               if(*itert!=tabrule.m_use)
               {
                  tabrule.m_tablename=*itert;
                  break;
               }
      	 		}
            string temps=m_sysdbi.getColumnStringValue(7);
            string vtable;
            vtable="VTABLE";
            int pos;
            //进行变量替换
            while((pos=temps.find(vtable,0))!=-1)
            {
                temps.replace(pos,vtable.size(),tabrule.m_tablename);
                break;
            }
            tabrule.m_objupdatesql =temps;
      	 		tabrule.m_pgbegname=m_sysdbi.getColumnStringValue(8);
      	 		tabrule.m_pgendname=m_sysdbi.getColumnStringValue(9);
      	 		tabrule.m_triggertag = (m_sysdbi.getColumnStringValue(10))[0];
      	 		tabrule.m_loadruleid = m_sysdbi.getColumnIntValue(11);
      	 		tabCrmLoadRuleVector.push_back(tabrule);
      	 	}
      	}catch(CException e)
      	{
      	 	m_sysdbi.disconnect();
      	 	m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
      	 	return false;
      	}
      	//( 3.4 )进行数据装载
      	for(vector<TabCrmLoadRule>::iterator iterc=tabCrmLoadRuleVector.begin();iterc!=tabCrmLoadRuleVector.end();++iterc)
      	{
      		if(iterc->m_triggertag=='0')
      		{ 
      		   try
          	 		tabrule.m_pgendname=m_sysdbi.getColumnStringValue(9);
      	 		tabrule.m_triggertag = (m_sysdbi.getColumnStringValue(10))[0];
      	 		tabrule.m_loadruleid = m_sysdbi.getColumnIntValue(11);
      	 		tabCrmLoadRuleVector.push_back(tabrule);
      	 	}
      	}catch(CException e)
      	{
      	 	m_sysdbi.disconnect();
      	 	m_theErrorMessages->insert(DB_ERROR_EXECSQL, e.GetString());
      	 	return false;
      	}
      	//( 3.4 )杩涜