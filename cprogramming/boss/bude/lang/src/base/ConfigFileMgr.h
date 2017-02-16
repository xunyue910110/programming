//written by chenzm @ 2009-3-13
//代码使用示例见后
#ifndef __CONFIGFILE_MGR_H_20090311__
#define __CONFIGFILE_MGR_H_20090311__
#include "base/SysParam.h"
#include "base/Exception.h"
#include <algorithm>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

class ConfigFileMgr
{
	public:
		map<string,string> m_keyValues;//当前节点实际保存的数据，<Key,Value>
		map<string,ConfigFileMgr*> m_sections;//<子节点名,管理子节点的对象ConfigFileMgr指针>		
	public:
		ConfigFileMgr():m_pCurrentSection(this){}
		~ConfigFileMgr(){this->clear();}
		void clear();
		
		//如果是相对路径，则以 $WORK_HOME/etc为基准目录
		//不传入文件名，默认为 $WORK_HOME/etc/boss.conf (即全部载入，包括子配置文件)
		//使用绝对路径（以\开头）来指定任意配置文件
		bool load(const string& r_cfg = "");//为空则为 boss.conf
	    
	    // 设置当前节
	    bool setSectionPath(const string& sectionPath);
		
		//根据key，获取对应的value值
		const string& getValue(const string& r_keyName);
		//根据key，获取对应的value值,功能同getValue
		const string& operator[](const string& r_keyName);
		
		//直接返回存储key，value的map指针，效率高
		map<string,string>* getKeyValuesPtr();
		//复制一份key，value到r_keyValues中
		void getKeyValues(map<string,string>& r_keyValues);
		
		//返回当前节点下的所有子节点名称
		void getSubSections(vector<string> & r_vSectionName);
		//直接返回存储子节点名称以及管理对象的map，效率高
		map<string,ConfigFileMgr*> * getSubSectionsPtr();
		
		//根据子节点名称返回对应的ConfigFileMgr类
		ConfigFileMgr& getSubSection(const string& sectionName);
		//根据子节点名称返回对应的ConfigFileMgr类,功能同getSubSection
		ConfigFileMgr& operator()(const string& sectionName);
		//返回配置文件的具体内容，如调用了setSectionPath，则从当前节点开始的所有内容（包括子节点）
		string getDumpInfo(int indent=0);
		//将以split分割的src字符，解析到vector中
		bool parseString(const string& src,char split,vector<string>& result);
	private:
		typedef enum LINE_TYPE{TYPE_BEGIN,TYPE_LINE,TYPE_END} LineType;
		typedef struct {int start;int end;} SectionPosDefine;
		typedef struct {int pos;string name;} SectionDef;		
		ConfigFileMgr::LineType getLineType(const string& src);
		string lTrim(const string& src);//去除左 空格、tab、回车、换行
		string rTrim(const string &src);//去除右 空格、tab、回车、换行
		bool fileExists(const string& r_fileName);//判断文件是否存在
		long getFileSize(const string& r_fileName);//获取文件大小
		//配置文件载入动态分配的内存
		bool loadFileToBuffer(const string& r_file,char *r_buf,long r_size);
		//将内存中的内容解析为一行一行的记录，保存到vector<string>中
		bool bufferToLines(char *r_buf,long r_size,vector<string> & r_lines);
		//判断是否 空格、tab、回车、换行
		bool isBlank(char c){return c == ' ' || c == '\t' || c == '\r' || c == '\n';}
		//从 r_lines 中获取所有的宏定义，保存到 r_macroDefine中，并删除 r_lines中的定义
		bool getMacroDefine(vector<string>& r_lines,map<string,string>& r_macroDefine);
		//用r_macroDefine中的宏定义替换 r_lines中的宏定义
		bool applyMacroDefine(vector<string>& r_lines,map<string,string>& r_macroDefine);
		//获得src中 宏定义的开始位置和长度
		bool getMacroStartLen(const string& src,int& start,int& len);
		//解析一行记录(key = value)为key，value
		bool getKeyValueFromLine(const string& r_line,string& r_key,string& r_value);
		//实际载入的控制逻辑
		bool load(const string& r_cfg,vector<string>& r_lines,map<string,string>& r_macroDefine);
		//递归解析字符创到m_keyValues，m_sessions
		bool parse(vector<string>& r_lines,int iStart,int iEnd);
		//将\section\section1\...解析为 section,section1到vector中
		void parseSections(const string& r_path,vector<string>& r_sections);
		//返回当前 key/value map中key的最大长度
		int getMaxKeyLen();
		//返回icount个空格组成的string对象
		string getBlankStr(int count);
		
	private:
		string m_configFileName;//配置文件
		//m_lines,m_macroDefine只是在load方法中使用，调用完毕即清空了
		vector<string>     m_lines;      //保存宏定义
		map<string,string> m_macroDefine;//保存除宏定义外的其余部分，注释已经剔除

		string m_currentSectionPath;//当前路径
		ConfigFileMgr * m_pCurrentSection;//当前section，默认\,调用setSectionPath后会改变
		
		//以下临时变量只是为了提高解析的效率
		vector<string> m_tmpSectionNames;//临时变量，不需要关注
		string         m_tmpSection;     //临时变量，不需要关注
		string         m_tmpStr;         //临时变量，不需要关注
};
#endif

/************ 代码使用示例 ***************************/
/******************************************************
ConfigFileMgr类是为了解析公司普遍使用的配置文件的一个类。
现在使用的类是SysParam类。SysParam类为了一些特殊的用途，牺牲了很多性能。
ConfigFileMgr类将所有相关的配置文件一次性的全部载入内存，并解析到map中保存，所以效率较高。
也可指定特定的配置文件（和boss.conf没有关系）

配置文件举例:
<cdr_bill_check>
	<ignore_bill_id>
	    ignore_bill_id = 18000 
	</ignore_bill_id>
<db_channel_no>
ORACLE_THREAD_COUNT = 20 
	<0025>
		1 = 1,5
		<oracle>
			<1>
				user   = UCR_NJ1
				passwd = dcydt86a
				server = NGBIL
			</1>
</oracle>
</0025>
</db_channel_no>
</cdr_bill_check>

使用代码示例
#include "ConfigFileMgr.h"

{
	ConfigFileMgr t_conMgr;
	t_conMgr.load();//默认载入boss.conf,也可指定特定的配置文件
	
	//访问oracle的连接配置：user/passwd/server
	t_conMgr.setSectionPath("\\ cdr_bill_check\\db_channel_no\\0025\\oracle\\1");
	cout<<"user  ="<< t_conMgr["user"]<<endl;
	cout<<"passwd="<< t_conMgr["passwd"]<<endl;
	cout<<"server="<< t_conMgr["server"]<<endl;
	
	//一次获取当前节点下所有的Key/Value定义
	map<string,string> t_mKeyValues;
	t_conMgr.getKeyValues(t_mKeyValues);
	
	//获取当前节点下所有子节点名称
	vector<string> t_vSections;
	t_conMgr.getSubSections(t_vSections);

	//访问任意位置元素的方法
	//只有设置为根目录才可以访问任意位置的元素
	//调用setSectionPath后，只能访问当前路径及以后子节点中的元素
	//load()方法调用后当前路径就是根目录”\\”
	t_conMgr.setSectionPath("\\"); 
	//比如要输出 ignore_bill_id 以及ORACLE_THREAD_COUNT
	//“()”内的是section(节点)的名字，“[]”内是键名
	cout<<t_conMgr("cdr_bill_check")("ignore_bill_id")["ignore_bill_id"]<<endl;
	cout<<t_conMgr("cdr_bill_check")("db_channel_no")["ORACLE_THREAD_COUNT"]<<endl;
}

*******************************************************/