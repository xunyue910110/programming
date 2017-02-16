#include "Inputer.h"
#include "RateLog.h"
#include "RateConstDef.h"

Inputer::Inputer()
{
  m_buffLen       = 0;
  m_buffer        = NULL;
  m_patternType   = PATTERN_TYPE_FILE;
}
Inputer::~Inputer()
{
}

void  Inputer::setPatternType(const PATTERNTYPE &r_patterType)
{
  m_patternType = r_patterType;
}
bool  Inputer::initialize(RateGlobalInfo *r_gRateInfo,
                          InputRuleMgr   *r_inputRule)
{
  m_gRateInfo    = r_gRateInfo;
  m_buffer       = r_gRateInfo->m_cdr.m_orgBuff;
  m_buffLen      = r_gRateInfo->m_cdr.m_orgBufLen;
  m_inputRuleMgr = r_inputRule;
  if(dealDelayedFile()==false)
  {
    #ifdef _DEBUG_
      cout<<"dealDelayedFile() false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

// 1 OK  0 文件结束 -1 失败
int Inputer::getCdr() 
{
	//Modified By Sunam 2009/8/10 19:35:37
 	//int t_iRet=0;
 	//t_iRet = read(m_buffer,m_buffLen,m_inputRule->m_pFormat->m_lineDeliminater);
 	//if(t_iRet>0)
 	//{
	//	m_gRateInfo->m_rateLog->addTotalCdr();
 	//}
 	//return t_iRet;
 	//话单是否处理完?
 	if (m_recordIndex < m_recordNum)
	{
		//未处理完从话单缓存中取一条记录至m_buffer
		memset(m_buffer,0,sizeof(m_buffer));
		strcpy(m_buffer,m_fileBuffer[m_recordIndex++]);
		return 1;
	}
	return 0;
}
//-1 错误文件 ， 0 无文件 ，1 有文件
int Inputer::getFile(PString &r_fileName,PString &r_fullFileName) 
{
  const int T_FILENAME_LEN = 128;
  unsigned long t_filesize =0;
  char t_file[T_FILENAME_LEN];
  char t_path[T_FILENAME_LEN*4];
  int  t_iRet = 0;
  m_remainFlag = false;//清缓冲标记
  // 取下一个文件
  while (m_dir.getFile(t_file, T_FILENAME_LEN, t_filesize)> 0) 
  {
    m_dir.getPath(t_path,sizeof(t_path)-1);
         
    setName(t_file,t_path);
    m_gRateInfo->setFileName(t_file);
    //忽略某些临时文件（可能正被其他进程处理）
    if(m_sFileName[0]=='#'||m_sFileName[0]=='%')
    {
      continue;
    }
    r_fileName = m_sFileName;
    r_fullFileName = m_fullName;
    m_inputRule = m_inputRuleMgr->getInputRule(m_sFileName.c_str(),m_patternType);
    //移除无效文件 (无效的文件名
    if (m_inputRule == NULL ) 
    {
      mvFileToInvalidPath();
      t_iRet = -1;
      return t_iRet;
    }
    t_iRet = 1;
    return t_iRet;
  }

  //扫描输入源
  m_inputRuleMgr->m_currentPathIndex++;
  if ( m_inputRuleMgr->m_currentPathIndex >= m_inputRuleMgr->m_intputPaths.size()) 
    m_inputRuleMgr->m_currentPathIndex=0;  
  InputRuleMgr::InputPath & t_inputPath2 = m_inputRuleMgr->m_intputPaths[m_inputRuleMgr->m_currentPathIndex];
  m_dir.setPath(t_inputPath2.m_inputPath.c_str());
  m_dir.setMaxScan(600); //每次处理的最大文件数
  m_dir.scanFiles((Directory::SortFlags)t_inputPath2.m_sortMode);

  return t_iRet;
}
//将文件移到无效目录下
bool Inputer::mvFileToInvalidPath()
{
  InputRuleMgr::InputPath & t_inputPath1 = m_inputRuleMgr->m_intputPaths[m_inputRuleMgr->m_currentPathIndex];
  renameFile(m_sFileName,t_inputPath1.m_invalidPath.c_str());
  return true;
}

bool Inputer::open()
{
	if (!Inoutputer::open(ios::in))
	{
		return false;
	}
	m_userList.clear();
  	m_recordIndex = 0;
	m_recordNum   = 0;
	int t_iRet=0;
	do
	{
		//t_iRet = read(m_fileBuffer[m_recordNum],m_buffLen,m_inputRule->m_pFormat->m_lineDeliminater);
		//if (t_iRet>0)
		//{
		//	m_gRateInfo->m_rateLog->addTotalCdr();
		//	strcpy(m_buffer,m_fileBuffer[m_recordNum]);
		//	++m_recordNum;
		//	//文件记录数太多
		//	if (m_recordNum >= MAXRECORDS)
		//	{
		//		return false;
		//	}
		//	formatCdr();
		//	m_userList.push_back(m_gRateInfo->m_cdr[ATTRI_USER_ID].c_str());
		//}
		memset(m_buffer,0,sizeof(m_buffer));
		t_iRet = read(m_buffer,m_buffLen,m_inputRule->m_pFormat->m_lineDeliminater);
		if (t_iRet>0)
		{
		    //过滤掉空行
		    if (strlen(m_buffer) < 10)
		    {
		        break;
		    }
		    //文件记录数太多
			if (m_recordNum >= MAXRECORDS)
			{
				return false;
			}
			memset(m_fileBuffer[m_recordNum],0,sizeof(m_fileBuffer[m_recordNum]));
			//strcpy(m_fileBuffer[m_recordNum],m_buffer);
			strncpy(m_fileBuffer[m_recordNum],m_buffer,MAXLENGTH-1);
			++m_recordNum;
			m_gRateInfo->m_rateLog->addTotalCdr();
			formatCdr();
			if (m_gRateInfo->m_pConfig->m_channelType == CHANNEL_PROV)
			{
			    m_userList.push_back(m_gRateInfo->m_cdr[ATTRI_USER_ID].c_str());
			}
			else
			{
			    m_userList.push_back(m_gRateInfo->m_cdr[ATTRI_MSISDN_A].c_str());
			}
		}
	}while(t_iRet>0);
	//用户改成单条加锁
	//Added By Sunam 2009/8/27 10:53:49 取所有户的付费关系
	//m_gRateInfo->m_comBill.getUserPayInfo(m_userList);
	return true;
}

//Added By Sunam 2009/8/10 19:27:30 获取当前未处理用户
void Inputer::genNoDealUser()
{
    //Added By Sunam 2009/10/25 14:42:26
    if (m_gRateInfo->m_pConfig->m_channelType != CHANNEL_PROV)
    {
        return;
    }
	m_gRateInfo->m_lockUserList.clear();
	//m_gRateInfo->m_lockAcctList.clear();
	vector<T_SERVER_ID>::iterator t_it; 
    //map<T_SERVER_ID, vector<PayRelation> >::iterator t_payItr;
	for (int i = m_recordIndex; i < m_recordNum; ++i)
	{
		m_gRateInfo->m_lockUserList.insert(m_userList[i]);
        //t_payItr = m_gRateInfo->m_comBill.m_userPaylationMap.find(m_userList[i]);
        //if (t_payItr != m_gRateInfo->m_comBill.m_userPaylationMap.end())
        //{
        //    for (int j=0; j < t_payItr->second.size(); ++j)
        //    {
        //        m_gRateInfo->m_lockAcctList.insert(t_payItr->second[j].m_acctId);
        //    }
        //}
	}
}

//Added By Sunam 2009/8/10 19:27:30 前一条话单
void Inputer::previousCdr()
{
	--m_recordIndex;
}

//返回值：-1 失败  0 成功 1 成功不需后续处理
int Inputer::formatCdr() 
{
  if(m_gRateInfo->m_cdr.inputCdr(m_inputRule->m_pFormat)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_gRateInfo->m_cdr.inputCdr(m_inputRule->m_pFormat) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  for(map<int,PString>::iterator t_initItr=m_inputRule->m_initValues.begin();
      t_initItr!=m_inputRule->m_initValues.end();++t_initItr)
  {
    m_gRateInfo->m_cdr[t_initItr->first] = t_initItr->second.c_str();
  }
  #ifdef _DEBUG_
    m_gRateInfo->m_cdr.dumpCdrInfo(m_inputRule->m_pFormat);
  #endif
  return 0;
}

bool  Inputer::fileCommit()
{
  InputRuleMgr::InputPath &t_inputInfo=
      m_inputRuleMgr->m_intputPaths[m_inputRuleMgr->m_currentPathIndex];
  bool t_bRet = true; 
  if(t_inputInfo.m_bakFlag == true &&
     !(strncmp(m_sFileName.c_str(),USER_FILE_BILL.c_str(),strlen(USER_FILE_BILL.c_str()))==0 ||
       strncmp(m_sFileName.c_str(),RENTFEE_FILE_INCRE.c_str(),strlen(RENTFEE_FILE_INCRE.c_str()))==0 ||
       strncmp(m_sFileName.c_str(),RENTFEE_FILE_ALL.c_str(),strlen(RENTFEE_FILE_ALL.c_str()))==0 )
     ) //出帐事件和固定费事件强制不备份
  {
    if(renameFile(m_sFileName,t_inputInfo.m_inputBackPath)==false)
    {
      t_bRet = false;
    }
  }else
  {
    if(unlinkFile()==false)
    {
      t_bRet = false;
    }
  }
  if(m_delayFile.is_open())  
  { //打开状态，表示有延迟话单输出
    m_delayFile.close();
    m_delayFile.renameFile(m_delayFile.getFileName(),
          m_inputRuleMgr->m_intputPaths[m_inputRuleMgr->m_currentPathIndex]
          .m_delayedPath.c_str());
  }
  return t_bRet;
}
//输出延迟话单
bool Inputer::outputDelayCdr()
{
  if(m_delayFile.is_open()==false)
  {
    PString t_fileName = m_sFileName+".D";
    m_delayFile.setName(t_fileName.c_str(),
                        m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_delayFile.open(ios::out)==false)
    {
      #ifdef _DEBUG_ 
        cout<<"open file error!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_gRateInfo->m_rateLog->setDelayFileName(m_delayFile.getFullFileName());
  }
  int t_cdrLen=strlen(m_buffer);
  m_buffer[t_cdrLen]='\n';++t_cdrLen;
  m_buffer[t_cdrLen]=0; //最后置结束符  
  if(m_delayFile.write(m_buffer)==false)
  {
    #ifdef _DEBUG_ 
      cout<<"wirte cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_gRateInfo->m_rateLog->addDelayCdr();
  return true;
}
//将延迟用户从延迟目录移至输入目录下.
bool Inputer::dealDelayedFile()
{
  Directory t_dir;
  int       t_iRet;
  const int T_FILENAME_LEN = 128;
  char t_file[T_FILENAME_LEN];
  bool      t_haveFile;
  
  for(vector<InputRuleMgr::InputPath>::iterator t_pathItr =
      m_inputRuleMgr->m_intputPaths.begin();
      t_pathItr!= m_inputRuleMgr->m_intputPaths.end();
      ++t_pathItr)
  {
    t_dir.setPath(t_pathItr->m_delayedPath.c_str());
    t_dir.setMaxScan(5);
    do
    {
      t_haveFile = false; //默认为无文件
      t_dir.scanFiles((Directory::SortFlags)t_pathItr->m_sortMode);
      while(t_dir.getFile(t_file, T_FILENAME_LEN)> 0) 
      {
        t_haveFile=true; //有文件，可能还有文件没扫描到
        m_delayFile.setName(t_file,t_pathItr->m_delayedPath.c_str());
        m_delayFile.renameFile(t_file,t_pathItr->m_inputPath.c_str());
      }
    }while(t_haveFile==true);
  } 
  return true;
}

bool Inputer::setInputRule(const char *r_sourceType,const PATTERNTYPE &r_patternType)
{
  m_patternType = r_patternType;
  m_inputRule = m_inputRuleMgr->getInputRule(r_sourceType,m_patternType);
  if(m_inputRule==NULL)
  {
    return false;
  }
  return true;
}

// 支持固定费话单 ： 一次性从文件中读取多条记录userid相同的
// -1 错误, 0 文件处理完毕, >0 成功话单条数
int Inputer::getCdr(T_SERVER_ID &r_userId,vector<PString> &r_rentCdrList)
{
  //返回话单条数
  int          t_iRet = 0;
  T_SERVER_ID  t_userId;
  bool         t_firstFlag = true;
  
  r_rentCdrList.clear();
  //1. 如果中间有保留的话单，先处理保留话单
  if(m_remainFlag == true)
  {
    sprintf(m_buffer,m_remainCdr.c_str());
    m_remainFlag = false;
    t_iRet = formatCdr();
    if(t_iRet < 0)
    {
      return -1;
    }else if(t_iRet==0) 
    {
      r_rentCdrList.push_back(m_remainCdr);
      t_firstFlag = false;
      t_userId = m_gRateInfo->m_cdr[ATTRI_USER_ID];
      r_userId = t_userId;
    }// t_iRet > 0 不需要处理
  }
  while(1)
  {
    t_iRet = getCdr(); //读取一条话单
    if(t_iRet < 0)     //出错
    {
      return -1;
    }else if(t_iRet==0)//文件处理完毕
    { 
      break;
    }else //t_iRet>0   读取话单成功
    {
      t_iRet = formatCdr();
      if(t_iRet < 0)
      {
        return -1;
      }else if(t_iRet>0) //不需要处理
      {
        continue; 
      }else
      {
        if(t_firstFlag == true)
        { //第一条话单
          r_rentCdrList.push_back(PString(m_buffer));
          t_firstFlag = false;
          t_userId = m_gRateInfo->m_cdr[ATTRI_USER_ID];
          r_userId = t_userId;
          continue;
        }else if(t_userId == m_gRateInfo->m_cdr[ATTRI_USER_ID])
        { //同一个用户
          r_rentCdrList.push_back(PString(m_buffer));
          t_firstFlag = false;
          continue;
        }else //不是同一个用户
        { //将该话单保留,并停止读取
          m_remainCdr = m_buffer;
          m_remainFlag= true;
          break;
        }
      }
    } 
  };
  return r_rentCdrList.size();
}
InputRule* Inputer::getInputRule()
{
  return m_inputRule;
}

