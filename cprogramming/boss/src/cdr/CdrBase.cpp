#include "config-all.h"
#include "CdrBase.h"
#include "CdrFormat.h"

CdrBase::CdrBase()
{
  m_attrList=NULL;
  m_orgBuff =NULL;
  m_desBuff =NULL;
  m_fldDelimListr=NULL;
  initialize(MAX_ITEM_COUNT);
}
void CdrBase::setErrorCode(const int &r_errorCode)
{
  m_errorCode = r_errorCode;
  m_attrList[ATTRI_ERROR_CODE] = m_errorCode;
}

CdrBase::~CdrBase()
{
  clear();
}
void CdrBase::clear()
{
  if(m_attrList!=NULL)
  {
    delete[] m_attrList;
    m_attrList=NULL;
  }
  if(m_orgBuff!=NULL)
  {
    delete[] m_orgBuff;
    m_orgBuff=NULL;
  }
  if(m_desBuff!=NULL)
  {
    delete[] m_desBuff;
    m_desBuff=NULL;
  }
  if(m_fldDelimListr!=NULL)
  {
    delete[] m_fldDelimListr;
    m_fldDelimListr=NULL;
  }  
}
int CdrBase::getMaxIndex()
{
  return m_maxNum;
};

bool CdrBase::initialize(const int &r_maxNum)
{
  clear();
  m_maxNum = r_maxNum;
  m_attrList   = new T_ATTRIBUTE_VALUE[m_maxNum+1];
  m_orgBufLen  = m_maxNum*sizeof(T_ATTRIBUTE_VALUE);
  m_orgBuff    = new char[m_orgBufLen+1];
  //几个超长的字段存放批价轨迹等信息
  m_desBuffLen = m_maxNum*sizeof(T_ATTRIBUTE_VALUE)+
                 1024*(ATTRI_SPLONG_MAX-ATTRI_SPLONG_MIN);
  m_desBuff    = new char[m_desBuffLen+1];;
  m_fldDelimListr = new int[m_maxNum+1];
  
  if(m_attrList==NULL || m_orgBuff==NULL || m_fldDelimListr==NULL||
     m_desBuff==NULL)
  {
    #ifdef _DEBUG_
      cout<<"CdrBase::initialize false! new mem false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  memset(m_orgBuff,0,m_orgBufLen+1);
  memset(m_desBuff,0,m_desBuffLen+1);
  return reset();
}

//读取话单前一些属性初始化
bool CdrBase::reset() 
{
  memset(m_attrList,0,sizeof(T_ATTRIBUTE_VALUE)*(m_maxNum+1));
  m_fldNum=0;
  m_errorCode=0;
  
  for(int i=0;i<ATTRI_SPLONG_MAX-ATTRI_SPLONG_MIN+1+1;++i)
  {
    m_splongFld[i].clear();
  }
  return true;
}
//读取话单后规整
bool CdrBase::canonicalize()
{
  m_errorCode = m_attrList[ATTRI_ERROR_CODE].value();
  
  return true;
}
bool CdrBase::finalize()
{
  return true;
}

T_ATTRIBUTE_VALUE& CdrBase::operator [] (const int& r_index) 
{
   return m_attrList[r_index];
};  

void  CdrBase::setStrCdr(const char * r_strCdr)
{
  if(&r_strCdr == &m_orgBuff)
  { //如果传入就是m_orgBuff本身，则不需再复制
    return;
  }else
  {
    strcpy(m_orgBuff,r_strCdr);
  }
}
bool  CdrBase::setSpLongValue(const int &r_index,const char * r_strValue)
{
  if(r_index<ATTRI_SPLONG_MIN || r_index>ATTRI_SPLONG_MAX)
  {
    return false;
  }
  int t_index = r_index-ATTRI_SPLONG_MIN;
  //modif by yangyi
  //m_splongFld[t_index] = r_strValue;
  string t_strValue = r_strValue;
  if (t_strValue.length() > 500)  
   m_splongFld[t_index] = t_strValue.substr(0,500);
  else
   m_splongFld[t_index] = t_strValue;   
  return true;
}
const char* CdrBase::getSpLongValue(const int &r_index)
{
  if(r_index<ATTRI_SPLONG_MIN || r_index>ATTRI_SPLONG_MAX)
  {
    return NULL;
  }
  int t_index = r_index-ATTRI_SPLONG_MIN;
  return m_splongFld[t_index].c_str();
}

bool  CdrBase::inputCdr(const CdrFormat *r_cdrFormat,const int &r_fldNum)
{
  int t_sOffSet,t_eOffSet,t_len;
  int t_cdrLen=strlen(m_orgBuff);
  char* t_pCh=m_orgBuff;
  
  m_fldDelimListr[0] = -1;
  m_fldNum=1;
  //将话单所有分割符位置信息取出
  for(int i=0;i<t_cdrLen;++i,t_pCh++)
  {
    if(*t_pCh==r_cdrFormat->m_fieldDeliminater)
    {
      m_fldDelimListr[m_fldNum]=i;
      ++(m_fldNum);
    }
  }
  m_fldDelimListr[m_fldNum]=t_cdrLen;
  //如果输入字段数大于0，则需要校验字段数
  if(r_fldNum>0)
  {
    if(m_fldNum!=r_fldNum)
    {
      #ifdef _DEBUG_
        cout<<"字段数错误! 标准字段数="<<r_fldNum<<" 实际字段数="<<m_fldNum
            <<" "<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  
  for(vector<FieldFormat>::const_iterator t_fldFmtItr=r_cdrFormat->m_fields.begin();
      t_fldFmtItr!=r_cdrFormat->m_fields.end();++t_fldFmtItr)
  {
    if(t_fldFmtItr->m_position > m_fldNum)
    { //字段数不够
      setErrorCode(100);
      continue;
    }
    t_sOffSet = m_fldDelimListr[(t_fldFmtItr->m_position-1)]+1,
    t_eOffSet = m_fldDelimListr[(t_fldFmtItr->m_position)];
    t_len     = t_eOffSet-t_sOffSet;
    
    switch(t_fldFmtItr->m_idType)  
    {
     case FieldFormat::FLD_IDTYPE_NOMAL  :
        m_attrList[t_fldFmtItr->m_id].setStrNValue(m_orgBuff+t_sOffSet,t_len);
        break;
     case FieldFormat::FLD_IDTYPE_SPECIAL: 
        m_splongFld[t_fldFmtItr->m_id-ATTRI_SPLONG_MIN].setStrNValue(m_orgBuff+t_sOffSet,t_len);
        break;
     case FieldFormat::FLD_IDTYPE_SUM:
        //输入总量信息 暂忽略
        break;
     case FieldFormat::FLD_IDTYPE_ORG:
     default: //作为空处理
        break;
    }
  }
  return true;
}

char *CdrBase::outputCdr(const CdrFormat *r_cdrFormat)
{
  int  t_desLen=0,t_fldLen=0;
  int  t_sOffSet,t_eOffSet;
  //1. 拼凑话单
  for(vector<FieldFormat>::const_iterator t_fdlItr=r_cdrFormat->m_fields.begin();
      t_fdlItr!=r_cdrFormat->m_fields.end();++t_fdlItr)
  {
    if(t_fdlItr!=r_cdrFormat->m_fields.begin())
    {
      m_desBuff[t_desLen]=r_cdrFormat->m_fieldDeliminater;
      ++t_desLen;
    }
    switch(t_fdlItr->m_idType)
    {
     case FieldFormat::FLD_IDTYPE_NOMAL  :
        t_fldLen = m_attrList[t_fdlItr->m_id].length();
        sprintf(m_desBuff+t_desLen,"%s",m_attrList[t_fdlItr->m_id].c_str());
        t_desLen+=t_fldLen;
        break;
     case FieldFormat::FLD_IDTYPE_ORG:
        if(t_fdlItr->m_id - ATTRI_ORG_MIN >= m_fldNum)
        {
          //作空处理
        }else
        {
          t_sOffSet = m_fldDelimListr[t_fdlItr->m_id-ATTRI_ORG_MIN]+1;
          t_eOffSet = m_fldDelimListr[t_fdlItr->m_id-ATTRI_ORG_MIN+1];
          t_fldLen  = t_eOffSet - t_sOffSet;
          memcpy(m_desBuff+t_desLen,m_orgBuff+t_sOffSet,t_fldLen);
          t_desLen+=t_fldLen;
        }
        break;
     case FieldFormat::FLD_IDTYPE_SPECIAL: 
        t_sOffSet=t_fdlItr->m_id-ATTRI_SPLONG_MIN;
        t_fldLen =m_splongFld[t_sOffSet].length();
        sprintf(m_desBuff+t_desLen,"%s",m_splongFld[t_sOffSet].c_str());
        t_desLen+=t_fldLen;
        break;
     default: //作为空处理
        break;
    }
  }
  m_desBuff[t_desLen]=r_cdrFormat->m_lineDeliminater;
  ++t_desLen;
  m_desBuff[t_desLen]=0; //最后置结束符
  return m_desBuff;
}

T_ATTRIBUTE_VALUE & PCDR::operator [] (const int& r_index) 
{
   return (get()->m_attrList[r_index]);
}; 

const T_ATTRIBUTE_VALUE &PCDR::operator [] (const int& r_index) const
{
   return (get()->m_attrList[r_index]);
}; 

//测试时，查看话单的信息    
void CdrBase::dumpCdrInfo(const CdrFormat *r_cdrFormat) const
{
  const int t_stdLen=20;
  char  t_NoStr[5+1];
  char  t_fieldValue[256+1];
  int   t_sOffSet,t_eOffSet,t_fldLen;
  for(vector<FieldFormat>::const_iterator t_fldFmtItr=r_cdrFormat->m_fields.begin();
      t_fldFmtItr!=r_cdrFormat->m_fields.end();++t_fldFmtItr)
  {
    sprintf(t_NoStr,"%03d",t_fldFmtItr->m_position);
    cout<<t_NoStr<<" "<<t_fldFmtItr->m_name;
    for(int i=strlen(t_fldFmtItr->m_name.c_str());i<t_stdLen;++i)
    {
      cout<<" ";
    }
    cout<<" = ";
    switch(t_fldFmtItr->m_idType)  
    {
     case FieldFormat::FLD_IDTYPE_NOMAL  :
        cout<<m_attrList[t_fldFmtItr->m_id];
        break;
     case FieldFormat::FLD_IDTYPE_SPECIAL: 
        cout<<m_splongFld[t_fldFmtItr->m_id-ATTRI_SPLONG_MIN];
        break;
     case FieldFormat::FLD_IDTYPE_SUM:
        //输入总量信息 暂忽略
        break;
     case FieldFormat::FLD_IDTYPE_ORG:
      if(t_fldFmtItr->m_id - ATTRI_ORG_MIN >= m_fldNum)
      {
        cout<<"";
      }else
      {
        t_sOffSet = m_fldDelimListr[t_fldFmtItr->m_id-ATTRI_ORG_MIN]+1;
        t_eOffSet = m_fldDelimListr[t_fldFmtItr->m_id-ATTRI_ORG_MIN+1];
        t_fldLen  = t_eOffSet - t_sOffSet;
        memcpy(t_fieldValue,m_orgBuff+t_sOffSet,t_fldLen);
        t_fieldValue[t_fldLen]=0;
        cout<<t_fieldValue;
      }
     default: //作为空处理
        break;
    }
    cout<<endl;
  }
}

