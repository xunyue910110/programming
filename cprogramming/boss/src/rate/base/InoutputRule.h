#ifndef _INOUTPUT_H_INCLUDE_20080618_
#define _INOUTPUT_H_INCLUDE_20080618_
#include "cdr/CdrFormat.h"
#include <map>
#include <vector>
#include <sys/stat.h>
#include <iostream>

class RateConfig;
class InoutputRuleMgr
{
  public:
    static bool setCdrFormat(const PString & r_formatName,
                             const vector<CdrFormat> &r_cdrFormats,
                             const CdrFormat  *      &r_cdrFmt
                             );
    static bool isPath(const PString &r_pathName);
};

typedef enum 
{
  PATTERN_TYPE_FILE       = 1,
  PATTERN_TYPE_SOURCETYPE = 2
}PATTERNTYPE;

class InputRule
{
  public:
    PString           m_pattern;    //前缀匹配模式
    PATTERNTYPE       m_patternType;//模式值类型：1文件名 2SourceType
    PString           m_formatName; //话单格式名称
    const CdrFormat  *m_pFormat;    //话单结构
    map<int,PString>  m_initValues; //一些初始值定义 
  public:
    InputRule()
    {
      m_pFormat=NULL;
      m_patternType=PATTERN_TYPE_FILE;//缺省为文件方式
    }
    InputRule(const InputRule &r_obj)
    {
      *this=r_obj;
    }
    virtual ~InputRule()
    {
      m_pFormat=NULL;
      m_initValues.clear();
    }
    void operator=(const InputRule &r_right)
    {
      m_pattern     = r_right.m_pattern;
      m_patternType = r_right.m_patternType;
      m_formatName  = r_right.m_formatName;
      m_pFormat     = r_right.m_pFormat;
      m_initValues  = r_right.m_initValues;
    }
    friend bool operator < (const InputRule &r_left ,const InputRule &r_right)
    {
      if(r_left.m_patternType<r_right.m_patternType)
      {
        return true;
      }
      if(r_left.m_patternType>r_right.m_patternType)
      {
        return false;
      }
      //按照前缀匹配从大到小排列
      return (strcmp(r_left.m_pattern,r_right.m_pattern)>0);
    }
};

class InputRuleMgr:public InoutputRuleMgr
{
  public:
    class InputPath
    {
      public:
        PString           m_inputPath;     ///输入路径
        bool              m_bakFlag;       ///是否备份标记
        PString           m_inputBackPath; ///输入文件备份目录
        PString           m_invalidPath;   ///无效文件目录
        PString           m_delayedPath;   ///延迟文件目录
        int               m_sortMode;      ///排序方式
      public:
        InputPath(){m_bakFlag=false;}
        InputPath(const InputPath &r_obj)
        {
          *this = r_obj;
        }
        void operator=(const InputPath &r_right)
        {
          m_inputPath    = r_right.m_inputPath;
          m_bakFlag      = r_right.m_bakFlag;
          m_inputBackPath= r_right.m_inputBackPath;
          m_invalidPath  = r_right.m_invalidPath;
          m_delayedPath  = r_right.m_delayedPath;
          m_sortMode     = r_right.m_sortMode;
        }
        bool valid() const
        { //检查是否有效的目录
          if(InoutputRuleMgr::isPath(m_inputPath)==false ||
             InoutputRuleMgr::isPath(m_invalidPath)==false ||
             InoutputRuleMgr::isPath(m_delayedPath)==false )
          {
            cout<<"m_inputPath="<<m_inputPath<<endl;
            cout<<"m_invalidPath="<<m_invalidPath<<endl;
            cout<<"m_delayedPath="<<m_delayedPath<<endl;
            return false;
          }
          if(m_bakFlag==true && isPath(m_inputBackPath)==false)
          {
            cout<<"m_inputBackPath="<<m_inputBackPath<<endl;
            return false;
          }
          return true;
        }
    };
    vector<InputPath> m_intputPaths;
    int               m_currentPathIndex;
    vector<InputRule> m_inputRules;
    InputRule *getInputRule(const char* r_pattern,const PATTERNTYPE &r_patternType);
    bool initialize(SysParam *r_sysparam,const char* r_appName,const int &r_channelNo,
                    const CdrAttribMacros &r_attribMacros);
    //根据话单格式列表设置输入规则中的话单格式指针
    bool setCdrFormat(const vector<CdrFormat> &r_cdrFormats);
    void dump() const;
  public:
    InputRuleMgr()
    {
      m_intputPaths.clear();
      m_inputRules.clear();
      m_currentPathIndex=0;
    }
    virtual ~InputRuleMgr()
    {
      m_intputPaths.clear();
      m_inputRules.clear();
    }
};

class OutputRule
{
  public:
    PString          m_pattern;    //前缀匹配模式
    PATTERNTYPE      m_patternType;//模式值类型：1文件名 2SourceType
    PString          m_formatName; //话单格式名称
    const CdrFormat *m_pFormat;    //话单格式
  public:
    OutputRule()
    {
      m_pFormat=NULL;
      m_patternType=PATTERN_TYPE_FILE;//缺省为文件方式
    }
    virtual ~OutputRule()
    {
      m_pFormat=NULL;
    }
    OutputRule(const OutputRule &r_obj)
    {
      *this=r_obj;
    }
    void operator=(const OutputRule &r_right)
    {
      m_pattern     = r_right.m_pattern;
      m_patternType = r_right.m_patternType;
      m_formatName  = r_right.m_formatName;
      m_pFormat     = r_right.m_pFormat;
    }
    friend bool operator < (const OutputRule &r_left ,const OutputRule &r_right)
    {
      if(r_left.m_patternType<r_right.m_patternType)
      {
        return true;
      }
      if(r_left.m_patternType>r_right.m_patternType)
      {
        return false;
      }
      //按照前缀匹配从大到小排列
      return (strcmp(r_left.m_pattern,r_right.m_pattern)>0);
    }
};

class OutputRuleMgr:public InoutputRuleMgr
{
  public:
    vector<OutputRule>    m_outputRules; //普通清单输出
    vector<PString>       m_outPaths;    //普通清单输出目录
    
    PString               m_errpath;    //错单目录
    
    //账单目录
    //包月费新增目录
    PString               m_monfeeMBillPath;//包月费用明细帐单输出目录(M)
    PString               m_monfeeBBillPath;//包月费账单文件输出目录(B)
    PString               m_monfeePBillPath;//包月费SP费用清单文件输出目录(P)
    PString               m_monfeeIBillPath;//包月费ADC费用清单文件输出目录(I)
    //
    PString               m_userBillPath;   //用户账单输出目录
    PString               m_userBillPathBak;//用户账单输出备份目录
    PString               m_acctBillPath;   //帐户账单输出目录
    PString               m_acctBillPathBak;//帐户账单输出备份目录
    
    PString               m_balInfoPath;    //结余文件输出目录
    PString               m_fixTrackPath;   //固定费优惠明细轨迹
    PString               m_acctTrackPath;  //帐务优惠明细轨迹
    PString               m_otherInfoPath;  //其他信息(赠送存折、异常信息、稽核信息)
    PString               m_adjustBeforePath;//帐前调账轨迹
    
    //Added By Sunam 2009/8/8 19:20:53
    PString               m_remindInfoPath;    //免费资源提醒输出目录
    //add by cuiyw 20090915 
    PString               m_wirelessPath;   //手机上网卡传递账单给计费的目录
    //end by cuiyw 20090915
    //Added By Sunam 2009/9/29 10:06:01
    PString               m_userSumBillPath;    //免费资源提醒输出目录
    
    PString               m_drpath;     //容灾接口
    PString               m_ocspath;    //OCS接口
    PString               m_crmpath;    //CRM接口
    PString               m_creditpath; //信控接口
    //过滤信息获取格式
  public:
    bool initialize(SysParam *r_sysparam,const char* r_appName,
                    const int &r_channelNo,
                    const CdrAttribMacros &r_attribMacros,
                    RateConfig *r_config);
    //根据话单格式列表设置输入规则中的话单格式指针
    bool setCdrFormat(const vector<CdrFormat> &r_cdrFormats);
    OutputRule *getOutputRule(const char* r_pattern,const PATTERNTYPE &r_patternType);
  public:
    void dump() const;
  protected:
    bool valid() const
    {
      for(vector<PString>::const_iterator t_itr=m_outPaths.begin();
          t_itr!=m_outPaths.end();++t_itr)
      {
        if(InoutputRuleMgr::isPath(*t_itr)==false)
        {
          #ifdef _DEBUG_
            cout<<"目录错误:"<<*t_itr<<" "<<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
      }
      if(InoutputRuleMgr::isPath(m_errpath)==false ||
         InoutputRuleMgr::isPath(m_userBillPath)==false || 
         InoutputRuleMgr::isPath(m_acctBillPath)==false ||
         (!(m_userBillPathBak == "" )&&
          InoutputRuleMgr::isPath(m_userBillPathBak)==false )||
         (!(m_acctBillPathBak == "" )&&
          InoutputRuleMgr::isPath(m_acctBillPathBak)==false )||
          InoutputRuleMgr::isPath(m_balInfoPath)==false ||
          InoutputRuleMgr::isPath(m_fixTrackPath)==false ||
          InoutputRuleMgr::isPath(m_acctTrackPath)==false ||
          InoutputRuleMgr::isPath(m_otherInfoPath)==false ||
          InoutputRuleMgr::isPath(m_remindInfoPath)==false ||
          InoutputRuleMgr::isPath(m_userSumBillPath)==false ||
		  //add by cuiyw 20090915
          InoutputRuleMgr::isPath(m_wirelessPath)==false||
          //end by cuiyw 20090915
		  InoutputRuleMgr::isPath(m_adjustBeforePath)==false||
          InoutputRuleMgr::isPath(m_monfeeMBillPath)==false   ||
          InoutputRuleMgr::isPath(m_monfeeBBillPath)==false   || 
          InoutputRuleMgr::isPath(m_monfeeIBillPath)==false   ||
          InoutputRuleMgr::isPath(m_monfeePBillPath)==false  )
      /*|| 
      InoutputRuleMgr::isPath(m_crmpath)==false || 
      InoutputRuleMgr::isPath(m_creditpath)==false 
      */
      {
      	  #ifdef _DEBUG_
            cout<<"目录不存在:"<<__FILE__<<__LINE__<<endl;
          #endif
        return false;
      }
      return true;
    }

};

#endif //_INOUTPUT_H_INCLUDE_20080618_
