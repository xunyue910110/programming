/*
* Copyright (c) 2003,Linkage
* All rights reserved.
*
*@文件说明:检查配置文件的合法性(1.节开始和结束的对应关系检查)
*@文件标识:暂无
*@摘    要:求配置参数
*
*@当前版本:1.0
*@作    者:Cheng HaiDong <chenghd@lianchuang.com>
*@完成日期:2003年
*@修改说明:按编程规范修改格式，根据新增功能修改、删除部分代码
*
*/

#ifndef CheckParam_H_INCLUDED
#define CheckParam_H_INCLUDED

#include "ConfigAll.h"
#include <map>
#include <string>
#include <fstream>
#include <vector>

USING_NAMESPACE(std)

string m_filename;///配置文件名
string m_scanPath;///扫描时的当前路径
fstream m_fstreamConfig;//配置文件流

enum    LINE_TYPE { LINE_SECTION, LINE_COMMENT, LINE_KEYVAL, LINE_UNKNOWN };
enum    { LINE_BUFFER_SIZE = 256 };

//去左右空格
void TrimString(string& str);

///求节名和父节名
void ExtractSectionPath(const string& pathFull, string& pathParent,
                        string& sectionSub);

///对节进行遍历
bool ScanSection(fstream &conFile);


///求节名
bool SectionLineProcess(const string& line, string& section, bool &isBegin);

///求参数值
bool KeyLineProcess(const string& line, string& key, string& value);

///清除注释
void CleanLineComments(string& line);

///求该配置行属性(节、注释、参数、未知)
LINE_TYPE LineType(const string& line);


#endif // CheckParam_H_INCLUDED

