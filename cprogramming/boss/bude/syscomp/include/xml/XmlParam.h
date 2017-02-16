/*
* Copyright (c) 2003,Linkage
* All rights reserved.
*
*@文件说明:XML参数处理
*@文件标识:暂无
*@摘    要:求XML配置参数
*
*@当前版本:1.0
*@作    者:wang xiaoxin <wangxx@lianchuang.com>
*@完成日期:2004年
*@修改说明:

*/

#ifndef XmlParam_H_INCLUDED
#define XmlParam_H_INCLUDED

#include "base/config-all.h"
#include "xml/XmlApi.h"

#include <map>
#include <string>
#include <fstream>
#include <vector>

USING_NAMESPACE(std)

///路径的分隔符
const char SECTDELIM = '\\';
typedef multimap<string, string> ResultMap;
typedef vector<string> ResultVector;
    
class XmlParam
{
public:
    XmlParam();
    virtual ~XmlParam();    
    
public:
    ///初始化参数配置文件名(框架中调用);框架中需传入配置文件名
    bool Initialization(char *filename);

    ///将参数从内存中释放(框架中调用)
    void CloseSection();
    
    ///根据输入求参数值:输入节名,输出multimap<参数名,参数值>
    bool GetValue(const string& sectionPath,  ResultMap& value);

    ///根据输入求参数值:输入节名和参数名(如p1,p2,p3),输出vector<参数值>
    bool GetValue(const string& sectionPath, const string& name,
                  ResultVector& value);
private:

    XmlApi xmlApi;
    string m_filename;
    string m_sectionPath;

};

#endif // XmlParam_H_INCLUDED
