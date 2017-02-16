/**
*   话单格式定义配置信息.
*   提供各种话单格式定义配置信息
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-7-31
*/
#ifndef _CDRFORMAT_H_INCLUDE_20080618_
#define _CDRFORMAT_H_INCLUDE_20080618_
#include "config-all.h"
#include "base/PString.h"
#include <string>
#include <vector>


class SysParam;


//字段属性宏定义结构
class CdrAttribMacros
{
  public:
    class AttribMacro
    {
      public:
        PString m_name;
        PString m_value;
      public:
        AttribMacro(){}
        AttribMacro(const AttribMacro &r_obj)
        {
          *this=r_obj;
        }
        void operator=(const AttribMacro &r_right)
        {
          m_name=r_right.m_name;
          m_value=r_right.m_value;
        }
        friend bool operator < (const AttribMacro &r_left ,const AttribMacro &r_right) 
        {
          return (strcasecmp(r_left.m_name.c_str(),r_right.m_name.c_str())<0);
        }
        friend bool operator == (const AttribMacro &r_left ,const AttribMacro &r_right) 
        {
          return (strcasecmp(r_left.m_name.c_str(),r_right.m_name.c_str())==0);
        }
    };
    vector<AttribMacro> m_attribMacros;
  public:
    bool getSysParam(SysParam *r_sysparam,
                     const char * r_path);
    bool getMacroValue(const PString &r_macroName,PString &r_value) const;
    void dump() const;
};


//字段格式定义信息
class FieldFormat 
{
  public:
    typedef enum
    {
      FLD_IDTYPE_NOMAL   = 0, //普通
      FLD_IDTYPE_SPECIAL = 1, //特殊编号 存放超长属性值
      FLD_IDTYPE_ORG     = 2, //原始值   存放非要素属性（输入输出不变）
      FLD_IDTYPE_SUM     = 3  //帐单或总量信息
    }FLD_IDTYPE;
  public:
    FieldFormat();
    FieldFormat(const FieldFormat & r_obj);
    virtual ~FieldFormat();
    //r_format的格式为：字段名,属性ID,defaultValue
    bool initialize(const char *r_name,const PString &r_format);
    void operator=(const FieldFormat &r_right);
    friend bool operator < (const FieldFormat &r_left ,const FieldFormat &r_right) 
    {
      return (r_left.m_position < r_right.m_position);
    };

    void dump() const;
  public:
    PString  m_name;       // 字段名
    int      m_id;         // 对应内部的属性下标编号
    FLD_IDTYPE  m_idType;     // 属性ID类型：0 普通,1 特殊编号,2 原始值编号,3 总量编号
    int      m_position;   // 话单文件中对应的字段编号
    PString  m_default;    // 缺省值
};

//文件格式
class CdrFormat 
{
  public:
    PString             m_formatName;       //格式名称
    vector<FieldFormat> m_fields;           //字段列表
    char                m_fieldDeliminater; //字段间分割符
    char                m_lineDeliminater;  //记录间分割符
    int                 m_storeType;        //存储类型：内存方式、文件方式
  public:
    CdrFormat() 
    {
      m_fieldDeliminater = ',';
      m_lineDeliminater  = '\n';
      m_storeType        = 0; //缺省为文件方式
    }
    CdrFormat(const CdrFormat &r_obj)
    {
      *this = r_obj;
    }
    void operator=(const CdrFormat &r_right);
    /**
     * getSysParam 从配置文件中获取一个话单格式定义.
     * @param  r_sysparam : 配置文件管理对象指针
     * @param  r_formatPath：要获取的话单格式根路径
     *                       ...cdrFormat
     * @param  r_formatName: 话单格式名称GSM_LOCAL/GSM_ROAMOUT等等
     * @param  r_attribMacros:宏定义列表
     * @return true: 成功, false: 失败
     */
    bool getSysParam(SysParam   *r_sysparam,
                     const char *r_formatPath,
                     const char *r_formatName,
                     CdrAttribMacros &r_attribMacros);
    friend bool operator < (const CdrFormat &r_left ,const CdrFormat &r_right) 
    {
      return (strcasecmp(r_left.m_formatName.c_str(),r_right.m_formatName.c_str())<0);
    }
    friend bool operator == (const CdrFormat &r_left ,const CdrFormat &r_right) 
    {
      return (strcasecmp(r_left.m_formatName.c_str(),r_right.m_formatName.c_str())==0);
    }
    void dump() const;
  public:
    static bool parseAttribMacro(string & r_str,const CdrAttribMacros &r_attribMacros);
};

#endif //_CDRFORMAT_H_INCLUDE_20080618_
