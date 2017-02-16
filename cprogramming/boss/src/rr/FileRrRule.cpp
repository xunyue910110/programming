#include "FileRrRule.h"
#include "AbstractFieldSelector.h"
#include <iostream>

//存放所有动态申请的Selector对象
list<AbstractFieldSelector*> FileRrRule::m_selectors;
void FileRrRule::clear()
{
  for(list<AbstractFieldSelector*>::iterator r_itr=m_selectors.begin();
      r_itr!=m_selectors.end();++r_itr)
  {
    delete *r_itr;
  }
  m_selectors.clear();
}

FileRrRule::FileRrRule()
{
  m_fidSelect      =NULL;
  m_beginTimeSelect=NULL;
  m_endTimeSelect  =NULL;
  m_rrFlagSelect   =NULL;
  m_rrFlagPos      = 0;
}
FileRrRule::~FileRrRule()
{
  m_fidSelect      =NULL;
  m_beginTimeSelect=NULL;
  m_endTimeSelect  =NULL;
  m_rrFlagSelect   =NULL;
}
FileRrRule::FileRrRule(const FileRrRule &r_right)
{
  *this = r_right;
}
FileRrRule& FileRrRule::operator=(const FileRrRule &r_right)
{
  m_fidSelect        = r_right.m_fidSelect;
  m_beginTimeSelect  = r_right.m_beginTimeSelect;
  m_endTimeSelect    = r_right.m_endTimeSelect;
  m_rrFlagSelect     = r_right.m_rrFlagSelect;
  m_filePrefix       = r_right.m_filePrefix;
  m_maxFidLen        = r_right.m_maxFidLen;
  m_fileType         = r_right.m_fileType;
  m_rrType           = r_right.m_rrType;
  m_fidSelectRule    = r_right.m_fidSelectRule;
  m_beginTimeRule    = r_right.m_beginTimeRule;
  m_endTimeRule      = r_right.m_endTimeRule;
  m_rrFlagSelectRule = r_right.m_rrFlagSelectRule;
  m_fileType_merge   = r_right.m_fileType_merge;
  m_rrFlagPos        = r_right.m_rrFlagPos;
  m_delimiter        = r_right.m_delimiter;
  m_timeDuration     = r_right.m_timeDuration;
  return *this;
}

bool FileRrRule::setRrFlagPos()
{
  int t_fieldIndex,t_offset,t_length;
  if(strlen(m_rrFlagSelectRule.c_str())<=0)
  {
    #ifdef _DEBUG_
      cout<<"get RrFlag Pos false!{"<<m_rrFlagSelectRule<<"}"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(sscanf(m_rrFlagSelectRule.c_str(),"%d%*[ (]%d%*[ ,]%d" ,
            &t_fieldIndex, &t_offset, &t_length) != 3)
  {
    #ifdef _DEBUG_
      cout<<"get RrFlag Pos false!{"<<m_rrFlagSelectRule<<"}"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_rrFlagPos = t_fieldIndex-1;
  return true;
}
//根据指定规则，将话单中指定字段值更改为r_value
bool FileRrRule::updateRrFlagValue(char* r_cdrStr) const
{
  int t_i,t_len;
  char *t_p=r_cdrStr;
  char *t_p2=r_cdrStr+strlen(r_cdrStr);
  
  if(m_rrFlagPos==0)
  {
    if(r_cdrStr[0] == m_delimiter)
    {
      t_len = t_p2-t_p;
      for(t_i=t_len;t_i>=0;--t_i)
      {
        r_cdrStr[t_i+1]=r_cdrStr[t_i];
      }
    }
    r_cdrStr[0]='1';
  }
  
  for(t_i=0, t_p = r_cdrStr; t_p != t_p2; ++t_p)
  {
    if(*t_p == m_delimiter)
    {
      ++t_i;
      if(t_i == m_rrFlagPos)
      {
        if(t_p+1==t_p2 ||
           *(t_p+1)==m_delimiter)
        { //需插入一个字符
          //将t_p+1 到 t_p2 向后移一位
 //连\0一起移
          t_len = t_p2-t_p;
          for(t_i=t_len+1;t_i>0;--t_i)
          {
            *(t_p+t_i+1)=*(t_p+t_i);
          }
        }
        *(t_p+1) = '1';
        return true;
      }
    }
  }
  #ifdef _DEUBG_
    cout<<"updateRrFlagValue false! t_i="<<t_i
        <<" "<<__FILE__<<__LINE__<<endl;
  #endif
  return false;
}
  
bool operator<(const FileRrRule &r_left,const FileRrRule &r_right)
{ //按照前缀从大到小排序
  return (r_left.m_filePrefix > r_right.m_filePrefix);
}
ostream& operator<<(ostream &r_os,const FileRrRule &r_rule)
{
  r_os<<"m_filePrefix      ="<<r_rule.m_filePrefix<<endl;
  r_os<<"m_maxFidLen       ="<<r_rule.m_maxFidLen<<endl;
  r_os<<"m_fileType        ="<<r_rule.m_fileType<<endl;
  r_os<<"m_rrType          ="<<r_rule.m_rrType<<endl;
  r_os<<"m_fidSelectRule   ="<<r_rule.m_fidSelectRule<<endl;
  r_os<<"m_beginTimeRule   ="<<r_rule.m_beginTimeRule<<endl;
  r_os<<"m_endTimeRule     ="<<r_rule.m_endTimeRule<<endl;
  r_os<<"m_rrFlagSelectRule="<<r_rule.m_rrFlagSelectRule<<endl;
  r_os<<"m_fileType_merge  ="<<r_rule.m_fileType_merge<<endl;
  r_os<<"m_rrFlagPos       ="<<r_rule.m_rrFlagPos<<endl;
  r_os<<"m_delimiter       ="<<r_rule.m_delimiter<<endl;
  r_os<<"m_timeDuration    ="<<r_rule.m_timeDuration<<endl;
  return r_os;
}
