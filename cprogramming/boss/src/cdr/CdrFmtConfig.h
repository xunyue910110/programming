/**
*   话单格式定义配置信息.
*   提供各种话单格式定义配置信息
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-7-31
*/
#ifndef _CDRFMTCONFIG_H_INCLUDE_20080731_
#define _CDRFMTCONFIG_H_INCLUDE_20080731_
#include "CdrFormat.h"
#include <vector>

class SysParam;

class CdrFmtConfig
{
  public:
    CdrAttribMacros    m_attribMacros;  ///<宏定义：字段属性ATTRI_开头
    vector<CdrFormat>  m_cdrFormatList; ///<话单格式定义
  public:
    /**
     * getSysParams 读取配置信息.
     * r_sysParam 配置信息管理对象
     * r_confPath 宏定义和话单格式配置的根路径
     * @return true: 成功. false : 失败
     ******************************************
     * <cdr_attrib_define> 为宏定义配置子目录
     * <cdr_format>        为话单格式配置子目录
     */
    virtual bool getSysParam(SysParam *r_sysParam,const char *r_confPath);
    const CdrFormat* getCdrFormat(const char * r_formatName) const;
    void dump() const;
};

#endif //_CDRFMTCONFIG_H_INCLUDE_20080731_

