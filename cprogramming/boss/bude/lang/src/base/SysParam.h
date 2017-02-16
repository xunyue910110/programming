/* Copyright (c) 2003-2005,Linkage All rights reserved.*/

#ifndef __SYSPARAM_H__
#define __SYSPARAM_H__

#include "config-all.h"

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <set>

USING_NAMESPACE(std)



///路径的分隔符
const char SECTDELIM = '\\';
typedef map<string, string> ResultMap;
typedef vector<string> ResultVector;


///
///\class SysParam 
///
///\brief 系统配置处理类.
///
///
///
/// 系统配置处理
/// 配置项包含:节，参数名，参数值
/// 节可以包含多个参数(含参数名加参数值)和子节，成为树结构
/// 
/// 配置样例:
/// <PRE>
/// <section_name>
/// name1 = value1...
/// namen = valuen
/// INCLUDE_CONFIG = /path1/path2/myconfig1.cfg
///    <subsection_name>
///    name11 = value11...
///    INCLUDE_CONFIG = /path1/path2/myconfig2.cfg
///    name13 = value13
///    </subsection_name>
/// name4 = value4
/// </section_name>
/// </PRE>
///
/// 
/// 1.引用外部配置文件
///  
/// INCLUDE_CONFIG = (带路径的配置文件名)
/// 说明:接口将配置文件中的内容放入相应节中，
///      读该参数时需要将boss.conf中的节加上该配置文件中的节作为全节名
/// 
///
/// 2.关于宏变量:
/// 
///     初始化方法被调用时<MACRO_DIFINE>节小的键值对被解释成相应的宏定义，键名既是
/// 相应的宏变量名，后面各个节中可以引用这些宏定义。读取配置信息这些宏标量被替换成
/// 相应的值。
/// 
/// 除了应用这些颞部定义的宏变量，何可以通过${@...}的形式应用环境中定义的变量。
/// 
/// 使用方法:
/// <PRE>
/// 
/// #宏定义节
/// <MACRO_DEFINE> 
///   ROOT_PATH=/bossjf/data
///   BILL_PTAH=/bossjf/data/bill
///   ORACLE_HOME=${@ORACLE_HOME}     #引用环境变量
///   ORACLE_BIN=${ORACLE_HOME}/bin   #引用另一格宏变量定义
/// </MACRO_DEFINE>
/// 
/// 
/// <account>
///   input_path = ${BILL_PATH}/sub/  #引用上面定义的宏
/// </account>
/// 
/// </PRE>
/// 


class SysParam
{
public:
    SysParam();
    ~SysParam();
    ///初始化参数配置文件名(框架中调用);框架中需传入配置文件名
    void initialize(char *filename);

    ///将参数从内存中释放(框架中调用)
    void closeSection();

    ///根据输入求参数值:输入节名,输出map<参数名,参数值>
    bool getValue(const string& sectionPath, ResultMap& value);

    ///根据输入求参数值:输入节名和参数名(如p1,p2,p3),输出vector<参数值>
    bool getValue(const string& sectionPath, const string& name,ResultVector& value);

    /// 根据节名和参数名查询参数值
    bool getValue(const string& sectionPath, const string& name,string& value);
    
    /// 取得配置相应的文件名
    string getFileName();  
   
    /// 设置当前节
    bool setSectionPath(const string& sectionPath);
	/// 取当前节下各个子节点名 withDeeperLevel 是否取出更深的节点名
    int getNextSubSection(string& subsection,bool withDeeperLevel = false );
    /// 取当前节下各个子节点名(只取次一级)
    int getSubSection(string& subsection);
    
    //add by gaojf 2008-6-6 13:04
    bool getValueFromSubFile(const string &subFileName,const string& name, string& value) const ;
    
    void dump();
private:
    struct PARAM {
        string name;
        string value;
    };
    
    string m_filename;///配置文件名
    
    string m_sectionPath;
    string m_subSectionPath;
    set<string> m_sectionDict;

    string m_currentSectionPath; ///上次取参数的节名
    string m_scanPath;///扫描时的当前路径
    fstream m_fstreamConfig;//配置文件流

    ///用于存放参数:节名,参数名
    typedef multimap<string, PARAM> ParamMap;
    typedef ParamMap::iterator ParamMapIter;
    
    typedef multimap<string, PARAM> PARAMMAP;
    typedef PARAMMAP::iterator PARAMMIter;
    ParamMap m_paramMap;

    map<string, string>  m_macroMap; /// 内部定义的宏
    bool m_needParse; ///

    enum LINE_TYPE { 
    	 LINE_SECTION, 
         LINE_COMMENT, 
         LINE_KEYVAL, 
         LINE_UNKNOWN
         };

    //去左右空格
    void TrimString(string& str);

    ///将节内的参数放入内存(取多个参数不用多次存取文件)
    bool OpenSection(const string& sectionPath);

    ///根据节名和参数名求参数值
    void GetOneValue(const string& sectionPath, const string& name,string& value);

    ///将参数内容放到内存中
    void AddValue(const string& sectionPath, const string& name,const string& value);
    
    ///替换宏变量
    bool ProcessMacro(string & value);

    ///求节名和父节名
    void ExtractSectionPath(const string& pathFull, string& pathParent,string& sectionSub);

    ///对节进行遍历
    bool ScanSection(fstream &conFile);

    ///定位到指定节
    bool Locate(const string& sectionPath);

    ///求节名
    bool SectionLineProcess(const string& line, string& section, bool &isBegin);

    ///求参数值
    bool KeyLineProcess(const string& line, string& key, string& value);

    ///根据折行标记'\\'求完整参数
    bool GetWholeParam(fstream &conFile, string& value);

    ///清除注释
    void CleanLineComments(string& line);

    ///求该配置行属性(节、注释、参数、未知)
    LINE_TYPE LineType(const string& line);

    ///定位到节结束处
    bool ToSectionEnd(const string& section);
    
};
#endif // 


