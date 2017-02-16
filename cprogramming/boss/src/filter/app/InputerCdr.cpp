#include "InputerCdr.h"
#include "SortLog.h"
#include "base/StringUtil.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

InputerCdr::InputerCdr()
{
  m_buffLen       = 0;
  m_inputRuleMgr  = NULL;
  m_inputParam    = NULL;
  m_gFilterInfo   = NULL;
  m_fileSum       = 0;
  m_pathNo        = 0;
}
InputerCdr::~InputerCdr()
{
  m_inputRuleMgr  = NULL;
  m_inputParam    = NULL;
  m_gFilterInfo   = NULL;
}

/*
void  InputerCdr::setPatternType(const PATTERNTYPE &r_patterType)
{
  m_patternType = r_patterType;
}
*/

bool  InputerCdr::initialize(FilterGlobalInfo *r_gFilterInfo,InputRuleMgr *r_inputRuleMgr)
{
  m_gFilterInfo  = r_gFilterInfo;

  m_buffLen      = m_gFilterInfo->m_cdr.m_orgBufLen;
  m_buffer       = m_gFilterInfo->m_cdr.m_orgBuff;

  if(m_buffer==NULL)
  {
    #ifdef _DEBUG_
      cout<<"InputerCdr::initialize false! new memory false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_inputRuleMgr    = r_inputRuleMgr;
  //设定起始扫描目录
  m_dir.setPath(m_inputRuleMgr->m_inputParam[0].m_inputPath.c_str());
  m_dir.setMaxScan(MAXSCANNUM);
  m_dir.scanFiles((Directory::SortFlags)m_inputRuleMgr->m_sortMode);

  if(m_inputRuleMgr->m_inputParam.empty())
  {
    #ifdef _DEBUG_
      cout<<"input channel is null false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_pathNo = 0;  
  PString t_subChanName = m_inputRuleMgr->m_inputParam[m_pathNo].m_subChanName;
  m_inputParam = m_inputRuleMgr->getInputParam(t_subChanName);

  return true;
}

// 1 OK  0 文件结束 -1 失败
int InputerCdr::getCdr() 
{
  int t_iRet=0;
  t_iRet = read(m_buffer,m_buffLen,m_inputParam->m_pFormat->m_lineDeliminater);
  if(t_iRet>0)
  {
    //m_gFilterInfo->m_SortLog->addTotalCdr();
  }
  return t_iRet;
}

//-2获取子进程配置失败 -1 错误文件 ， 0 无文件 ，1 有文件
int InputerCdr::getFile(PString &r_fileName) 
{
  const int T_FILENAME_LEN = 128;
  unsigned long t_filesize =0;
  char t_file[T_FILENAME_LEN];
  char t_path[T_FILENAME_LEN*4];
  int  t_iRet = 0;

  //取不到文件or处理文件数超过20，更改目录，和规则
  while (m_dir.getFile(t_file, T_FILENAME_LEN)>0)
  {
    //个数超过20 切换目录
    if(m_fileSum>=20)
    {
       if(changeDirectory()==false)
       {
          t_iRet = -2;
          return t_iRet;
       }
       else
       {
          t_iRet = -1;
          return t_iRet;
       }
    }
    m_dir.getPath(t_path,sizeof(t_path)-1);
    //设定全路径文件名 缺省文件名
    setName(t_file,t_path);
    PString t_subChanName = m_inputRuleMgr->m_inputParam[m_pathNo].m_subChanName;
    m_gFilterInfo->setFileNameChannelName(t_file,m_fullName.c_str(),t_subChanName,m_inputParam->m_bindmethod);
    //忽略某些临时文件（可能正被其他进程处理)
    if(m_sFileName[0]=='#'||m_sFileName[0]=='%')
    {
      continue;
    }
    r_fileName = m_sFileName;
    m_fileSum++;

    t_iRet = 1;
    return t_iRet;  //返回1 有文件
  }

  //目录取不到文件
  if(changeDirectory()==false)
  {
     t_iRet = -2;
     return t_iRet;
  }
  else if(m_pathNo > 0)
  {
     t_iRet = -1;
     return t_iRet;
  }

  return t_iRet;  //返回0 无文件
}

bool InputerCdr::open()
{
  return Inoutputer::open(ios::in);
}

//返回值：-1 失败  0 成功 1 成功不需后续处理
int InputerCdr::formatCdr()
{
  if(m_gFilterInfo->m_cdr.inputCdr(m_inputParam->m_pFormat,m_inputParam->m_fieldCount)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_gFilterInfo->m_cdr.inputCdr(m_inputParam->m_pFormat,m_inputParam->m_fieldCount) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  
  //读入记录日志累加
  m_gFilterInfo->m_sortLog->addLogCdrs(CHANNEL_FILEINPUT,"","");

  return 0;
}

//-1 移动文件失败 0 移动文件成功
int InputerCdr::moveErrFile()
{
    if(rename(m_fullName,(m_inputRuleMgr->m_errorPath+m_sFileName).c_str())<0)
    {
      //cout<<"errno: "<<errno<<"  "<<__FILE__<<__LINE__<<endl;
    	if(Inoutputer::copyFile(m_fullName,(m_inputRuleMgr->m_errorPath+m_sFileName).c_str())<0)
    	{
         return -1;
    	}
    	if(unlinkFile()==false)
    	{
         return -1;
    	}
    }
  return 0;
}

bool  InputerCdr::fileCommit()
{

  if(m_openFlag==true)
  {
     return false;
  }
  if(m_inputRuleMgr->m_isbackSource=='Y')
  {
    if(access((m_inputParam->m_inputPathBack+m_sFileName).c_str(), F_OK) == 0)
    {
      //备份文件存在先删除
      ::unlink((m_inputParam->m_inputPathBack+m_sFileName).c_str());
    }
    if(rename(m_fullName,(m_inputParam->m_inputPathBack+m_sFileName).c_str())<0)
    {
	    #ifdef _DEBUG_
	      cout<<"backup "<<m_fullName<<" to "<<m_inputParam->m_inputPathBack<<" false!"
	          <<__FILE__<<__LINE__<<endl;
	    #endif
      theErrorMessages->insert(E_MOVE_FILE_ERR,"backup '" + m_fullName +  + "' to "+ m_inputParam->m_inputPathBack + " false!");
      return false;
    }
  }
  else if(unlinkFile()==false)
  {
    return false;
  }

  return true;
}

bool InputerCdr::changeDirectory()
{
  PString t_subChanName;
  if(m_pathNo+1>=m_inputRuleMgr->m_inputParam.size())
  {
     m_pathNo = 0;
     m_fileSum = 0;
     t_subChanName = m_inputRuleMgr->m_inputParam[m_pathNo].m_subChanName;
     m_dir.setPath(m_inputRuleMgr->m_inputParam[m_pathNo].m_inputPath.c_str());
     m_dir.setMaxScan(MAXSCANNUM);
     m_dir.scanFiles((Directory::SortFlags)m_inputRuleMgr->m_sortMode);
     m_inputParam = m_inputRuleMgr->getInputParam(t_subChanName);
     if (m_inputParam == NULL)
     {
         #ifdef _DEBUG_
           cout<<"m_inputRule("<<m_inputRuleMgr->m_inputParam[m_pathNo].m_inputPath<<") false!"
              <<__FILE__<<__LINE__<<endl;
         #endif
         return false;
     }
  }
  else
  {
      m_pathNo++;
      m_fileSum = 0;
      t_subChanName = m_inputRuleMgr->m_inputParam[m_pathNo].m_subChanName;
      m_dir.setPath(m_inputRuleMgr->m_inputParam[m_pathNo].m_inputPath.c_str());
      m_dir.setMaxScan(MAXSCANNUM);
      m_dir.scanFiles((Directory::SortFlags)m_inputRuleMgr->m_sortMode);
      m_inputParam = m_inputRuleMgr->getInputParam(t_subChanName);
      if (m_inputParam == NULL)
      {
          #ifdef _DEBUG_
            cout<<"m_inputParam("<<m_inputRuleMgr->m_inputParam[m_pathNo].m_inputPath<<") false!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          return false;
      }
  }

  return true;
}
