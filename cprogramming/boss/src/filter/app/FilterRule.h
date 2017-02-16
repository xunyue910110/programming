#ifndef FILTERRULE_CODE_H_20080709_
#define FILTERRULE_CODE_H_20080709_

#include "cdr/CdrFormat.h"
#include <map>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include "OutChanInfo.h"
#include "FilterCdr.h"
#include "FilterError.h"

class SysParam;
class FilterConfig;
//class FilterCdr;
class FilterRuleMgr
{
  public:
    static bool setCdrFormat(const PString & r_formatName,
                             const vector<CdrFormat> &r_cdrFormats,
                             const CdrFormat  *      &r_cdrFmt
                             );
    static bool isPath(const PString &r_pathName);
};


////////输入规则
class InputRuleMgr:public FilterRuleMgr
{
  public:
    class InputParam
    {
      public:
        PString           m_inputPath;     ///输入路径
        PString           m_inputPathBack; ///输入备份路径
        int               m_fieldCount;    ///输入cdr中的字段数,用于校验
        char              m_separate;      ///输入cdr中字段分隔符
        PString           m_fileType;      ///数据类型 临时量
        PString           m_inFormatName;  ///话单格式名称 临时量
        PString           m_subChanName;   ///子进程名称 临时量 channel_1 channel_2
        const CdrFormat  *m_pFormat;       ///话单结构
        vector<PString>   m_procName;      ///处理过程名
        int               m_bindmethod;    ///绑定资料方式
      public:
        InputParam(){
                      m_inputPath = "";
                      m_inputPathBack = "";
                      m_fieldCount = 0;
                      m_separate = ',';
                      m_fileType = "";
                      m_inFormatName = "";
                      m_subChanName = "";
                      m_pFormat=NULL;
                      m_procName.clear();
                      m_bindmethod = 0;
                     }
        InputParam(const InputParam &r_obj)
        {
          *this = r_obj;
        }
        void operator=(const InputParam &r_right)
        {
          m_inputPath    = r_right.m_inputPath;
          m_inputPathBack= r_right.m_inputPathBack;
          m_fieldCount   = r_right.m_fieldCount;
          m_separate     = r_right.m_separate;
          m_fileType     = r_right.m_fileType;
          m_inFormatName = r_right.m_inFormatName;
          m_subChanName  = r_right.m_subChanName;
          m_pFormat      = r_right.m_pFormat;
          m_procName     = r_right.m_procName;
          m_bindmethod   = r_right.m_bindmethod;
        }
        friend bool operator < (const InputParam &r_left ,const InputParam &r_right) 
        {
          return (strcasecmp(r_left.m_subChanName.c_str(),r_right.m_subChanName.c_str())<0);
        }
    };

    public:
    char              m_isNeeduseMdb;  ///连接内存数据库标志 Y-需要 N-不需要
    char              m_isbackSource;  ///是否备份分拣前数据源 Y-需要 N-不需要 默认不需要
    int               m_sortMode;      ///排序方式
    PString           m_errorPath;     ///无效目录
    vector<InputParam> m_inputParam;

    //InputRule *getInputRule(const char* r_pattern,const PATTERNTYPE &r_patternType);
    bool initialize(SysParam *r_sysparam,const char* r_appName,
                    const int &r_channelNo,
                    const vector<CdrFormat> &r_cdrFormats,
                    const CdrAttribMacros &r_attribMacros,
                    FilterConfig *r_config);
    void dump() const;
    bool valid() const;//检查是否有效的目录
    //获得子通道输入参数
    InputRuleMgr::InputParam *getInputParam(const PString & r_subChanName);

  public:
    InputRuleMgr()
    {
      m_inputParam.clear();
      m_sortMode = 0;
      m_errorPath = "";
    }
    virtual ~InputRuleMgr()
    {
      m_inputParam.clear();
    }



};

////////输出规则
class OutputRuleMgr:public FilterRuleMgr
{
  public:
    class OutputParam
    {
      public:
        PString           m_outputName;        ///输出路径名 临时变量
        PString           m_fileType;          ///数据类型 临时量
        PString           m_defaul_outputName; ///默认输出路径名 临时量
        PString           m_prefixName;        ///前缀名称 临时量
        PString           m_expressionName;    ///表达式名称 临时量
        PString           m_subChanName;       ///子进程名称 临时量 channel_1 channel_2
        PString           m_outputFormatName;      ///输出话单格式名称 临时量
        const CdrFormat     *m_bFormat;           ///输出话单结构
        vector<OutChanInfo>  m_outChanInfo;       ///输出话单通道名 路径 前缀 表达式

      public:
        OutputParam(){
                      m_outputName = "";
                      m_fileType = "";
                      m_defaul_outputName = "";
                      m_prefixName = "";
                      m_expressionName = "";
                      m_subChanName = "";
                      m_outputFormatName = "";
                      m_bFormat=NULL;
                      m_outChanInfo.clear();
                     }
        OutputParam(const OutputParam &r_obj)
        {
          *this = r_obj;
        }
        void operator=(const OutputParam &r_right)
        {
          m_outputName        = r_right.m_outputName;
          m_fileType          = r_right.m_fileType;
          m_defaul_outputName = r_right.m_defaul_outputName;
          m_prefixName        = r_right.m_prefixName;
          m_expressionName    = r_right.m_expressionName;
          m_subChanName       = r_right.m_subChanName;
          m_outputFormatName  = r_right.m_outputFormatName;
          m_bFormat           = r_right.m_bFormat;
          m_outChanInfo       = r_right.m_outChanInfo;
        }
        friend bool operator < (const OutputParam &r_left ,const OutputParam &r_right) 
        {
          return (strcasecmp(r_left.m_subChanName.c_str(),r_right.m_subChanName.c_str())<0);
        }
    };

    public:
    vector<OutputParam> m_outputParam;

    bool initialize(SysParam *r_sysparam,const char* r_appName,
                    const int &r_channelNo,
                    const vector<CdrFormat> &r_cdrFormats,
                    const CdrAttribMacros &r_attribMacros,
                    FilterConfig *r_config);
    void dump() const;
    bool valid() const; //检查是否有效的目录
    //获得子通道输出参数
    OutputRuleMgr::OutputParam *getOutputParam(const PString & r_subChanName);
    //获得输出管理类中所有的输出个数 channel_1:m_outChanInfo.size()+channel_2:m_outChanInfo.size()+...
    int getallOutChannelNumber();

  public:
    OutputRuleMgr()
    {
      m_outputParam.clear();
    }
    virtual ~OutputRuleMgr()
    {
      m_outputParam.clear();
    }
};


////////排重字段下标定义

//排重字段提取规则
class DividRule
{
  public:
    class DividParam
    {
      public:
	      int               m_fieldId;     //排重字段下标
	      int               m_pos;         //字段起始位置
	      int               m_len;         //字段获取长度
	      int               m_cdrid;       //CDR内部属性下标
	      string            m_fixstr;      //固定字符串
      public:
        DividParam(){
                      m_fieldId = 0;
                      m_pos = 0;
                      m_len = 0;
                      m_cdrid = 0;
                      m_fixstr = "";
                     }
        DividParam(const DividParam &r_obj)
        {
          *this = r_obj;
        }
        void operator=(const DividParam &r_right)
        {
          m_fieldId  = r_right.m_fieldId;
          m_pos      = r_right.m_pos;
          m_len      = r_right.m_len;
          m_cdrid    = r_right.m_cdrid;
          m_fixstr   = r_right.m_fixstr;
        }
        friend bool operator < (const DividParam &r_left ,const DividParam &r_right) 
        {
          if(r_left.m_fieldId < r_right.m_fieldId)
          {
             return true;
          }
          return false;
        }
    };

  public:
    class VectorDividParam
    {
      public:
        PString m_ruleName;
        PString m_fidName;
        vector<DividParam> m_DividParamList;
        //change fid to vector DividParam
        CDRJScript *m_DividRule;
        string m_reWrite;  //add by xiangbin at 2011-4-6 19:44:33  Y:覆盖填写FID N:FID+组合的排重字段 默认覆盖方式

        bool initialize(const CdrFormat * &r_cdrFormats);
      public:
        VectorDividParam()
        {
          m_ruleName = "";
          m_fidName = "";
          m_DividParamList.clear();
        }
        virtual ~VectorDividParam()
        {
          m_DividParamList.clear();
        }
        void operator=(const VectorDividParam &r_right)
        {
          m_ruleName    = r_right.m_ruleName;
          m_fidName     = r_right.m_fidName;
          m_DividParamList  = r_right.m_DividParamList;
        }

    };

    public:
    PString m_SourceName;           //数据源名称
    vector<PString> m_sourceType;   //排重数据源
    int m_maxLength;                //最大排重字段长度
    PString m_rrFormatName;         ///话单格式名称 临时量
    const CdrFormat  *m_rrFormat;   ///话单结构
    vector<VectorDividParam> m_vectorDividParamList;
    bool initialize(SysParam *r_sysparam,const char* r_appName,
								    const string & r_subPath,
								    const vector<CdrFormat> &rt_cdrFormats,
								    const CdrAttribMacros &rt_attribMacros,
								    FilterConfig *rs_config);
    void dump() const;
    //根据规则获得具体要提取的排重字段配置(按条、按天、包天、包月)
    DividRule::VectorDividParam *getVectorDividParamRule(FilterCdr *r_cdr);

  public:
    DividRule()
    {
      m_sourceType.clear();
      m_vectorDividParamList.clear();
    }
    virtual ~DividRule()
    {
      m_sourceType.clear();
      m_vectorDividParamList.clear();
    }
    void operator=(const DividRule &r_right)
    {
      m_sourceType        = r_right.m_sourceType;
      m_maxLength         = r_right.m_maxLength;
      m_SourceName       = r_right.m_SourceName;
      m_vectorDividParamList  = r_right.m_vectorDividParamList;
    }
};

class DividRuleMgr
{
  public:
    vector<DividRule>  m_dividRuleList;
    bool initialize(SysParam *r_sysparam,const char* r_appName,
								    const vector<CdrFormat> &r_cdrFormats,
								    const CdrAttribMacros &r_attribMacros,
								    FilterConfig *r_config);
    //根据cdr中的数据源类型获得数据源排重规则(数据源大类)
    DividRule *getDividRule(const PString & r_sourceType);
    void dump() const;
};

////////通道提取规则
class ChannelRule
{
  public:
    class ChannelParam
    {
      public:
	      PString           m_name;      //字段名
	      int               m_id;        //对应内部的属性下标编号
      public:
        ChannelParam(){
                        m_name = "";
                        m_id = 0;
                      }
        ChannelParam(const ChannelParam &r_obj)
        {
          *this = r_obj;
        }
        void operator=(const ChannelParam &r_right)
        {
          m_name  = r_right.m_name;
          m_id    = r_right.m_id;
        }
    };
  public:
    PString               m_biz_type;     //数据源 支持多位 modify by shenglj 2009-03-29 02:47:00
    vector<ChannelParam>  m_channelparam;
  public:
    ChannelRule()
    {
      m_biz_type = "0";
      m_channelparam.clear();
    }
    ChannelRule(const ChannelRule &r_obj)
    {
      *this=r_obj;
    }
    virtual ~ChannelRule()
    {
      m_channelparam.clear();
    }
    void operator=(const ChannelRule &r_right)
    {
      m_biz_type=r_right.m_biz_type;
      m_channelparam=r_right.m_channelparam;
    }
    bool initialize(SysParam *r_sysparam,const char* r_appName,const string & r_subPath,const CdrAttribMacros &rt_attribMacros);
    void dump() const;
};

class ChannelRuleMgr
{
  public:
    vector<ChannelRule>  m_channelRuleList;
    bool initialize(SysParam *r_sysparam,const char* r_appName,const CdrAttribMacros &r_attribMacros);
		//根据cdr中的数据源类型获得数据源排重规则(数据源大类)
    ChannelRule *getChannelRule(const PString & r_BizType);
    void dump() const;
};



#endif //FILTERRULE_CODE_H_20080709_
