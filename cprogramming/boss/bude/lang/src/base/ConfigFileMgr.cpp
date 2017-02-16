#include "ConfigFileMgr.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

bool ConfigFileMgr::load(const string& r_cfg)
{
	if (r_cfg == ""){ //默认配置文件 boss.conf
		m_configFileName=getenv("WORK_HOME");
		m_configFileName+="/etc/boss.conf";
		m_configFileName+=r_cfg;
	}
	else //指定配置文件
	{
		if (r_cfg[0] != '/') //非绝对路径
		{
			m_configFileName=getenv("WORK_HOME");
			m_configFileName+="/etc/";
			m_configFileName+=r_cfg;
		}
		else //绝对路径
		{
			m_configFileName = r_cfg;
		}
	}
	//调用实际处理过程
	bool t_bRet=load(m_configFileName,m_lines,m_macroDefine);
	if (t_bRet)
	{
		parse(m_lines,0,m_lines.size() - 1);
		m_lines.clear();
		m_macroDefine.clear();
		
		m_pCurrentSection=this;
		m_currentSectionPath="\\";		
	}
	return t_bRet;
}
bool ConfigFileMgr::load(const string& r_cfg,vector<string>& r_lines,map<string,string>& r_macroDefine)
{
	//判断配置文件是否存在
	if (!fileExists(r_cfg))
		return false;
	//获取配置文件的总长度
	long t_fileSize=getFileSize(r_cfg);
	if (t_fileSize == -1)
		return false;
	//根据文件长度申请一块内存
	char *t_buffer=(char *)malloc(t_fileSize + 1);
	memset(t_buffer,0,t_fileSize + 1);
	//将配置文件所有内容全部载入刚分配的内存
	if (!loadFileToBuffer(r_cfg,t_buffer,t_fileSize))
	{
		free(t_buffer);
		return false;
	}
	//将内存中的内容解析为一行一行的记录，保存到vector<string>
	bufferToLines(t_buffer,t_fileSize,r_lines);
	free(t_buffer);//释放申请的内存
	//获得配置文件中的宏定义 到 map<string,string>中：key，value
	//如果不存在，则检查环境变量中是否存在，如果存在，也放入r_macroDefine中
	getMacroDefine(r_lines,r_macroDefine);
	//将配置文件中的宏定义，替换为实际值
	applyMacroDefine(r_lines,r_macroDefine);

	string key,value;//临时变量保存 INCLUDE_CONFIG 和 配置文件名

	vector<string>::iterator it=r_lines.begin();
	vector<string>::iterator itPos;
	while (1)//循环处理，直到所有配置文件处理完毕
	{
		for (; it != r_lines.end(); it++)
		{
			if (it->substr(0,14) == "INCLUDE_CONFIG")//获取一个配置文件
				break;
		}
		if (it == r_lines.end())//未找到
			break;
		getKeyValueFromLine(*it,key,value);//获得具体的配置文件名
		vector<string> t_lines;//临时存储配置文件中的信息
		if (load(value,t_lines,r_macroDefine))//递归：将配置文件中的内容载入上面定义的临时变量 t_lines
		{//载入成功
			itPos=r_lines.erase(it); //删除 INCLUDE_CONFIG = 。。。这一行
			//插入具体的配置文件内容
			r_lines.insert(itPos,t_lines.begin(),t_lines.end());
			//迭代器重新定位
			it=r_lines.begin();
		}
		else
		{
			//载入失败，则忽略
			it ++;
		}
	}	
	return true;
}
long ConfigFileMgr::getFileSize(const string& r_fileName)
{
	struct stat t_type;
	if (lstat(r_fileName.c_str(),&t_type) < 0)
		return -1;
	return t_type.st_size;
}
bool ConfigFileMgr::fileExists(const string& r_fileName)
{
	if (access(r_fileName.c_str(),0) == -1)
		return false;
	return true;
}
bool ConfigFileMgr::loadFileToBuffer(const string& r_file,char *r_buf,long r_size)
{
	FILE *fp = fopen(r_file.c_str(),"r");
	if (fp == NULL)
		return false;
	if (fread(r_buf,r_size,1,fp) != 1)
	{
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}
bool ConfigFileMgr::bufferToLines(char *r_buf,long r_size,vector<string> & r_lines)
{
	string line;
	string::size_type t_pos;
	for (int i=0;i<r_size;i++)
	{
		line += r_buf[i];

		if (r_buf[i] == '\n' || i == r_size - 1)
		{
			if (isBlank(line[0]))
				line = lTrim(line);
			if (line[0] != '#' && line[0] != ';')
			{
				t_pos = line.find('#',0);
				if (t_pos == string::npos)
				{
					//某些值中可能用“;”分割
					//t_pos = line.find(';',0);
					//if (t_pos == string::npos)
						t_pos = line.find("//",0);
				}
				if (t_pos != string::npos)
					line = line.substr(0,t_pos);
				if (isBlank(line[line.length() - 1]))
					line = rTrim(line);
				if (line.length() > 0)
					r_lines.push_back(line);
			}
			line.clear();
		}
	}
	return true;
}
string ConfigFileMgr::lTrim(const string& src)
{
    int ipos=0;
    int iLen=src.length();
    for(int i=0;i<iLen;i++)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            ipos ++;
        else
            break;
    }
    return src.substr(ipos,iLen - ipos + 1);
}
string ConfigFileMgr::rTrim(const string &src)
{
    int  i,iLen;
    iLen=src.length();
    int iCount = 0;
    for(i=iLen - 1;i>=0;i--)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            iCount ++;
        else
            break;
    }
    return src.substr(0,iLen - iCount);	
}
bool ConfigFileMgr::getMacroDefine(vector<string>& r_lines,map<string,string>& r_macroDefine)
{
	int iCount = r_lines.size();
	int iStartPos=0;
	int iEndPos=0;
	vector<string>::iterator it=r_lines.begin();
	vector<string>::iterator itBegin;
	vector<string>::iterator itEnd;

	for (int i = 0; i < iCount; i++,it++)
	{
		if (r_lines[i] == "<MACRO_DEFINE>")
		{
			itBegin = it;
			iStartPos = i;
		}
		if (r_lines[i] == "</MACRO_DEFINE>")
		{
			itEnd = it;
			itEnd++;
			iEndPos = i;
			break;
		}
	}
	if (iStartPos == iEndPos || iStartPos + 1 == iEndPos)
	{
		return false;
	}
	string key,value;
	for (int i=iStartPos + 1;i< iEndPos;i++)
	{
		if (getKeyValueFromLine(r_lines[i],key,value))
			r_macroDefine.insert(map<string,string>::value_type(key,value));
		key.clear();
		value.clear();
	}
	//r_lines.erase(itBegin,itEnd);
	return true;
}
bool ConfigFileMgr::getKeyValueFromLine(const string& r_line,string& r_key,string& r_value)
{
	string::size_type t_pos=r_line.find('=',0);
	if (t_pos == string::npos || t_pos == 0)
		return false;
	
	r_key   = rTrim(r_line.substr(0,t_pos));
	r_value = lTrim(r_line.substr(t_pos + 1,r_line.length() - t_pos));
	return true;
}

string ConfigFileMgr::getDumpInfo(int indent)
{
	string ret;
	int iKeyLen=getMaxKeyLen();
	map<string,string>::iterator it = m_pCurrentSection->m_keyValues.begin();
	for (; it != m_pCurrentSection->m_keyValues.end(); it++)
	{
		ret += getBlankStr(indent);
		ret += it->first;
		ret += getBlankStr(iKeyLen - it->first.length());
		ret += " = ";
		ret += it->second;
		ret += "\n";
	}
	map<string,ConfigFileMgr*>::iterator it0=m_pCurrentSection->m_sections.begin();
	for (; it0 != m_pCurrentSection->m_sections.end(); it0 ++)
	{
		ret += getBlankStr(indent);
		ret += "<";
		ret += it0->first;
		ret += ">\n";
		ret += it0->second->getDumpInfo(indent + 4);
		ret += getBlankStr(indent);
		ret += "</";
		ret += it0->first;
		ret += ">\n";
	}
	return ret;
}
bool ConfigFileMgr::getMacroStartLen(const string& src,int& start,int& len)
{
	string::size_type pos_start = src.find("${");
	if (pos_start != string::npos)
	{
		string::size_type pos_end;
		pos_end = src.find('}',pos_start);
		if (pos_end != string::npos)
		{
			if (pos_end - pos_start > 2)
			{
				start = pos_start;
				len = pos_end - pos_start + 1;
				return true;
			}
		}
	}
	return false;
}
bool ConfigFileMgr::applyMacroDefine(vector<string>& r_lines,map<string,string>& r_macroDefine)
{
	map<string,string>::iterator it;
	int iCount = r_lines.size();
	int iStart,iLen;
	string key,value;
	for (int i = 0; i < iCount; i++)
	{
		if (!getMacroStartLen(r_lines[i],iStart,iLen))
			continue;
		it = r_macroDefine.find(r_lines[i].substr(iStart + 2,iLen - 3));
		if (it != r_macroDefine.end())
			r_lines[i].replace(iStart,iLen,it->second);
		else
		{
			key = r_lines[i].substr(iStart + 2,iLen - 3);
			value = getenv(key.c_str());
			if (value.length() > 0)
			{
				r_macroDefine.insert(map<string,string>::value_type(key,value));
				r_lines[i].replace(iStart,iLen,value);
			}
		}
	}
	return true;
}

void ConfigFileMgr::parseSections(const string& r_path,vector<string>& r_sections)
{
	int iLen=r_path.length();
	
	m_tmpSection.clear();
	
	for (int i=1;i<iLen;i++)
	{
		if (r_path[i]!= '\\')
		{
			m_tmpSection += r_path[i];
			if (i == iLen - 1)
			{
				r_sections.push_back(m_tmpSection);
				m_tmpSection.clear();
			}
		}
		else
		{
			r_sections.push_back(m_tmpSection);
			m_tmpSection.clear();
		}
	}
}

ConfigFileMgr::LineType ConfigFileMgr::getLineType(const string& src)
{
	if (src[0]=='<' && src[src.length() - 1] == '>')
	{
		if (src[1] != '/')
			return ConfigFileMgr::TYPE_BEGIN;
		else
			return ConfigFileMgr::TYPE_END;
	}
	return ConfigFileMgr::TYPE_LINE;
}
bool ConfigFileMgr::parse(vector<string>& r_lines,int iStart,int iEnd)
{

	SectionDef sd;
	map<string,SectionPosDefine> t_mSubSections;
	//vector<int> t_vSectionPos;
	vector<SectionDef> t_vSectionPos;
	ConfigFileMgr::LineType t_type;
	int iPos;
	string section;
	string key,value;
	map<string,string>::iterator t_it;
	for (int i=iStart;i<=iEnd;i++)
	{
		t_type=getLineType(r_lines[i]);
		if (t_type == ConfigFileMgr::TYPE_BEGIN)
		{
			sd.pos = i;
			sd.name = r_lines[i].substr(1,r_lines[i].length() - 2);
			t_vSectionPos.push_back(sd);
			//t_vSectionPos.push_back(i);
		}
		else if (t_type == ConfigFileMgr::TYPE_END)
		{
			if (r_lines[i].substr(2,r_lines[i].length() - 3) != t_vSectionPos[t_vSectionPos.size() - 1].name)
			{
				for (int k=0;k<t_vSectionPos.size();k++)
				{
					cout<<"<"<<t_vSectionPos[k].name<<">"<<endl;
				}
				cout<<"------------------------"<<endl;
				printf("节开始标记 <%s> 和结束标记 %s 不匹配\n",t_vSectionPos[t_vSectionPos.size() - 1].name.c_str(),r_lines[i].c_str());
				return false;
			}
			if (t_vSectionPos.size() == 1)
			{
				SectionPosDefine node;
				//node.start=t_vSectionPos[0];
				node.start=t_vSectionPos[0].pos;
				node.end  =i;
				section = r_lines[node.start].substr(1,r_lines[node.start].length() - 2);
				t_mSubSections.insert(map<string,SectionPosDefine>::value_type(section,node));
			}
			t_vSectionPos.pop_back();
		}
		if (t_type == ConfigFileMgr::TYPE_LINE)
		{
			if (t_vSectionPos.empty())
			{
				getKeyValueFromLine(r_lines[i],key,value);
				t_it = m_keyValues.find(key);
				if (t_it == m_keyValues.end())
					m_keyValues.insert(map<string,string>::value_type(key,value));
				else
					t_it->second = value;						
			}
		}
	}
	map<string,SectionPosDefine>::iterator it = t_mSubSections.begin();
	for (; it != t_mSubSections.end(); it ++)
	{
		ConfigFileMgr * t_cfgMgr = new ConfigFileMgr;
		t_cfgMgr->parse(r_lines,it->second.start + 1,it->second.end - 1);
		m_sections.insert(map<string,ConfigFileMgr*>::value_type(it->first,t_cfgMgr));
	}
	return true;
}
void ConfigFileMgr::clear()
{
	m_currentSectionPath.clear();
	m_pCurrentSection = this;
	m_lines.clear();
	m_macroDefine.clear();
	m_keyValues.clear();
	map<string,ConfigFileMgr*>::iterator it = m_sections.begin();
	for (; it != m_sections.end(); it++)
	{
		it->second->clear();
		delete it->second;
		it->second = NULL;
	}
	m_sections.clear();
}
int ConfigFileMgr::getMaxKeyLen()
{
	int ilen=0;
	map<string,string>::iterator it = m_pCurrentSection->m_keyValues.begin();
	for (; it != m_pCurrentSection->m_keyValues.end(); it++)
	{
		if (it->first.length() > ilen)
			ilen = it->first.length();
	}
	return ilen;
}
string ConfigFileMgr::getBlankStr(int count)
{
	string ret;
	for (int i=0;i<count;i++)
		ret += " ";
	return ret;
}
bool ConfigFileMgr::setSectionPath(const string& sectionPath)
{
	m_currentSectionPath = sectionPath;
	m_tmpSectionNames.clear();
	parseSections(sectionPath,m_tmpSectionNames);
	
	m_pCurrentSection=this;
	map<string,ConfigFileMgr*>::iterator it;
	int iCount = m_tmpSectionNames.size();
	for (int i=0;i<iCount;i++)
	{
		it = m_pCurrentSection->m_sections.find(m_tmpSectionNames[i]);
		if (it == m_pCurrentSection->m_sections.end())
		{
			#ifdef _DEBUG_
			printf("无效的路径:%s, 其中section=%s 无效\n",sectionPath.c_str(),m_tmpSectionNames[i].c_str());
			#endif
			return false;
		}
		m_pCurrentSection = it->second;
	}
	return true;
}
const string& ConfigFileMgr::getValue(const string& r_keyName)
{
	map<string,string>::iterator it;
	it = m_pCurrentSection->m_keyValues.find(r_keyName);
	if (it != m_pCurrentSection->m_keyValues.end())
		return it->second;
	#ifdef _DEBUG_
	printf("无效的键值:%s\n",r_keyName.c_str());
	#endif		
	throw new Exception(-2,"无效的键值:%s",r_keyName.c_str());
}
const string& ConfigFileMgr::operator[](const string& r_keyName)
{
	return getValue(r_keyName);
}
ConfigFileMgr& ConfigFileMgr::getSubSection(const string& sectionName)
{
	map<string,ConfigFileMgr*>::iterator it;
	it = m_pCurrentSection->m_sections.find(sectionName);
	if (it != m_pCurrentSection->m_sections.end())
		return *(it->second);
	#ifdef _DEBUG_
	printf("无效的节点名称:%s\n",sectionName.c_str());
	#endif	
	throw new Exception(-2,"无效的节点名称:%s",sectionName.c_str());	
}
ConfigFileMgr& ConfigFileMgr::operator()(const string& sectionName)
{
	return getSubSection(sectionName);
}
map<string,string>* ConfigFileMgr::getKeyValuesPtr()
{
	return &(m_pCurrentSection->m_keyValues);
}
void ConfigFileMgr::getKeyValues(map<string,string>& r_keyValues)
{
	r_keyValues.clear();
	r_keyValues.insert(m_pCurrentSection->m_keyValues.begin(),m_pCurrentSection->m_keyValues.end());
}
void ConfigFileMgr::getSubSections(vector<string> & r_vSectionName)
{
	r_vSectionName.clear();
	map<string,ConfigFileMgr*>::iterator it=m_pCurrentSection->m_sections.begin();
	for (;it != m_pCurrentSection->m_sections.end();it++)
		r_vSectionName.push_back(it->first);
}
map<string,ConfigFileMgr*> * ConfigFileMgr::getSubSectionsPtr()
{
	return &(m_pCurrentSection->m_sections);
}
bool ConfigFileMgr::parseString(const string& src,char split,vector<string>& result)
{
	int iCount=src.length();
	m_tmpStr.clear();
	for (int i=0;i<iCount;i++)
	{
		if (src[i] != split)
		{
			m_tmpStr+=src[i];
			if (i == iCount - 1)
			{
				result.push_back(m_tmpStr);
				m_tmpStr.clear();
			}
		}
		else
		{
			result.push_back(m_tmpStr);
			m_tmpStr.clear();
		}
	}
	return true;
}
