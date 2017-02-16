#include "CMonfeeInputFile.h"
#include "CMonfeeOutputFile.h"
#include "CMonfeeInternalData.h"
#include "CMonfeeBillMgr.h"
#include "CRecvMonfee.h"
#include "CDiscntMonfee.h"

#include "base/Directory.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "param/CMonfeeParamMgr.h"
#include "log4cpp/Logger.h"

#include "CMonfeeApp.h"

CMonfeeApp  g_application;
Application * const theApp = &g_application;
extern log4cpp::Logger *theLogger;

CMonfeeApp::CMonfeeApp()
{
    m_monfeeParamMgr = NULL;
}


CMonfeeApp::~CMonfeeApp()
{
    /*
    //Add by Malone 2009-11-1 18:23 start
    if (m_pMonfeeParam)
    {
        m_pMonfeeParam->end();
        delete m_pMonfeeParam;
        m_pMonfeeParam = 0;
    }
    //Add by Malone 2009-11-1 18:23 end
    */
}


bool CMonfeeApp::initialization()
{
    setoptstr("c:");
    
    // 调用父类的方法进行标准初始化
    if (!Application::initialization())
    {
        return false;
    }

    //置状态
    m_objectName = "程序初始化";
    m_status = PROC_READY;
    updateStatus();

    //将程序设置成后台执行
#ifdef _DEBUG_
    set_asdaemon(false);
    set_onlyone(false);
    set_runinbkg(false);
#else
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif

    /*
    //Add by Malone 2009-11-1 18:16 start
    //取配置文件参数(theApp-->m_monfeeConfig)
    if (!getSysParams())
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,"GetSysParams Error");
        
        m_objectName = "从系统配置文件中获取系统参数出错，请检查。";
        updateStatus();
        return false;
    }
    
    m_pMonfeeParam = new PM_Billing;
    if ( m_pMonfeeParam == NULL ) {
        
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
            "Param initialization Error" );
        return false;
    }
    m_pMonfeeParam->setDbNo(m_monfeeConfig.m_dbNo);
    //设置地市编码
    m_pMonfeeParam->setEparchyCode(m_monfeeConfig.m_eparchyCode);
    //init param
    
    m_pMonfeeParam->setDbConnInfo(m_monfeeConfig.m_userName,
                                 m_monfeeConfig.m_passWord,
                                 m_monfeeConfig.m_servName);
    m_pMonfeeParam->setHostName(m_monfeeConfig.m_hostName.c_str());
    m_pMonfeeParam->setAppName(m_name);
    m_pMonfeeParam->setUpdateInterval(m_monfeeConfig.m_interval);
    
    m_pMonfeeParam->setChannelNo(m_channelNo); 


    if (!(*m_pMonfeeParam).initialization())
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
                                   "Param Initialization Error");
        m_objectName = "从数据库和数据文件中获取系统参数出错，请检查。";
        updateStatus();
        
        return false;
    }
    //Add by Malone 2009-11-1 18:16 end
    */

    return true;
}


bool CMonfeeApp::beforeLoop()
{
    if ( !Application::beforeLoop() )
    {
        return false;
    }
    
    //-----------------------------------------------------------------------//
    // 增加自己的初始化代码
    //-----------------------------------------------------------------------//
    //取配置文件参数(theApp-->m_monfeeConfig)
    if (!getSysParams())
    {
        return false;
    }
 
    int rd = 0;
    m_monfeeParamMgr = new CMonfeeParamMgr();
    
    //设置用户资料参数管理的类基本信息
    (*m_monfeeParamMgr).setDbConnInfo(m_monfeeConfig.m_userName,
                                        m_monfeeConfig.m_passWord,
                                        m_monfeeConfig.m_servName);
    (*m_monfeeParamMgr).setHostName(m_monfeeConfig.m_hostName.c_str());
    (*m_monfeeParamMgr).setAppName(m_name);
    (*m_monfeeParamMgr).setUpdateInterval(m_monfeeConfig.m_interval);
    (*m_monfeeParamMgr).setChannelNo(m_channelNo);
    (*m_monfeeParamMgr).setDbNo(m_monfeeConfig.m_dbNo);
    //Add by Malone 2009-11-2 13:56 start
    (*m_monfeeParamMgr).setEparchyCode(m_monfeeConfig.m_eparchyCode);
    //Add by Malone 2009-11-2 13:56 end

    MdbInfo  t_mdbInfo;
	  t_mdbInfo.m_dbType = m_monfeeConfig.m_mdbType;
	  t_mdbInfo.m_hostName = m_monfeeConfig.m_mdbHostName;//"";
	  t_mdbInfo.m_port = m_monfeeConfig.m_mdbPort; //0;
	  t_mdbInfo.m_dbServer = m_monfeeConfig.m_mdbServer;//"testmdb";
	  t_mdbInfo.m_user = m_monfeeConfig.m_mdbUserName;
	  t_mdbInfo.m_passwd = m_monfeeConfig.m_mdbPassWord;

	    #ifdef _DEBUG_
	      cout<<"setMdbInfo("<<t_mdbInfo<<")...."<<endl;
	    #endif	  
	  if((*m_monfeeParamMgr).setMdbInfo(t_mdbInfo)==false)
	  {
	    #ifdef _DEBUG_
	      cout<<"setMdbInfo("<<t_mdbInfo<<") false!"
	          <<__FILE__<<__LINE__<<endl;
	    #endif
	    return false;
	  }

    //用户资料参数管理类初始化
    if( !(*m_monfeeParamMgr).initialization() )
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
                                   "UserParam Initialization Error" );
        return false;
    }

    //获取当前时间
    getDealDate();

    //根据当前时间获取帐期信息
    if ( !(*m_monfeeParamMgr).getcycPara(m_dealDate, m_cycPara) )
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
                                   "Param td_b_cycparam Error");
        return false;
    }

    //如果当前帐期已使用，错误
    if (m_cycPara.m_useTag == '1')
    {
          m_theErrorMessages->insert(ERROR_PARAM_DATABASE,
            "the account opening :" + m_dealDate );
          return false;
    }

    //设置当前计费帐期、当前文件帐期
    m_currentCycId = m_cycPara.m_cycleId;
    m_fileCycId    = m_currentCycId;
    #ifdef _DEBUG_
    cout<<"m_currentCycId:"<<m_currentCycId<<endl;
    #endif
    //获取数据库中的参数信息
    if (!getSysParamsFromDb())
    {
        return false;
    }

    //获取帐目信息
    //if ( !(*m_monfeeParamMgr).getItemInfo() )
    //{
    //    m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
    //                               "Param Item and ItemRelation Error");
    //    return false;
    //}

    //设置处理通道和读取文件排序方法
    setPathIn(m_monfeeConfig.m_inputFilePath.c_str());
    setFileSortMethod(m_monfeeConfig.m_sortFlag);

    //置状态
    m_objectName = "初始化帐单文件";
    m_status = PROC_READY;
    updateStatus();


    //初始化帐单类
    rd = m_userBillManager.initialize(&m_monfeeConfig,m_currentCycId);
    if (rd == 1)
    {
        //恢复帐单退出，需要输出全量帐单
        m_userBillManager.end();
        
        m_theErrorMessages->insert(ERROR_BILL_RECOVERY_OK ,
            "Bill Recovery Ok");
        m_objectName = "帐单恢复成功,请再运行此进程";
    }
    else if(rd == -1)
    {
        m_theErrorMessages->insert(ERROR_BILL_NO_LOG_FILE ,
            "No Bill Log File");
        m_objectName = "没有帐单文件";
    }
    else if (rd == -2)
    {
        m_theErrorMessages->insert(ERROR_BILL_FILE,
            "Bill File Error");
        m_objectName = "帐单文件错误";
    }
    else if (rd == -3)
    {
        m_theErrorMessages->insert(ERROR_BILL_INITIALIZE,
            "Bill Initialization Error");
        m_objectName = "帐单文件初始化错误";
    }
    else
    {
        return true;
    }
    
    updateStatus();

    return false;
}


bool CMonfeeApp::loopProcess()
{
    if(!monfeeRun())
    {
        m_stop = true;
    }

    return Application::loopProcess();
}


bool CMonfeeApp::recovery()
{
    Application::recovery();

    return true;
}


bool CMonfeeApp::end()
{
    /*
    //Add by Malone 2009-11-1 start
    if (m_pMonfeeParam)
    {
        m_pMonfeeParam->end();
        delete m_pMonfeeParam;
        m_pMonfeeParam = 0;
    }
    //Add by Malone 2009-11-1 end
    */
    
    //清空内存
    m_userBillManager.end();
    (*m_monfeeParamMgr).end();

    m_accountCheckInfoVector.clear();
    m_sysDiscountVector.clear();
    m_spRelaDiscntMap.clear();

    //析构参数管理器
    delete m_monfeeParamMgr;

    //置状态
    m_objectName = "程序退出";
    m_status = PROC_EXIT;
    updateStatus();

    return Application::end();
}


bool CMonfeeApp::getSysParams()
{
	string	name;
	string	value;
	string	C;
	string	L;
	string	t_channelNo;

	char cTmp[4];
	sprintf(cTmp,"%d", m_channelNo);
	t_channelNo = cTmp;
	memset(cTmp,0,sizeof(cTmp));

	L += SECTDELIM;
	L += m_name ;
	L += SECTDELIM;
	L += t_channelNo;

	C += SECTDELIM;
	C += m_name ;
	C += SECTDELIM;
	C += "commParam";

//	#define GET_PARAM(X,Y)  name = Y; if (!m_theSysParam->getValue(X, name, value)) { m_theErrorMessages->insert(ERROR_PARAM_FILE,X + SECTDELIM + name); return false; };

	#define GET_PARAM(X,Y)  name = Y;                                       \
		if (!m_theSysParam->getValue(X, name, value))                        \
		{                                                                    \
		   m_theErrorMessages->insert(EAPPFRM_SYSVAL,X + SECTDELIM + name); \
		   return false;                                                    \
		};
       
	GET_PARAM(C,"hostName");
	m_monfeeConfig.m_hostName = value;

	GET_PARAM(C,"timeInterval");
	m_monfeeConfig.m_interval = atoi(value.c_str());	
//add by yuxk for MT 
	GET_PARAM(C,"unitfee_M");
	m_monfeeConfig.m_unitfee_M = atoi(value.c_str());	
	GET_PARAM(C,"unitfee_T");
	m_monfeeConfig.m_unitfee_T = atoi(value.c_str());	
	GET_PARAM(C,"product_iphone");
	m_monfeeConfig.m_product_iphone = value;	
	GET_PARAM(L,"product_iphone");
  if (value!="") m_monfeeConfig.m_product_iphone = value;	
//add by yuxk for MT   
	GET_PARAM(L,"dbNo");
	m_monfeeConfig.m_dbNo = atoi(value.c_str());	
	
	// Added by Malone 2009-11-1 17:36 start
	GET_PARAM(L,"eparchyCode");
	m_monfeeConfig.m_eparchyCode = value;
  // Added by Malone 2009-11-1 17:36 end

	GET_PARAM(L,"userName");
	m_monfeeConfig.m_userName = value;

	GET_PARAM(L,"passWord");
	m_monfeeConfig.m_passWord = value;

	GET_PARAM(L,"servName");
	m_monfeeConfig.m_servName = value;
	
	GET_PARAM(L,"mdbType");
	m_monfeeConfig.m_mdbType = (T_DATABASETYPE)atoi(value.c_str());

	GET_PARAM(L,"mdbServer");
	m_monfeeConfig.m_mdbServer = value;

	GET_PARAM(L,"mdbHostName");
	m_monfeeConfig.m_mdbHostName = value;

	GET_PARAM(L,"mdbPort");
	m_monfeeConfig.m_mdbPort = atoi(value.c_str());

	GET_PARAM(L,"mdbUserName");
	m_monfeeConfig.m_mdbUserName = value;

	GET_PARAM(L,"mdbPassWord");
	m_monfeeConfig.m_mdbPassWord = value;
		
	GET_PARAM(L,"inputFilePath");
	m_monfeeConfig.m_inputFilePath = value;
	if (m_monfeeConfig.m_inputFilePath.substr(
            m_monfeeConfig.m_inputFilePath.length()-1) != "/")
	{
		m_monfeeConfig.m_inputFilePath =
            m_monfeeConfig.m_inputFilePath + "/";
	}

	GET_PARAM(L,"invalidFilePath");
	m_monfeeConfig.m_invalidFilePath = value;
	if (m_monfeeConfig.m_invalidFilePath.substr(
            m_monfeeConfig.m_invalidFilePath.length()-1) != "/")
	{
		m_monfeeConfig.m_invalidFilePath =
            m_monfeeConfig.m_invalidFilePath + "/";
	}

	GET_PARAM(L,"workFilePath");
	m_monfeeConfig.m_workFilePath = value;
	if (m_monfeeConfig.m_workFilePath.substr(
            m_monfeeConfig.m_workFilePath.length()-1) != "/")
	{
		m_monfeeConfig.m_workFilePath =
            m_monfeeConfig.m_workFilePath + "/";
	}
	
	GET_PARAM(L,"outputFilePath_D");
	m_monfeeConfig.m_outputFilePath_D = value;
	if (m_monfeeConfig.m_outputFilePath_D.substr(
            m_monfeeConfig.m_outputFilePath_D.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_D =
            m_monfeeConfig.m_outputFilePath_D + "/";
	}

	GET_PARAM(L,"outputFilePath_P");
	m_monfeeConfig.m_outputFilePath_P = value;
	if (m_monfeeConfig.m_outputFilePath_P.substr(
            m_monfeeConfig.m_outputFilePath_P.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_P =
            m_monfeeConfig.m_outputFilePath_P + "/";
	}

	GET_PARAM(L,"outputFilePath_I");
	m_monfeeConfig.m_outputFilePath_I = value;
	if (m_monfeeConfig.m_outputFilePath_I.substr(
            m_monfeeConfig.m_outputFilePath_I.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_I =
            m_monfeeConfig.m_outputFilePath_I + "/";
	}

	// Add by Malone 2007-7-19 10:01 start
	GET_PARAM(L,"outputBakupFilePath_P");
	m_monfeeConfig.m_outputBakupFilePath_P = value;
	if (m_monfeeConfig.m_outputBakupFilePath_P.substr(
            m_monfeeConfig.m_outputBakupFilePath_P.length()-1) != "/")
	{
		m_monfeeConfig.m_outputBakupFilePath_P =
            m_monfeeConfig.m_outputBakupFilePath_P + "/";
	}

	GET_PARAM(L,"outputBakupFilePath_I");
	m_monfeeConfig.m_outputBakupFilePath_I = value;
	if (m_monfeeConfig.m_outputBakupFilePath_I.substr(
            m_monfeeConfig.m_outputBakupFilePath_I.length()-1) != "/")
	{
		m_monfeeConfig.m_outputBakupFilePath_I =
            m_monfeeConfig.m_outputBakupFilePath_I + "/";
	}
	// Add by Malone 2007-7-19 10:01 end

	GET_PARAM(L,"outputFilePath_B");
	m_monfeeConfig.m_outputFilePath_B = value;
	if (m_monfeeConfig.m_outputFilePath_B.substr(
            m_monfeeConfig.m_outputFilePath_B.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_B =
            m_monfeeConfig.m_outputFilePath_B + "/";
	}
	
	GET_PARAM(L,"outputFilePath_C");
	m_monfeeConfig.m_outputFilePath_C = value;
	if (m_monfeeConfig.m_outputFilePath_C.substr(
            m_monfeeConfig.m_outputFilePath_C.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_C =
            m_monfeeConfig.m_outputFilePath_C + "/";
	}
	
	GET_PARAM(L,"outputFilePath_N");
	m_monfeeConfig.m_outputFilePath_N = value;
	if (m_monfeeConfig.m_outputFilePath_N.substr(
            m_monfeeConfig.m_outputFilePath_N.length()-1) != "/")
	{
		m_monfeeConfig.m_outputFilePath_N =
            m_monfeeConfig.m_outputFilePath_N + "/";
	}
				
	GET_PARAM(L,"totalBillFilePath");
	m_monfeeConfig.m_totalBillFilePath = value;
	if (m_monfeeConfig.m_totalBillFilePath.substr(
            m_monfeeConfig.m_totalBillFilePath.length()-1) != "/")
	{
		m_monfeeConfig.m_totalBillFilePath =
            m_monfeeConfig.m_totalBillFilePath + "/";
	}

	GET_PARAM(L,"billLogFilePath");
	m_monfeeConfig.m_billLogFilePath = value;
	if (m_monfeeConfig.m_billLogFilePath.substr(
            m_monfeeConfig.m_billLogFilePath.length()-1) != "/")
	{
		m_monfeeConfig.m_billLogFilePath =
            m_monfeeConfig.m_billLogFilePath + "/";
	}

	GET_PARAM(L,"autoCommitBillUserNum");
	m_monfeeConfig.m_autoCommitBillUserNum = atoi(value.c_str());

	GET_PARAM(L,"autoOutputBillUserNum");
	m_monfeeConfig.m_autoOutputBillUserNum = atoi(value.c_str());

	GET_PARAM(L,"sleepTime");
	m_monfeeConfig.m_sleepTime = atoi(value.c_str());

    m_monfeeConfig.m_channelNo = m_channelNo;
    m_monfeeConfig.m_sortFlag = Directory::SF_MODIFYTIME;

	return true;
}


bool CMonfeeApp::getSysParamsFromDb()
{
    //获取当前的省份编码
    (*m_monfeeParamMgr).getLocalProvinceCode(m_provinceCode);

    (*m_monfeeParamMgr).getCycleTime(m_currentCycId, m_cycPara);   
                                                
    (*m_monfeeParamMgr).getAllRelaDiscount(m_spRelaDiscntMap); 
    
    return true;
}


bool CMonfeeApp::shiftNewCyc()
{
    CycParam    cycPara;
    string      fileInName  = "";
    string      fileOutName = "" ;

    //文件帐期和当前帐期相同，跳出返回true
    if (m_fileCycId == m_currentCycId)
    {
        return true;
    }

    //如果文件帐期未获取到相应帐期信息，返回错误信息
    if ( !(*m_monfeeParamMgr).getCycleTime(m_fileCycId, cycPara) )
    {
        m_theErrorMessages->insert(ERROR_PARAM_DATABASE,
                                   "--get cyc param info failed!");
        return false;
    }

    //如果文件帐期对应的相应帐期标志为已使用，删除文件并返回错误信息
    if (cycPara.m_useTag == '1')
    {
        fileInName = m_monfeeConfig.m_inputFilePath + m_fileName;
        fileOutName = m_monfeeConfig.m_invalidFilePath + m_fileName;
        if (rename(fileInName.c_str(), fileOutName.c_str()) < 0)
        {
            m_theErrorMessages->insert(ERROR_LINK_UNLINKE,
                fileInName + " ->" + fileOutName + "--file rename error");
            return false;
        }

        m_theErrorMessages->insert(ERROR_INVALID_FILE,
                                "cycid from filename error" + m_fileName);
        return false;
    }

    //对帐单内存切换帐期
    if (!shiftMemoryBillForNewCyc(m_fileCycId))
    {
        return false;
    }

    //当前计费帐期和帐期信息重新赋值
    m_currentCycId = cycPara.m_cycleId;
    m_cycPara = cycPara;

    //更新进程处理信息
    m_objectName = "帐期切换完成";
    updateStatus();

    return true;
}


bool CMonfeeApp::shiftMemoryBillForNewCyc(const int& cycId)
{
    int rd = 0;

    //传入帐期标识和当前帐期标识不相等，内存帐期进行切换
    if (cycId != m_currentCycId)
    {
        //更新进程处理状态
        m_objectName = "切换帐单文件";
        m_status = PROC_RUN;
        updateStatus();

        //内存帐单类终止，将全量内存帐单输出到全量帐单文件中，并清理内存
        if (!m_userBillManager.end())
        {
            m_theErrorMessages->insert(ERROR_BILL_END,
                                "--write totalBill to billfile failed!");
            return false;
        }

        //内存帐单类初始化
        rd = m_userBillManager.initialize(&m_monfeeConfig, cycId);
        if (rd == 1)
        {
            m_theErrorMessages->insert(ERROR_BILL_RECOVERY_OK ,
                                       "Bill Recovery Ok");
            m_objectName = "帐单恢复成功,请再运行此进程";
            return false;
        }
        else if (rd == -1)
        {
            m_theErrorMessages->insert(ERROR_BILL_NO_LOG_FILE ,
                                       "No Bill Log File");
            m_objectName = "日志文件出错";
            return false;
        }
        else if (rd == -2)
        {
            m_theErrorMessages->insert(ERROR_BILL_FILE,
                                       "Bill File Error");
            m_objectName = "帐单文件错误";
            return false;
        }
        else if (rd == -3)
        {
            m_theErrorMessages->insert(ERROR_BILL_INITIALIZE,
                                       "Bill Initialization Error");
            m_objectName = "帐单文件初始化错误";
            return false;
        }
    }

    return true;
}


void CMonfeeApp::getDealDate()
{
    time_t now;
    char tmpDate[14+1];
    struct tm *w;
    memset(tmpDate,0,sizeof(tmpDate));

    time(&now);
    w = localtime(&now);
    sprintf(tmpDate,"%04d%02d%02d%02d%02d%02d",
            w->tm_year + 1900,w->tm_mon + 1,
            w->tm_mday,w->tm_hour,
            w->tm_min, w->tm_sec);

    m_dealDate = tmpDate;
		
//		m_dealDate = "20090224000001";
		
    memset(tmpDate,0,sizeof(tmpDate));
}


bool CMonfeeApp::getValidFile()
{
	char fileName[80];

	memset(fileName,0,sizeof(fileName));

	if (getFile(fileName,80) > 0)
	{
        m_fileName = fileName;
        m_monfeeConfig.m_fileName = m_fileName;

        if (checkFileType() && checkFileName())
        {
            m_fileType = m_fileName[0];
            m_fileCycId = atoi(m_fileName.substr(1,6).c_str());
						m_monfeeConfig.m_cdrFileName = m_fileName.substr(1,9) +
			                                        m_fileName.substr(16,12); 

            memset(fileName,0,sizeof(fileName));
            return true;
        }
	}

    memset(fileName,0,sizeof(fileName));
	return false;
}


bool CMonfeeApp::checkFileType()
{
    //文件为正在处理的文件
    if (m_fileName[0]=='%' || m_fileName[0]=='#' || m_fileName[0]=='_')
    {
        return false;
    }

    //文件类型为U或者F文件
    if (m_fileName[0]==FILE_TYPE_IN_A || m_fileName[0]==FILE_TYPE_IN_B ||
        m_fileName[0]==FILE_TYPE_IN_M || m_fileName[0]==FILE_TYPE_IN_H)
    {
        return true;
    }
    //其他文件
    else
    {
        string fileInName  = "";
        string fileOutName = "";
        fileInName = m_monfeeConfig.m_inputFilePath + m_fileName;
        fileOutName = m_monfeeConfig.m_invalidFilePath + m_fileName;
        if (rename(fileInName.c_str(), fileOutName.c_str()) < 0)
        {
            m_theErrorMessages->insert(ERROR_LINK_UNLINKE,
                fileInName + " ->" + fileOutName + "--file rename error");
            return false;
        }

        m_theErrorMessages->insert(ERROR_INVALID_FILE,
                                   m_fileName + "--Invalid File Name");
        return false;
    }
}


bool CMonfeeApp::checkFileName()
{
    bool    fileNameIsValid = true;
    string  year = "";
    string  month = "";
    string  day = "";
    string  hour = "";
    string  minute = "";
    string  second = "";

    //文件名称长度检查
    if (m_fileName.length() != 28)
    {
        fileNameIsValid = false;
    }
    //文件名称内容检查
    else
    {
        for (int i=1;i<m_fileName.length();i++)
        {
            if (m_fileName[i] >'9' || m_fileName[i] <'0')
            {
                fileNameIsValid = false;
                break;
            }
        }
        //<A|B|M|H><YYYYMM><XXX><YYMMDD><YYMMDDHHMISS>
        //<YYYYMM>
        year = m_fileName.substr(1,4);
        if (year > "2099" || year < "2000")
        {
            fileNameIsValid = false;
        }
        month = m_fileName.substr(5,2);
        if (month > "12" || month < "01")
        {
            fileNameIsValid = false;
        }

        //<YYMMDD>
        year = m_fileName.substr(10,2);
        if (year > "99" || year < "00")
        {
            fileNameIsValid = false;
        }
        month = m_fileName.substr(12,2);
        if (month > "12" || month < "01")
        {
            fileNameIsValid = false;
        }
        day = m_fileName.substr(14,2);
        if (month > "31" || month < "01")
        {
            fileNameIsValid = false;
        }

        //<YYMMDDHHMISS>
        year = m_fileName.substr(16,2);
        if (year > "99" || year < "00")
        {
            fileNameIsValid = false;
        }
        month = m_fileName.substr(18,2);
        if (month > "12" || month < "01")
        {
            fileNameIsValid = false;
        }
        day = m_fileName.substr(20,2);
        if (month > "31" || month < "01")
        {
            fileNameIsValid = false;
        }
        hour = m_fileName.substr(22,2);
        if (hour > "23" || hour < "00")
        {
            fileNameIsValid = false;
        }
        minute = m_fileName.substr(24,2);
        if (minute > "59" || minute < "00")
        {
            fileNameIsValid = false;
        }
        second = m_fileName.substr(26,2);
        if (second > "59" || second < "00")
        {
            fileNameIsValid = false;
        }
    }

    if (!fileNameIsValid)
    {
        string fileInName  = "";
        string fileOutName = "";
        fileInName = m_monfeeConfig.m_inputFilePath + m_fileName;
        fileOutName = m_monfeeConfig.m_invalidFilePath + m_fileName;
        if (rename(fileInName.c_str(), fileOutName.c_str()) < 0)
        {
            m_theErrorMessages->insert(ERROR_LINK_UNLINKE,
                fileInName + " ->" + fileOutName + "--file rename error");
        }

        m_theErrorMessages->insert(ERROR_INVALID_FILE,
                                   m_fileName + "--Invalid File Name");
    }

    return fileNameIsValid;
}


void CMonfeeApp::addupAccountInfo()
{
    m_monfeeInternalData.addupInfo(m_accountAddupInfo);
}


void CMonfeeApp::checkAddupInfo()
{
    vector<SysParamInfo>            sysParamInfoVector;
    vector<SysParamInfo>::iterator  iter;

    int                             minValue = 0;
    int                             maxValue = 0;
    AccountCheckInfo                accountCheckInfo;

    m_errorTag = '0';

    //1.错误用户占总用户数比例进行核查
    if ((*m_monfeeParamMgr).getSysParamInfo("AS_ASP_MonthFeeErrorIdRatio",
                                            sysParamInfoVector))
    {
        for (iter=sysParamInfoVector.begin();
             iter!=sysParamInfoVector.end(); iter++)
        {
            //对用户数达到一定数后，错误用户占总用户数比例进行核查
            if ((m_accountAddupInfo.m_idNum>=iter->m_preValueN2) &&
                (m_accountAddupInfo.m_idNum<iter->m_preValueN3))
            {
                minValue = m_accountAddupInfo.m_idNum*0;
                maxValue = m_accountAddupInfo.m_idNum*(1.0*iter->m_valueN/100);

                accountCheckInfo.m_checkContentCode = "14";
                accountCheckInfo.m_itemCode = 0;
                accountCheckInfo.m_factValue = m_accountAddupInfo.m_errorIdNum;
                accountCheckInfo.m_minValue = minValue;
                accountCheckInfo.m_maxValue = maxValue;

                if ((accountCheckInfo.m_factValue>maxValue) ||
                    (accountCheckInfo.m_factValue<minValue))
                {
                    m_errorTag = '1';
                }

                m_accountCheckInfoVector.push_back(accountCheckInfo);
                break;
            }
        }
    }

    //2.错误记录占总记录数比例进行核查
    if ((*m_monfeeParamMgr).getSysParamInfo("AS_ASP_MonthFeeErrorRecordRatio",
                                            sysParamInfoVector))
    {
        for (iter=sysParamInfoVector.begin();
             iter!=sysParamInfoVector.end(); iter++)
        {
            //对记录数达到一定数后，错误记录占总记录数比例进行核查
            if ((m_accountAddupInfo.m_recordNum>=iter->m_preValueN2) &&
                (m_accountAddupInfo.m_recordNum<iter->m_preValueN3))
            {
                minValue = m_accountAddupInfo.m_recordNum*0;
                maxValue = m_accountAddupInfo.m_recordNum*(1.0*iter->m_valueN/100);

                accountCheckInfo.m_checkContentCode = "15";
                accountCheckInfo.m_itemCode = 0;
                accountCheckInfo.m_factValue =
                                            m_accountAddupInfo.m_errorRecordNum;
                accountCheckInfo.m_minValue = minValue;
                accountCheckInfo.m_maxValue = maxValue;

                if ((accountCheckInfo.m_factValue>maxValue) ||
                    (accountCheckInfo.m_factValue<minValue))
                {
                    m_errorTag = '1';
                }

                m_accountCheckInfoVector.push_back(accountCheckInfo);
                break;
            }
        }
    }

    //3.对用户数达到一定数后，总优惠金额占总费用比例进行核查
    if ((*m_monfeeParamMgr).getSysParamInfo("AS_ASP_MonthFeeDiscountFeeRatio",
                                            sysParamInfoVector))
    {
        for (iter=sysParamInfoVector.begin();
             iter!=sysParamInfoVector.end(); iter++)
        {
            //对用户数达到一定数后，总优惠金额占总费用比例进行核查
            if ((m_accountAddupInfo.m_idNum>=iter->m_preValueN2) &&
                (m_accountAddupInfo.m_idNum<iter->m_preValueN3))
            {
                minValue = m_accountAddupInfo.m_feeSum*(1.0*iter->m_valueN/100);
                maxValue = m_accountAddupInfo.m_feeSum*(1.0*iter->m_preValueN1/100);

                accountCheckInfo.m_checkContentCode = "16";
                accountCheckInfo.m_itemCode = 0;
                accountCheckInfo.m_factValue = m_accountAddupInfo.m_afeeSum +
                                               m_accountAddupInfo.m_bfeeSum;
                accountCheckInfo.m_minValue = minValue;
                accountCheckInfo.m_maxValue = maxValue;

                if (accountCheckInfo.m_factValue > maxValue ||
                    accountCheckInfo.m_factValue < minValue)
                {
                    m_errorTag = '1';
                }

                m_accountCheckInfoVector.push_back(accountCheckInfo);
                break;
            }
        }
    }

    if (!(*m_monfeeParamMgr).getSysParamInfo("AS_ASP_MonthFeeSupportErrorDeal",
                                              sysParamInfoVector))
    {
        m_errorTag = '0';
    }
}


bool CMonfeeApp::monfeeRun()
{
    //判断用户出帐资料参数是否需要更新
    if ((*m_monfeeParamMgr).needUpdate())
    {
        //更新参数错误
        if (!(*m_monfeeParamMgr).update())
        {
            m_theErrorMessages->insert(ERROR_PARAM_UPDATING,
                                       "UserParam Update Error" );
            sleep(m_monfeeConfig.m_sleepTime);
            return true;
        }
        
        //获取数据库中的系统参数
        if (!getSysParamsFromDb())
        {
            return false;
        }        
    }

    //获取有效文件
    if (getValidFile())
    {
    	  //对于梦网文件，由于资费参数和处罚规则是是实时变化的，
    	  //每次计算都要重新加载参数
    	  if(m_fileType == 'M')
        {
            //更新参数错误
            if (!(*m_monfeeParamMgr).update())
            {
                m_theErrorMessages->insert(ERROR_PARAM_UPDATING,
                                       "UserParam Update Error 1" );
            		sleep(m_monfeeConfig.m_sleepTime);
                return true;
            }
        }

        //日志开始
        m_performId = theLogger->pBegin();

        //如果文件所属计费帐期不等同于当前计费帐期
        if (m_fileCycId != m_currentCycId)
        {
            //进行帐期切换
            if (!shiftNewCyc())
            {
                return false;
            }
        }

        //获取当前处理时间并创建流水号
        getDealDate();
cout<<"getDealDate"<<endl;        
        //获取系统级优惠
        (*m_monfeeParamMgr).getSysDiscount("7",m_cycPara.m_beginTime,
								                       m_cycPara.m_endTime,
								                       m_sysDiscountVector);     

        m_errorTag = '0';

        //初始化日志信息、累计信息和核查信息
        m_accountAddupInfo.m_idNum = 0;
        m_accountAddupInfo.m_errorIdNum = 0;
        m_accountAddupInfo.m_oweIdNum  = 0;
        m_accountAddupInfo.m_recordNum = 0;
        m_accountAddupInfo.m_errorRecordNum = 0;
        m_accountAddupInfo.m_feeSum = 0;
        m_accountAddupInfo.m_afeeSum = 0;
        m_accountAddupInfo.m_bfeeSum = 0;
        m_accountAddupInfo.m_oweFeeSum = 0;
        m_accountCheckInfoVector.clear();

        //内存帐单比较处理日期，看是否需要切换日期
        if (!m_userBillManager.fileInit(m_dealDate.c_str()))
        {
            m_theErrorMessages->insert(ERROR_BILL_INITIALIZE, m_dealDate +
                                       "--memory bill shiftday fail!");
            return false;
        }        
        
        //用户出帐输出文件类初始化
        m_monfeeOutputFile.initialize(m_performId, &m_monfeeConfig);

        //用户出帐输入文件类初始化
        m_monfeeInputFile.initialize(&m_monfeeConfig,
                                          m_performId,
                                          &m_accountAddupInfo);

        //内部数据类初始化
        if(! m_monfeeInternalData.initialize(m_cycPara, 
                                      m_provinceCode, 
                                      m_performId,
                                      m_fileType,
                                      m_monfeeParamMgr,
                                      m_dealDate,
                                      m_sysDiscountVector,
                                      &m_spRelaDiscntMap,
                                      &m_monfeeConfig))//modify by yuxk for MT
        {
            m_theErrorMessages->insert(ERROR_BILL_INITIALIZE, m_dealDate +
                                       "--Internal Data  initialize fail!");
            return false;
        }                                        

        
        //打开输入文件
        if (!m_monfeeInputFile.openFile())
        {
            return false;
        }
cout<<"m_fileName:"<<m_fileName<<endl;        
        m_objectName = "处理文件:" + m_fileName;
        m_status = PROC_RUN;
        updateStatus();

        USERID                      userId = INVALID_USERID;
        int                         outputUserNum = 0;
        int                         userNum = 0;
        vector<UserSubInfo>    userSubInfoVector;

        userSubInfoVector.clear();
cout<<"AAAAAAAAAAAAAAAAAAAAAAA"<<endl;
        //循环处理用户
        while (m_monfeeInputFile.getUserSubInfo(userId, userSubInfoVector))
        {


            //如果是无效记录
            if (userId == INVALID_USERID)
            {
                continue;
            }

            //用户内部数据类初始化
 return false;
        }                                        

        
        //鎵撳紑杈撳叆鏂囦欢
        if (!m_monfeeInputFile.openFile())
        {
            return false;
        }
cout<<"m_fileName:"<<m_fileName<<endl;        
        m_objectName = "澶勭悊鏂囦欢:" + m_fileName;
        m_status = PROC_RUN;
        updateStatus();

        USERID                      userId = INVALID_USERID;
        int                         outputUserNum = 0;
        int                         userNum = 0;
        vector<UserSubInfo>    userSubInfoVector;

        userSubInfoVector.clear();
cout<<"AAAAAAAAAAAAAAAAAAAAAAA"<<endl;
        //寰