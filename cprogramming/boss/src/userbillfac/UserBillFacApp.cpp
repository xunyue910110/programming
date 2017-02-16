#include "UserBillFacApp.h"
#include "appfrm/ErrorHandle.h"
#include "base/SysParam.h"
#include "log4cpp/Logger.h"
#include <algorithm>

UserBillFacApp  g_application;
Application * const theApp = &g_application;

extern log4cpp::Logger *theLogger;

UserBillFacApp::UserBillFacApp()
{
}


UserBillFacApp::~UserBillFacApp()
{
}


bool UserBillFacApp::initialization()
{
	setoptstr("c:");

	//置状态
	m_objectName = "程序初始化";
	m_status = PROC_READY;
	updateStatus();

	// normally, call standard initialization first
	if (!Application::initialization())
	{
		return false;
	}

	//set application run method if need, all default is false
    #ifdef _DEBUG_
	set_asdaemon(false);
	set_onlyone(false);
	set_runinbkg(false);
    #else
	set_asdaemon(true);
	set_onlyone(true);
	set_runinbkg(true);
    #endif

    if (!getSysParams())
	{
		m_theErrorMessages->insert
		        (ERROR_PARAM_INITIALIZE, "SysParams Get Error");
		return false;
	}

	//初始化目录类
	m_directory.setPath(m_billFacConfig.m_inputFilePath.c_str());

	//清空用户帐单内存
	m_userBillMap.clear();

	//日志初始化
  	m_accountLogInfo.m_idNum = 0;
	m_accountLogInfo.m_errorIdNum = 0;
	m_accountLogInfo.m_recordNum = 0;
	m_accountLogInfo.m_errorRecordNum = 0;
	m_accountLogInfo.m_fee = 0;
	m_accountLogInfo.m_filterFee = 0;

	return true;
}


bool UserBillFacApp::loopProcess()
{
	if (!userBillFacRun())
	{
		m_stop = true;
	}

	return Application::loopProcess();
}


bool UserBillFacApp::recovery()
{
	Application::recovery();
	return true;
}


bool UserBillFacApp::end()
{
    return Application::end();
}


bool UserBillFacApp::getSysParams()
{
	vector<string>   vecstr;
	char strtmp[100], strtmp2[4];
	memset(strtmp, 0, sizeof(strtmp));
	memset(strtmp2, 0, sizeof(strtmp2));
	int i,j,iNum1,iNum2;
	string strFileType="";

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

	#ifdef _DEBUG_
	cout << "C==" << C << endl;
    #endif

	#define GET_PARAM(X,Y)  name = Y; if (!m_theSysParam->getValue(X, name, value)) { m_theErrorMessages->insert(ERROR_PARAM_FILE,X + SECTDELIM + name); return false; };

	//输入文件名称是否作详细判断
	GET_PARAM(C,"IsNeedJudgeNameDetail");
	m_billFacConfig.m_isNeedJudgeNameDetail = (bool)atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "输入文件名称是否作详细判断:" <<
	    (m_billFacConfig.m_isNeedJudgeNameDetail?"是":"否") << endl;
	#endif

	//获取最多处理文件个数
	GET_PARAM(C,"dealFileNum");
	m_billFacConfig.m_dealFileNum = atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "获取最多处理文件个数:" << m_billFacConfig.m_dealFileNum <<endl;
	#endif

	//获取通道个数
	GET_PARAM(C,"channelNum");
	m_billFacConfig.m_channelNum = atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "通道个数:" << m_billFacConfig.m_channelNum << endl;
	#endif

	//add by zhouq begin
	GET_PARAM(C,"recordNum");
	m_billFacConfig.m_RecordNum = atol(value.c_str());
	if (m_billFacConfig.m_RecordNum < 1000) m_billFacConfig.m_RecordNum = 200000;
	#ifdef _DEBUG_
	cout << "文件最大记录数:" << m_billFacConfig.m_RecordNum << endl;
	#endif
	//add by zhouq end

	//获取程序空闲时休眠秒数
	GET_PARAM(C,"sleepTime");
	m_billFacConfig.m_sleepTime = atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "程序空闲时休眠秒数:" << m_billFacConfig.m_sleepTime << endl;
	#endif

	//获取是否备份合法文件
	GET_PARAM(C,"IsNeedBackupValidFile");
	m_billFacConfig.m_isNeedBackupValidFile = (bool)atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "是否备份合法文件:"
	     << (m_billFacConfig.m_isNeedBackupValidFile?"是":"否") << endl;
	#endif

	//获取是否备份非法文件
	GET_PARAM(C,"IsNeedBackupInvalidFile");
	m_billFacConfig.m_isNeedBackupInvalidFile =  (bool)atoi(value.c_str());
	#ifdef _DEBUG_
	cout << "是否备份非法文件:"
	     << (m_billFacConfig.m_isNeedBackupInvalidFile?"是":"否") << endl;
	#endif

	//获取本地文件分别是什么文件头
	GET_PARAM(C, "localFileType");
	//如果这种类型的文件来源有多种账单文件
	if (value.find(',')!=string::npos)
	{
		//破开以逗号或其他分隔符隔开的
		iNum1 = split(value, ",\t ", vecstr);
		for (j=0; j<=iNum1-1; j++)
		{
			m_mapLocalOrRemote.insert(
			     map<char, char>::value_type(vecstr[j][0], '0'));
			#ifdef _DEBUG_
			cout << vecstr[j][0] << " ";
		    #endif
		}
		#ifdef _DEBUG_
		cout << endl;
		#endif
	}
	else
	{
	    m_mapLocalOrRemote.insert(map<char, char>::value_type(value[0], '0'));
	    #ifdef _DEBUG_
	    cout << value[0] << endl;
	    #endif
	}

	//获取外地文件分别是什么文件头
	GET_PARAM(C, "remoteFileType");
	//如果这种类型的文件来源有多种账单文件
	if (value.find(',')!=string::npos)
	{
		//破开以逗号或其他分隔符隔开的
		iNum1 = split(value, ",\t ", vecstr);
		for (j=0; j<=iNum1-1; j++)
		{
			m_mapLocalOrRemote.insert(
			   map<char, char>::value_type(vecstr[j][0], '1'));
			#ifdef _DEBUG_
			cout << vecstr[j][0] << " ";
			#endif
		}
		#ifdef _DEBUG_
		cout << endl;
		#endif
	}
	else
	{
	    m_mapLocalOrRemote.insert(map<char, char>::value_type(value[0], '1'));
	    #ifdef _DEBUG_
	    cout << value[0] << endl;
	    #endif
	}

	memset(strtmp, 0, sizeof(strtmp));
	FileNameRule fileNameRule;
	vector<FileNameRule> fileNameRuleVec;

	//获取输入文件命名规则
	GET_PARAM(C+"\\fileNameRule", "fileTypeNum");
	iNum1 = atoi(value.c_str());
	for (i = 1; i <= iNum1; i++)
	{
		sprintf(strtmp, "filetype%d",i);
		#ifdef _DEBUG_
		cout << strtmp << endl;
		cout << C+"\\fileNameRule\\"+strtmp << endl;
		#endif

		GET_PARAM(C+"\\fileNameRule\\"+strtmp, "fileType");
		strFileType = value;
		#ifdef _DEBUG_
		cout << "strFileType==" << strFileType << endl;
		#endif

		GET_PARAM(C+"\\fileNameRule\\"+strtmp, "ruleNum");
		iNum2 = atoi(value.c_str());
		#ifdef _DEBUG_
		cout << "ruleNum==" << iNum2 << endl;
		#endif

		for (j = 1; j<= iNum2; j++)
		{
            sprintf(strtmp2, "%d",j);
            #ifdef _DEBUG_
            cout << C+"\\fileNameRule\\"+strtmp+"\\"+strtmp2 << endl;
            #endif

            GET_PARAM(C+"\\fileNameRule\\"+strtmp+"\\"+strtmp2, "firstPos");
            fileNameRule.m_firstPos = atoi(value.c_str());
            GET_PARAM(C+"\\fileNameRule\\"+strtmp+"\\"+strtmp2, "len");
            fileNameRule.m_len = atoi(value.c_str());
            GET_PARAM(C+"\\fileNameRule\\"+strtmp+"\\"+strtmp2, "maxValue");
            fileNameRule.m_MaxValue = value;
            GET_PARAM(C+"\\fileNameRule\\"+strtmp+"\\"+strtmp2, "minValue");
            fileNameRule.m_MinValue = value;
            #ifdef _DEBUG_
            cout << fileNameRule.m_firstPos << " "
                 << fileNameRule.m_len << " "
                 << fileNameRule.m_MaxValue << " "
                 << fileNameRule.m_MinValue << " "
                 << endl;
            #endif
            fileNameRuleVec.push_back(fileNameRule);
		}
		m_mmapFileNameRule.insert(FILENAMERULEMMAP::value_type(
		  strFileType[0], make_pair((int)strFileType.size(), fileNameRuleVec)));
	    memset(strtmp, 0, sizeof(strtmp));
	    memset(strtmp2, 0, sizeof(strtmp2));
	    fileNameRuleVec.clear();
	}

	//直接从数据库获取用户统一付费账目与账户号码对应关系
	GET_PARAM(L, "userName");
	m_userName = value;

	GET_PARAM(L, "passWord");
	m_passWord = value;

	GET_PARAM(L, "servName");
	m_servName = value;

	/*
	if(!getPayRelationFromDB(m_userName, m_passWord, m_servName))
	{
		return false;
	}
	*/

	//获取输入文件路径
	GET_PARAM(L,"inputFilePath");
	m_billFacConfig.m_inputFilePath = value;
	if (!IsValidPath(m_billFacConfig.m_inputFilePath))
	{
		exit(-1);
	}
	if (m_billFacConfig.m_inputFilePath
	   [m_billFacConfig.m_inputFilePath.length()-1]!='/')
	{
		m_billFacConfig.m_inputFilePath.append("/");
	}
	#ifdef _DEBUG_
	cout << "输入文件路径:" << m_billFacConfig.m_inputFilePath << endl;
	#endif

	//获取输出文件工作路径
	GET_PARAM(L,"outputFileWorkPath");
	m_billFacConfig.m_outputFileWorkPath = value;
	if (!IsValidPath(m_billFacConfig.m_outputFileWorkPath))
	{
		exit(-1);
	}
	if (m_billFacConfig.m_outputFileWorkPath
	   [m_billFacConfig.m_outputFileWorkPath.length()-1]!='/')
	{
		m_billFacConfig.m_outputFileWorkPath.append("/");
	}
	#ifdef _DEBUG_
	cout << "输出文件工作路径:"
	     << m_billFacConfig.m_outputFileWorkPath << endl;
	#endif


	//获取输出文件路径
	for (int k = 0; k < MAX_USERACCOUNT_CHANNELNUM; k++)
	{
	    string path;
        sprintf(cTmp,"%d", k/10); //modify 20091231
        path=cTmp;

	    GET_PARAM(L, "outputFilePath" + path);
    	m_billFacConfig.m_outputFilePath[k] = value;
    	if (!IsValidPath(m_billFacConfig.m_outputFilePath[k]))
    	{
    		exit(-1);
    	}
    	if (m_billFacConfig.m_outputFilePath[k]
                        [m_billFacConfig.m_outputFilePath[k].length()-1]!='/')
        {
            m_billFacConfig.m_outputFilePath[k].append("/");
        }
    	#ifdef _DEBUG_
    	cout << "输出文件路径:" << m_billFacConfig.m_outputFilePath[k] << endl;
    	#endif
  }

	if (m_billFacConfig.m_isNeedBackupValidFile)
	{
		//获取备份文件路径
		GET_PARAM(L,"backupPath");
		m_billFacConfig.m_backupPath = value;
		if (!IsValidPath(m_billFacConfig.m_backupPath))
	    {
		    exit(-1);
	    }
		if (m_billFacConfig.m_backupPath
		  [m_billFacConfig.m_backupPath.length()-1]!='/')
		{
			m_billFacConfig.m_backupPath.append("/");
		}
		#ifdef _DEBUG_
		cout << "备份文件路径:" << m_billFacConfig.m_backupPath << endl;
		#endif
	}



	if (m_billFacConfig.m_isNeedBackupInvalidFile)
	{
		//获取非法文件放置路径
		GET_PARAM(L,"invalidPath");
		m_billFacConfig.m_invalidPath = value;
		if (!IsValidPath(m_billFacConfig.m_invalidPath))
	    {
		    exit(-1);
	    }
		if (m_billFacConfig.m_invalidPath
		  [m_billFacConfig.m_invalidPath.length()-1]!='/')
		{
			m_billFacConfig.m_invalidPath.append("/");
		}
		#ifdef _DEBUG_
		cout << "非法文件放置路径:"
		     << m_billFacConfig.m_invalidPath << endl;
		#endif
	}

	return true;
}


/*
bool UserBillFacApp::getPayRelationFromDB(const string& userName,
                              const string& passWord, const string& servName)
{
	DbInterface dbi;

	bool bConnected=false;

	int iNum=0;
    int iRet=0;
    int iErr=0;

	vector<string> recordSet;
    vector<string> errorSet;

    string s1, s2;

	if (int iRet=dbi.connect(userName.c_str(),
	                        passWord.c_str(), servName.c_str()))
    {
    	theErrorMessages->insert(ERROR_PARAM_DATABASE, "Fail to connect DB");
    	cout << iRet << endl;
    	return false;
    }
    else
    {
        #ifdef _DEBUG_
    	cout << "Connect DB successfully" << endl;
    	#endif
    	bConnected=true;
    }

	if (dbi.dataBind(recordSet,errorSet))
	{
		theErrorMessages->insert(ERROR_PARAM_DATABASE, "dataBind error!!!");
		return false;
	}
	else
	{
	    #ifdef _DEBUG_
		cout << "dataBind successfully!!" << endl;
		#endif
	}

	if (bConnected)
    {
    	iNum = DEFAULTARRAYLINE;
    	while (iNum == DEFAULTARRAYLINE)
        {
	    	if (dbi.executeSql("select unify_pay_code, to_char(acct_id) from \
	    	                    TI_A_ACCT_UNIFYPAY_ALL order by unify_pay_code",
	    	                                                    iNum,iRet,iErr))
	    	{
	    		theErrorMessages->insert(ERROR_PARAM_DATABASE,
		                "Execute Sql Error! SELECTSql: \
		                 select unify_pay_code, to_char(acct_id) from \
			    	     TI_A_ACCT_UNIFYPAY_ALL order by unify_pay_code");
	    		return false;
	    	}
	    	if (iNum>0)
	    	{
	    		//for (int i=1; i<= iNum-2; i+=2)
	    		for (int i=1; i<= iNum*2-1; i+=2)
	    		{
	                m_mapUnifiedPayRelation[recordSet[i-1]] = recordSet[i];
	    		}
	    	}
	    }
    }

	if (bConnected)
    {
    	dbi.disconnect();
    }

	return true;
}
*/


void UserBillFacApp::getCurrDate()
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
}


bool UserBillFacApp::getBcycDateFromFile(const string &fileName,
                                             string &bcycDate)
{
	if (judgeFileNameIsValid(fileName))//如果文件名称合法
	{
	    int ipos = fileName.length();
	    int iposStart1 = ipos - 19;

	        ipos = fileName.length();
	    int iposStart2 = ipos - 14;

	    bcycDate = fileName.substr(iposStart2,2) + fileName.substr(iposStart1,4); 
	    //add by zhouq begin
      int bcycDateint;
      if( !(bcycDateint = atoi(bcycDate.c_str())))
      return false;
      int year = bcycDateint/100;
      int month = bcycDateint%100;
      if(year <2000)
      {
         return false;
      }
      if(month < 1||month >12)
      {
         return false;
      }
      return true;
      //add by zhouq end

	}   
	else
	{   
		return false;
	}   
}     
      
      
int  UserBillFacApp::judgeFileDateIsValid(const string &fileName)
{     
	//在getBcycDateFromFile从首个文件获取到本次处理的m_dealBcycDate之后，
	//判断其他文件名称中日期是否为每次处理m_dealBcycDate
	int ipos = fileName.length();
	int iposStart1 = ipos - 19;

	ipos = fileName.length();
	int iposStart2 = ipos - 14;
	if (fileName.substr(iposStart2,2) + fileName.substr(iposStart1,4) == m_dealBcycDate)
	{
		return VALID_FILE;
	}
	else
	{
		return NOT_ONE_BATCH_FILE; //文件名称合法但不属于此次处理的一批文件中
	}
}


bool UserBillFacApp::judgeFileNameIsValid(const string &fileName)
{
	/*
	FILENAMERULEMMAPITER itr;

	if (fileName.length() < 20)  //名城最短的文件C文件长度也有20了
	{
		return false;
	}

	if (!m_mmapFileNameRule.count(fileName[0]))
	{
		//无此类型的文件,直接返回false
		return false;
	}
	else
	{
		//找出所有此类型文件的命名规则
		for (itr = m_mmapFileNameRule.lower_bound(fileName[0]);
		       itr != m_mmapFileNameRule.upper_bound(fileName[0]); itr++)
		{
			//文件名长度是否对(由于文件名长度不一定，所以此处不再做精确判断。)
			if ( fileName.length() >= 20 )
			{
				//是否需要对文件名称进一步作详细判断
				if (m_billFacConfig.m_isNeedJudgeNameDetail)
				{
					return judgeFileNameDetailIsValid(fileName,
					                                 (*itr).second.second);
				}
				else
				{
				    return true;
				}
			}
			else
			{
				continue;
			}
		}
		return false;
	}*/
	return true;
}


bool UserBillFacApp::judgeFileNameDetailIsValid(const string &fileName,
                  const vector<FileNameRule>& vecFileNameRule)
{
	vector<FileNameRule>::const_iterator itr,itrend;
	itrend = vecFileNameRule.end();
	for (itr = vecFileNameRule.begin();
	              itr != itrend; itr++)
    {
        if ((fileName.substr((*itr).m_firstPos,(*itr).m_len)>(*itr).m_MaxValue)
    	|| (fileName.substr((*itr).m_firstPos,(*itr).m_len)<(*itr).m_MinValue))
    	{
    		return false;
    	}
    }
    return true;
}


bool UserBillFacApp::userBillFacRun()
{
	//获取所有文件
	if (getAllValidFile())
	{
	    if (m_dealBcycDate!="") //如果有合法文件
		{
			//获取当前时间
	        getCurrDate();

	        //刷新统一付费关系资料
	        /*
	        if (!getPayRelationFromDB(m_userName, m_passWord, m_servName))
	        {
	        	return false;
	        }
	        */

	        //处理所有合法文件
	        if (!dealAllValidFile())
	        {
	        	return false;
	        }

	        //输出账单
			if (!writeUserBill())
			{
				return false;
			}
		}

		if (!linkUnlink())
		{
			return false;
		}
	}
	else
	{
		//更新状态
		m_objectName = "------";
        m_status = PROC_IDLE;
        updateStatus();

        //休眠
		#ifdef _DEBUG_
		cout << "sleep(" << m_billFacConfig.m_sleepTime << ")" << endl;
		#endif
		sleep(m_billFacConfig.m_sleepTime);
	}

	return true;
}


int UserBillFacApp::getAllValidFile()
{
	int                                  iRet = 0;
	unsigned long                        ulongFileSize = 0;

	FileNameInfo                         fileNameInfo;
	vector<FileNameInfo>                 vecFileNameInfoTmp;

	vector<FileNameInfo>::iterator       itrFileNameInfo;

	char                                 tmpName[100];
	memset(tmpName, 0, sizeof(tmpName));

	//初始化
	m_dealBcycDate="";
	m_fileNameInfoVector.clear();
	vecFileNameInfoTmp.clear();
	fileNameInfo.m_fileName = "";
	fileNameInfo.m_bcycDate = "";
	fileNameInfo.m_validTag = INVALID_FILE;

	//设置扫描文件最大个数
	m_directory.setMaxScan(m_billFacConfig.m_dealFileNum+2);
	//第一个1表示按照名称排列,后一个表示只遍历一层,不对子文件夹递归
	m_directory.scanFiles(Directory::SF_MODIFYTIME, 1);
	//先将所有文件名称取到m_originFileNameVector中，按照文件名称所标注的时间排序
	while (iRet = m_directory.getFile(tmpName, MAX_FILENAME_LEN, ulongFileSize))
	{
	    fileNameInfo.m_fileName = tmpName;
	    //超出文件名最大长度或者文件大小为0,文件直接非法
		if (iRet < 0 || ulongFileSize==0 )
		{
			//处理不合法文件(名称超常，大小为0)
			fileNameInfo.m_bcycDate = "";
	        fileNameInfo.m_validTag = INVALID_FILE;
		}
		else
		{
		    //因为还没有确定处理文件的开始时间
		    //所以把每个文件都当作首个文件来判断一下,顺便获取文件所标注的时间
		    if (getBcycDateFromFile(fileNameInfo.m_fileName,
		                                               fileNameInfo.m_bcycDate))
	        {
	        	  // add by zhouq begin
	        	  if(fileNameInfo.m_bcycDate.substr(2,4) == "0000")
	        	  {
	        	  	 fileNameInfo.m_bcycDate = "";
	        	  	 fileNameInfo.m_validTag = INVALID_FILE;
	        	  }else
	        	  {
	        	  //add by zhouq end
	               //文件命名规则合法
	               fileNameInfo.m_validTag = VALID_FILE;
	            }
	        }
	        else
	        {
	            //文件命名规则非法
	            fileNameInfo.m_bcycDate = "";
	            fileNameInfo.m_validTag = INVALID_FILE;
	        }
		}
		vecFileNameInfoTmp.push_back(fileNameInfo);
		memset(tmpName, 0, sizeof(tmpName));
	}

	#ifdef _DEBUG_
	for (vector<FileNameInfo>::iterator itr=vecFileNameInfoTmp.begin();
	           itr!=vecFileNameInfoTmp.end(); itr++)
	{
	    cout << itr->m_fileName << "|"
	         << itr->m_bcycDate << "|"
	         << itr->m_validTag << ", ";
	}
	cout << endl;
	cout << endl;
	#endif

	//按照文件名称所标注的时间排序,fileNameInfo'<'规则
	::partial_sort(vecFileNameInfoTmp.begin(),
	             vecFileNameInfoTmp.end(),  vecFileNameInfoTmp.end());
	#ifdef _DEBUG_
	for (vector<FileNameInfo>::iterator itr=vecFileNameInfoTmp.begin();
	           itr!=vecFileNameInfoTmp.end(); itr++)
	{
	    cout << itr->m_fileName << "|"
	         << itr->m_bcycDate << "|"
	         << itr->m_validTag << ", ";
	}
	cout << endl;
	#endif

    for (itrFileNameInfo = vecFileNameInfoTmp.begin();
	    itrFileNameInfo != vecFileNameInfoTmp.end(); itrFileNameInfo++)
	{
	    //cout << "进入for循环" << endl;
	    //cout << "vecFileNameInfoTmp.size() = "
	    //      << vecFileNameInfoTmp.size() << endl;
        if (itrFileNameInfo->m_validTag == INVALID_FILE)//非法文件INVALID_FILE
        {
            //处理非法文件
            //cout << "非法文件:" << itrFileNameInfo->m_fileName << endl;
            m_fileNameInfoVector.push_back(*itrFileNameInfo);
            //文件个数超过最大允许文件个数，跳出for循环，不再插入
		    if (m_fileNameInfoVector.size()>m_billFacConfig.m_dealFileNum)
		    {
		        break;
		    }
        }
        else //合法文件VALID_FILE
        {
            //m_dealBcycDate还没有赋值,表明是首个文件
			if (m_dealBcycDate=="")
			{
				//第一次取到合法文件,生成流水号,日志开始
	            m_performId = theLogger->pBegin();

				//是首个文件,设置本次处理的日期
				m_dealBcycDate = itrFileNameInfo->m_bcycDate;

	            m_fileNameInfoVector.push_back(*itrFileNameInfo);
	            //文件个数超过最大允许文件个数，跳出for循环，不再插入
    		    if (m_fileNameInfoVector.size() >
    		        m_billFacConfig.m_dealFileNum)
    		    {
    		        break;
    		    }
			}
			else
			{
			    //判断文件名称所包含的日期是否是本次m_performId期间处理
				itrFileNameInfo->m_validTag =
				              judgeFileDateIsValid(itrFileNameInfo->m_fileName);
				if ( (itrFileNameInfo->m_validTag == VALID_FILE)   ||
				     (itrFileNameInfo->m_validTag == INVALID_FILE))
				{
					m_fileNameInfoVector.push_back(*itrFileNameInfo);
					//文件个数超过最大允许文件个数，跳出for循环，不再插入
        		    if (m_fileNameInfoVector.size() >
        		                                  m_billFacConfig.m_dealFileNum)
        		    {
        		        break;
        		    }
			    }
			    else   //NOT_ONE_BATCH_FILE
			    {
			    	//itrFileNameInfo.m_validTag==NOT_ONE_BATCH_FILE的情况
			    	//文件名称合法但不属于此次处理的一批文件中
	                //则不保留,从m_fileNameInfoVector中删除该文件名称，
	                //留在input目录下，下次再处理
			    }
			}
        }
	} //end of for

	/*由于现在同一个文件中可能存在重复的账单记录，所以采用后取到的账单更新先取到的账单的机制，
	  所以不同批次之间需要保持相同的处理顺序，先生成的先处理。
	//按文件名称逆序(大的在前,小的在后)排序,fileNameInfo'>'规则
	::partial_sort(m_fileNameInfoVector.begin(), m_fileNameInfoVector.end(),
	           m_fileNameInfoVector.end(), greater<FileNameInfo>());
	*/
	#ifdef _DEBUG_
	for (vector<FileNameInfo>::iterator itr=m_fileNameInfoVector.begin();
           itr!=m_fileNameInfoVector.end(); itr++)
	{
	    cout << itr->m_fileName << "|"
	         << itr->m_bcycDate << "|"
	         << itr->m_validTag << ", ";
	}
	cout << endl;
	#endif

	return m_fileNameInfoVector.size();
}


bool UserBillFacApp::dealAllValidFile()
{
    vector<FileNameInfo>::iterator itr;
    vector<FileNameInfo>::iterator itrend = m_fileNameInfoVector.end();

    //初始化路径
    m_userBillFacInputFile.init(m_billFacConfig.m_inputFilePath,
                                   m_mapUnifiedPayRelation);

    for (itr=m_fileNameInfoVector.begin();
                          itr != itrend; itr++)
    {
    	//如果是合法文件
    	if ((*itr).m_validTag == VALID_FILE)
    	{
    		//针对输入文件日志开始处理
            theLogger->pBeginInput(m_performId,
                           (m_billFacConfig.m_inputFilePath +
                                        (*itr).m_fileName).c_str(),
                           "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                           "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");

    		//更新状态
		    m_objectName = "处理输入文件" + (*itr).m_fileName;
            m_status = PROC_RUN;
            updateStatus();

    		//处理文件
    		if(!dealFile((*itr).m_fileName))
    		{
    			return false;
    		}

    		//针对输入文件日志结束处理
        theLogger->pEndInput(m_performId,
                         (m_billFacConfig.m_inputFilePath +
                                        (*itr).m_fileName).c_str(),
                         m_accountLogInfo.m_recordNum,
                         "TOTAL_ID_NUM=%d,ERR_ID_NUM=%d,ERR_REC_NUM=%d,"
                         "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=%lld",
                         m_accountLogInfo.m_idNum,
                         m_accountLogInfo.m_errorIdNum,
                         m_accountLogInfo.m_errorRecordNum,
                         m_accountLogInfo.m_fee,
                         m_accountLogInfo.m_filterFee);

    	}
    }

    return true;
}


bool UserBillFacApp::dealFile(const string &inputBillFileName)
{
	vector<UserBill>                          vecUserBillNow; //新取出的帐单
	vector<UserBill>::iterator                itrNow,vecitr;

	//add by zhouq end
	USERBILLMAPITER                           itrMap;

	UserKey                                   userKey;

	UserBill                                  userbillNew,userbillUpdate;

	USERBILLFEEMAP                            mapUserBillFee;//取出的账目费用进行排重
	USERBILLFEEMAPITER                        mapUserBillFeeIter;
	UserBillTrack                             userBillTrack;
	long                                      mapUserBilldiscountFee;

	//累计内存账单中找到相同账单的次数
	int	                                      findnum = 0;

	bool     IsRecorded = false;    //以前是否有记录

	//初始化日志信息
	m_accountLogInfo.m_idNum = 0;
	m_accountLogInfo.m_errorIdNum = 0;
	m_accountLogInfo.m_recordNum = 0;
	m_accountLogInfo.m_errorRecordNum = 0;
	m_accountLogInfo.m_fee = 0;
	m_accountLogInfo.m_filterFee = 0;

	//判断文件是本地文件还是异地文件
	char localOrRemoteTag = (m_mapLocalOrRemote[inputBillFileName[0]]?
	                         m_mapLocalOrRemote[inputBillFileName[0]]:'0');
	//打开文件
	if (m_userBillFacInputFile.openFile(inputBillFileName, localOrRemoteTag,
	                                                      &m_accountLogInfo))
	{
		//从m_accountLogInfo返回记录数m_recordNum和错误记录数m_errorRecordNum
		while (m_userBillFacInputFile.getUserBill(userKey,vecUserBillNow))
		{
			//用户数+1
			m_accountLogInfo.m_idNum++ ;

			//累计日志费用m_fee
			int fee;
			fee = 0;
			for (itrNow=vecUserBillNow.begin();
			        itrNow!=vecUserBillNow.end(); itrNow++)
			{
			   //逐笔费用累加
			   fee += (*itrNow).m_discountFee;
			}
			m_accountLogInfo.m_fee += fee;

			itrMap=m_userBillMap.find(userKey);
			//如果以往有这个帐单记录
			if (itrMap!=m_userBillMap.end())
			{
			  //处理方式 2 ：将该用户之前的账单全部删除，将新获取的账单全量插入
			  (*itrMap).second.clear();
			  (*itrMap).second.swap(vecUserBillNow);
			}
			//如果以往没有这个用户的帐单记录,直接插入
			else
			{
				m_userBillMap.insert(
				        USERBILLMAP::value_type(userKey, vecUserBillNow));
			}
			vecUserBillNow.clear();
			
		}//end of while

		//关闭文件
		m_userBillFacInputFile.closeFile();
	}
	else
	{
		//打开输入文件失败处理
		return false;
  }
    return true;
}


bool UserBillFacApp::writeUserBill()
{
	if (!m_userBillFacOutputFile.init(m_performId, &m_billFacConfig,
	                                       m_dealBcycDate, m_dealDate))
	{
		return false;
	}

	//更新状态
	m_objectName = "正在输出文件...";
	m_status = PROC_RUN;
	updateStatus();
            
	if (!m_userBillFacOutputFile.writeFile(m_userBillMap))
	{
		return false;
	}

	m_userBillFacOutputFile.closeFile();

	if (!m_userBillFacOutputFile.linkUnlink())
	{
		return false;
	}

  //更新状态
	m_objectName = "文件输出完毕";
  m_status = PROC_RUN;
  updateStatus();

	//清空用户帐单内存
	m_userBillMap.clear();
	
	//日志结束
  theLogger->pEnd(m_performId);
  m_performId = "";

  return true;
}


bool UserBillFacApp::linkUnlink()
{
	//删除输入文件
	vector<FileNameInfo>::iterator itr;
	for (itr=m_fileNameInfoVector.begin();
	     itr!=m_fileNameInfoVector.end(); itr++)
	{
		if ((*itr).m_validTag == VALID_FILE)
		{
			//删除或备份原有的合法的原始外部输入文件
			if (m_billFacConfig.m_isNeedBackupValidFile)
			{
				//备份
				if (rename(
				    (m_billFacConfig.m_inputFilePath+(*itr).m_fileName).c_str(),
				    (m_billFacConfig.m_backupPath+(*itr).m_fileName).c_str()
				          ) < 0)
				{
					theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_billFacConfig.m_inputFilePath+(*itr).m_fileName
                        + " ->" +
                        m_billFacConfig.m_backupPath+(*itr).m_fileName +
                        "--file rename error");
                    return false;
				}
			}
			else
			{
				//直接删除
				if (unlink(
				(m_billFacConfig.m_inputFilePath+(*itr).m_fileName).c_str()
				         ) != 0)
				{
					theErrorMessages->insert(ERROR_LINK_UNLINKE,
					    m_billFacConfig.m_inputFilePath+(*itr).m_fileName +
                                                    "-- file unlink error");
                    return false;
				}
			}
		}
		else if ((*itr).m_validTag == INVALID_FILE)
		{
			//删除或备份原有的不合法的原始外部输入文件
			if (m_billFacConfig.m_isNeedBackupInvalidFile)
			{
				//拷贝到Invalid路径下
				if (rename(
				(m_billFacConfig.m_inputFilePath+(*itr).m_fileName).c_str(),
				(m_billFacConfig.m_invalidPath+(*itr).m_fileName).c_str()
				         ) < 0)
				{
					theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_billFacConfig.m_inputFilePath+(*itr).m_fileName +
                        " ->" +
                        m_billFacConfig.m_backupPath+(*itr).m_fileName +
                        "--file rename error");
                    return false;
				}
			}
			else
			{
				//直接删除
				if (unlink(
				 (m_billFacConfig.m_inputFilePath+(*itr).m_fileName).c_str()
				         )!= 0)
				{
					theErrorMessages->insert(ERROR_LINK_UNLINKE,
					m_billFacConfig.m_inputFilePath+(*itr).m_fileName +
                    "-- file unlink error");
                    return false;
				}
			}
		}
	}//end of for (itr=m_fileNameInfoVector.begin()...

	return true;
}


bool UserBillFacApp::IsValidPath(const string& sPath)
{
	DIR *pDir;
	if ( (pDir=opendir(sPath.c_str())) == NULL )
	{
	    cout << "open "
		     << sPath << " err! " << endl;
		return false;
	}
	return true;
}


int  UserBillFacApp::split(string& source, const string& seperator,
                        vector<string> &destVector)
{
    destVector.clear();

    string::size_type pos;
    string            tempString;
    while((pos=source.find_first_of(seperator)) != string::npos)
    {
        tempString = source.substr(0, pos);
        destVector.push_back(tempString);
        source.erase(0, pos+1);
    }
    destVector.push_back(source);

    return destVector.size();
}

