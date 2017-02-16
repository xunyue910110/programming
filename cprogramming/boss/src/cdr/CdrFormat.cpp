#include "CdrFormat.h"
#include "base/SysParam.h"
#include <algorithm>
#include <iostream>
#include "base/StringUtil.h"
#include "AttrIdConstDef.h"

bool CdrAttribMacros::getSysParam(SysParam *r_sysparam,
                                  const char * r_path)
{
  m_attribMacros.clear();
  //取所有宏定义信息
  map<string,string> t_attridefs;
  if(r_sysparam->getValue(r_path,t_attridefs)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysparam->getValue("<<r_path<<",t_attridefs) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  AttribMacro t_attribMacro;
  for(map<string,string>::iterator t_itr=t_attridefs.begin();
      t_itr!=t_attridefs.end();++t_itr)
  {
    t_attribMacro.m_name = t_itr->first.c_str();
    t_attribMacro.m_value= t_itr->second.c_str();
    m_attribMacros.push_back(t_attribMacro);
  }
  ::sort(m_attribMacros.begin(),m_attribMacros.end());
  return true;
}
bool CdrAttribMacros::getMacroValue(const PString &r_macroName,
                                    PString &r_value) const
{
  AttribMacro t_attribMacro;
  vector<AttribMacro>::const_iterator t_itr;
  t_attribMacro.m_name = r_macroName;
  t_itr=::lower_bound(m_attribMacros.begin(),m_attribMacros.end(),
                      t_attribMacro);
  if(t_itr==m_attribMacros.end()) return false;
  if(*t_itr == t_attribMacro)
  {
    r_value = t_itr->m_value;
    return true;
  }else
  {
    return false;
  }
}
void CdrAttribMacros::dump() const
{
  cout<<"-------------------------------------------"<<endl;
  for(vector<AttribMacro>::const_iterator t_itr=m_attribMacros.begin();
      t_itr!=m_attribMacros.end();++t_itr)
  {
    cout<<t_itr->m_name<<" = "<<t_itr->m_value<<endl;
  }
  cout<<"-------------------------------------------"<<endl;
}
FieldFormat::FieldFormat()
{
  m_idType=FLD_IDTYPE_NOMAL;
}
FieldFormat::FieldFormat(const FieldFormat &r_obj)
{
  *this = r_obj;
}
FieldFormat::~FieldFormat()
{
}

void FieldFormat::operator=(const FieldFormat &r_right)
{
  m_name     = r_right.m_name;
  m_id       = r_right.m_id;
  m_idType   = r_right.m_idType;
  m_position = r_right.m_position;
  m_default  = r_right.m_default;
}
//r_format的格式为：字段名 属性ID,Position,defaultValue
bool FieldFormat::initialize(const char *r_name,const PString &r_format)
{
  vector<PString> t_values;
  PString         t_format;
  t_format = r_format;
  int t_fNum=t_format.split(',',t_values,true);
  if (t_fNum < 2)
  {
    #ifdef _DEBUG_
      cout<<"Field define Format false:"<<t_format<<"! "
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_name    = r_name;
  m_id      = atoi(t_values[0].c_str());
  if(m_id >= ATTRI_SPLONG_MIN &&
     m_id <= ATTRI_SPLONG_MAX)
  {
    m_idType = FLD_IDTYPE_SPECIAL;
  }else if(m_id>=ATTRI_ORG_MIN &&
           m_id<=ATTRI_ORG_MAX)
  {
    m_idType = FLD_IDTYPE_ORG;
  }else if(m_id>ATTRI_ORG_MAX)
  {
    m_idType = FLD_IDTYPE_SUM;
  }else
  {
    m_idType = FLD_IDTYPE_NOMAL;
  }
    
  m_position= atoi(t_values[1].c_str());
  if(t_fNum>2)
  {
    m_default =t_values[2].c_str();
  }else
  {
    m_default = "";
  }
  if(m_id<=0 || m_position<=0)
  {
    #ifdef _DEBUG_
      cout<<"Field define Format false:"<<r_format<<"! "
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}
void FieldFormat::dump() const
{
   cout << "\t" << m_name << "="
        << " m_id :"        << m_id
        << " m_idType :"     <<m_idType
        << " m_position :"  << m_position
        << " m_default :"   << m_default
        << endl;
}

void CdrFormat::operator=(const CdrFormat &r_right)
{
  m_formatName=r_right.m_formatName;
  m_fields = r_right.m_fields;
  m_fieldDeliminater= r_right.m_fieldDeliminater;
  m_lineDeliminater = r_right.m_lineDeliminater;
  m_storeType       = r_right.m_storeType;
}
/**
 * getSysParam 从配置文件中获取一个话单格式定义.
 * @param  r_sysparam : 配置文件管理对象指针
 * @param  r_formatPath：要获取的话单格式根路径
 *                       ...cdrFormat
 * @param  r_formatName: 话单格式名称GSM_LOCAL/GSM_ROAMOUT等等
 * @param  r_attribMacros:宏定义列表
 * @return true: 成功, false: 失败
 */
bool CdrFormat::getSysParam(SysParam   *r_sysparam,
                            const char *r_formatPath,
                            const char *r_formatName,
                            CdrAttribMacros &r_attribMacros)
{
  m_formatName = r_formatName;
  string  t_rootpath=r_formatPath;
  map<string,string> t_params;
  FieldFormat        t_fieldFmt;
  PString            t_fieldFmtValue;
  if(r_sysparam->getValue(t_rootpath,t_params)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysparam->getValue("<<t_rootpath<<",t_params) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_fields.clear();
  for(map<string,string>::iterator t_itr=t_params.begin();
      t_itr!=t_params.end();t_itr++)
  {
    if(strcasecmp(t_itr->first.c_str(),"FILE_FORMAT")==0)
    {
      ///////////////////////////////////////////////////
      //定义文件级格式
    }else
    {
      if(parseAttribMacro(t_itr->second,r_attribMacros)==false)
      {
        #ifdef _DEBUG_
          cout<<"Field Format def Error:{"<<t_itr->second<<"}!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        return false;
      }
      t_fieldFmtValue=t_itr->second.c_str();
      //处理second
      if(t_fieldFmt.initialize(t_itr->first.c_str(),t_fieldFmtValue)==false)
      {
        return false;
      }
      m_fields.push_back(t_fieldFmt);
    }
  }
  ::sort(m_fields.begin(),m_fields.end());
  return true;
}
bool CdrFormat::parseAttribMacro(string & r_str,const CdrAttribMacros &r_attribMacros) 
{
  size_t t_spos=0;
  size_t t_epos=0;
   
  vector<string> t_keywords;
  vector<string>::reverse_iterator t_strItr;
  size_t t_keyword_len;
  t_spos = r_str.find("ATTRI_",t_spos);
  while (t_spos != string::npos) 
  {
    t_epos = r_str.find_first_not_of("1234567890"
               "abcdefghijklmnopqrstuvwxyz"
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ_",t_spos + 6);
    if (t_epos == string::npos) 
    {
      t_keyword_len = strlen(r_str.c_str()+t_spos);
    } else 
    {
      t_keyword_len = t_epos - t_spos;
    }
    t_keywords.push_back(r_str.substr(t_spos,t_keyword_len));
    t_spos = r_str.find("ATTRI_",t_spos + t_keyword_len );
  } 

  ::sort(t_keywords.begin(),t_keywords.end());

  PString t_name,t_value;
  string  t_strValue;
  //for (t_strItr=t_keywords.rbegin();t_strItr!=t_keywords.rend();++t_strItr) 
  //{
  //  t_name=t_strItr->c_str();
  //  if(r_attribMacros.getMacroValue(t_name,t_value)==false)
  //  {
  //    #ifdef _DEBUG_
  //      cout<<"parseAttribMacro("<<r_str<<",...) false!"
  //          <<__FILE__<<__LINE__<<endl;
  //    #endif
  //    return false;
  //  }
  //  t_strValue = t_value.c_str();
  //  StringUtil::replace(r_str,*t_strItr,t_strValue);
  //}
  //for (t_strItr=t_keywords.begin();t_strItr!=t_keywords.end();++t_strItr) 
  int t_sizeOfKeys=t_keywords.size();
  for (int i=t_sizeOfKeys-1;i>=0;--i)
  {
    //t_name=t_strItr->c_str();
    t_name=t_keywords[i].c_str();
    if(r_attribMacros.getMacroValue(t_name,t_value)==false)
    {
      #ifdef _DEBUG_
        cout<<"parseAttribMacro("<<r_str<<",...) false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_strValue = t_value.c_str();
    StringUtil::replace(r_str,t_keywords[i],t_strValue);
  }
   return true;
} 

void CdrFormat::dump() const
{
  cout<<"--------CdrFormat : "<<m_formatName<<" -----------------"<<endl;
  cout<<"\tm_fieldDeliminater="<<m_fieldDeliminater<<endl;
  cout<<"\tm_lineDeliminater ="<<m_lineDeliminater<<endl;
  cout<<"\tm_storeType       ="<<m_storeType<<endl;
  for(vector<FieldFormat>::const_iterator t_itr=m_fields.begin();
      t_itr!=m_fields.end();++t_itr)
  {
    t_itr->dump();
  }
  cout<<"-------------------------------------------------------"<<endl;
}




