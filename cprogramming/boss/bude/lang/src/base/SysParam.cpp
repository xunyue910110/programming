
#include "SysParam.h"
#include "CryptUtil.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>


///路径的开始字符
const char LSECTCHAR = '<';
///路径的结束字符
const char RSECTCHAR = '>';
///路径尾的路径结束字符
const char ESECTCHAR = '/';
///参数定义字符
const char KEYVALDELIM = '=';
///注释开始字符
const char CONF_COMMENTS = '#';
///转义字符
const char TRANSCHAR = '\\';
///空格等
const string WHITE_SPACE(" \t\n\r");
///折行标识
const string CHANGE_LINE("\\");

///宏定义节
const string MACRO_DEFINE_SECTION("\\MACRO_DEFINE");
///宏变量引用标志
const char MACRO_FLAG = '$';
///宏变量开始标志
const char MACRO_BEGIN = '{';
///宏变量结束标志
const char MACRO_END = '}';
///环境宏变量标志
const char ENVIRONMENT_MACRO_FLAG = '@';


SysParam::SysParam()
{
	m_currentSectionPath = "";    
    m_filename = "";
    m_needParse = false;
}

SysParam::~SysParam()
{
}


void SysParam::initialize(char *filename)
{

    // prepare macro define table
    SysParam macro;
    macro.m_filename = filename;
    macro.getValue(MACRO_DEFINE_SECTION,m_macroMap);

    m_needParse = true;
    // interate to parse the macro variables.
    // for some marco maybe denpend on others
    map<string,string>::iterator mapItr; 
    for (mapItr = m_macroMap.begin();mapItr != m_macroMap.end(); mapItr++) {
      ProcessMacro((*mapItr).second);
    }
    macro.closeSection();
    
    m_currentSectionPath = "";    
    m_filename = filename;
}


void SysParam::closeSection()
{
    m_paramMap.clear();
    m_sectionDict.clear();
}

bool SysParam::getValue(const string& sectionPath, ResultMap& value)
{
    bool isOk = true; 

    if (m_currentSectionPath != sectionPath)
    {
        isOk = OpenSection(sectionPath);
        m_currentSectionPath = sectionPath;
    }

    if (isOk)
    {
        ParamMapIter it;
        pair<ParamMapIter, ParamMapIter> mrange;
        mrange = m_paramMap.equal_range(sectionPath);
        value.clear();
        m_currentSectionPath = sectionPath;
    
        for (it = mrange.first; it != mrange.second; ++it)
        {
            value.insert(map<string,string>::value_type((*it).second.name,
                                                        (*it).second.value));
        }
    }
    return isOk;
}

bool SysParam::getValue(const string& sectionPath, const string& name,
                        ResultVector& value)
{
    int iTemp;
    bool isOk = true;
    string strName,strValue,strTemp1,strTemp2;
    strTemp1 = name;
    value.clear();

    if (m_currentSectionPath != sectionPath)
    {
        isOk = OpenSection(sectionPath);
        m_currentSectionPath = sectionPath;
    }

    while(isOk)
    {
        iTemp = strTemp1.find_first_of(",");
        if (iTemp > 0)
        {
            strName = strTemp1.substr(0,iTemp);
            GetOneValue(sectionPath, strName, strValue);
            value.push_back(vector<string>::value_type(strValue));
            strTemp2 = strTemp1.substr(iTemp + 1);
            strTemp1 = strTemp2;
        }
        else
        {
            GetOneValue(sectionPath, strTemp1, strValue);
            value.push_back(vector<string>::value_type(strValue));
            break;
        }
    }
    return isOk;
}



bool SysParam::getValue(const string& sectionPath, const string& name,string& value)
{
	ResultVector values;
	bool ret = false;
	
	if ((ret=getValue(sectionPath,name,values)) == true ) {
		if (values.size() > 0)
		  value=values[0];
	} 
	return ret;
}


string SysParam::getFileName() 
{
   return m_filename;
}  


bool SysParam::ProcessMacro(string & value) 
{
    if (!m_needParse)
      return true;

    // test macro variable existence
    if (value.find_first_of(MACRO_FLAG,0) == string::npos) 
      return true;

    string finalValue,macro;
    char buffer[256*8];
    
    // to locate marco variable positions and parse the macros
    int pos = 0,last_pos =0 ;
    while((pos=value.find_first_of(MACRO_FLAG,pos)) != string::npos) {
        memset(buffer,0,sizeof(buffer));
        strncpy(buffer,value.c_str() + last_pos, pos-last_pos);
        finalValue += buffer; // catenate no macro string

        // parse and replace the valid macro variable
    	int pos_start= value.find_first_of(MACRO_BEGIN,pos);
        int pos_end  = value.find_first_of(MACRO_END  ,pos+1); 
        if ((pos_start != string::npos) && (pos_end != string::npos) && (pos_end > pos_start))
        {
            memset(buffer,0,sizeof(buffer));
            strncpy(buffer,value.c_str() + pos_start+1, pos_end-pos_start-1);

            // is a enviroment variable
		    if (buffer[0] == ENVIRONMENT_MACRO_FLAG) { 
		        macro=buffer+1;  
		        const char * s = getenv(macro.c_str());
		        if (s!=0)
		          finalValue += string(s);
		    // is a internal macro variable
		    } else { 
		    	macro = buffer;
		        map<string,string>::iterator itr;
                pair<map<string,string>::iterator, map<string,string>::iterator> mrange;
                mrange = m_macroMap.equal_range(string(buffer));
		        for (itr = mrange.first; itr != mrange.second; ++itr) {
    	            finalValue += (*itr).second ;
    	            break;
    	        }
            }

            pos = pos_end;
        } 

        pos ++; // advance to locate the next macro variable
        last_pos = pos;
    }

    finalValue += (value.c_str() + last_pos); // catenate the remnant strings
    value = finalValue ;

    return true;
}


void SysParam::AddValue(const string& sectionPath, const string& name,
                        const string& value)
{
    PARAM param;
    param.name = name;
    param.value = value;

    ProcessMacro(param.value);
    
    m_paramMap.insert(ParamMap::value_type(sectionPath, param));
}

void SysParam::ExtractSectionPath(const string& pathFull, string& pathParent,
                                  string& sectionSub)
{
    string::size_type pos;
    pathParent.assign("");
    sectionSub.assign("");

    if ((pos = pathFull.rfind(SECTDELIM)) != string::npos)
    {
        pathParent = pathFull.substr(0, pos);
        if (++pos < pathFull.size())
        {
            sectionSub = pathFull.substr(pos);
        }
    }
}

void SysParam::GetOneValue(const string& sectionPath, const string& name,
                           string& value)
{
    value = "";
    ParamMapIter it;
    pair<ParamMapIter, ParamMapIter> mrange;
    mrange = m_paramMap.equal_range(sectionPath);

    for (it = mrange.first; (it != mrange.second); ++it)
    {
        if ((*it).second.name == name)
        {
            value = (*it).second.value;
            break;
        }
    }
}

void SysParam::TrimString(string& str)
{
    string::size_type pos;
    pos = str.find_first_not_of(WHITE_SPACE);
    if (pos != string::npos)
    {
        str = str.substr(pos);
        pos = str.find_last_not_of(WHITE_SPACE);      
        if (pos != string::npos)
        {  
        	str = str.substr(0, pos + 1);
        }
    }
    else
    {
        str = "";
    }
}

bool SysParam::OpenSection(const string& sectionPath)
{
    bool isOk = true;
    set<string>::iterator itr;
    itr=m_sectionDict.find(sectionPath); 
    if (itr == m_sectionDict.end())
    {
        isOk = false;
        m_fstreamConfig.clear();
        m_fstreamConfig.open(m_filename.c_str(), ios::in);
        if (m_fstreamConfig.rdstate() == ios::goodbit)
        {
            if ((sectionPath == "") || (Locate(sectionPath)))
            {
                m_scanPath = sectionPath;
                isOk = ScanSection(m_fstreamConfig);
            }
        }
        m_fstreamConfig.close();
    }
    return isOk;
}




bool SysParam::ScanSection(fstream &conFile)
{
    int iTemp;
    bool isBegin, isOk = true;
    string line, section, key, value, temp;
    LINE_TYPE lineType;

    while (isOk && (getline(conFile, line)))
    {
        lineType = LineType(line);
        if (lineType == LINE_COMMENT)
            continue;

        //处理参数名及其内容
        if ((lineType == LINE_KEYVAL) && (KeyLineProcess(line, key, value)))
        {
            if (key == "INCLUDE_CONFIG")
            {
                fstream selfConfig;
                selfConfig.open(value.c_str(), ios::in);
                if (selfConfig.rdstate() == ios::goodbit)
                {
                    isOk = ScanSection(selfConfig);
                }
                selfConfig.close();
                continue;
            }
            
            //if key

            iTemp = value.size() - CHANGE_LINE.size();
            if ((iTemp > 0) && (value.substr(iTemp) == CHANGE_LINE))
            {
                temp = value.substr(0, value.size() - CHANGE_LINE.size() - 1);
                value = temp;
                GetWholeParam(conFile, value);
            }

            AddValue(m_scanPath, key, value);
        }
        else
        {
        	
            //处理配置中的路径
            if (   (lineType == LINE_SECTION)
                && (SectionLineProcess(line, section, isBegin)))
            {
            	
                if (isBegin)
                {
                    //子路径开始:递归处理子路径中的参数
                    string s1 = m_scanPath;
                    m_scanPath = s1 + SECTDELIM + section;
                    
                    isOk = ScanSection(conFile);
                    
                }
                else
                {
                	m_sectionDict.insert(m_scanPath);
                    //子路径结束:恢复上一层路径
                    string s1 = m_scanPath;
                    m_scanPath = s1.substr(0, s1.find_last_of(SECTDELIM));
                }
            }
        }
    }
    return isOk;
}




bool SysParam::GetWholeParam(fstream &conFile, string& value)
{
    string line;

    while (getline(conFile, line))
    {
        CleanLineComments(line);
        assert(!line.empty());
        TrimString(line);

        if (line.substr(line.size() - CHANGE_LINE.size()) == CHANGE_LINE)
        {
            value += line.substr(0, line.size() - CHANGE_LINE.size() - 1);
            continue;
        }
        else
        {
            value += line;
            break;
        }
    }
    return true;
}



bool SysParam::Locate(const string& sectionPath)
{
    bool isBegin, isOk = true;
    string line, section, sectionCurrent, pathCurrent, newPath;
    LINE_TYPE lineType;

    while (isOk && (isOk = (getline(m_fstreamConfig, line))))
    {
        lineType = LineType(line);
        
        if (lineType != LINE_SECTION)
            continue;

        if (!SectionLineProcess(line, section, isBegin))
        {
            isOk = false;
        }
        else
        {
            if (isBegin)
            {
                newPath = pathCurrent + SECTDELIM + section;
                if (sectionPath.find(newPath) != 0)
                {
                    isBegin = false;
                    isOk = ToSectionEnd(section);
                }
                else
                {
                    if (newPath == sectionPath)
                    {
                        break;
                    }
                    pathCurrent = newPath;
                    sectionCurrent = section;
                }
            }
            else
            {
                string pathParent;
                ExtractSectionPath(pathCurrent, pathParent, sectionCurrent);
                pathCurrent = pathParent;
                ExtractSectionPath(pathCurrent, pathParent, sectionCurrent);
            }
        }
    }
    return isOk;
}


bool SysParam::SectionLineProcess(const string& lineVal, string& section,
                                  bool &isBegin)
{
    bool isOk = true;
    string line = lineVal;

    CleanLineComments(line);
    isOk = ((!line.empty()) && (line.size() >= 3)
         && ((*line.begin() == LSECTCHAR) || (*line.rbegin() == RSECTCHAR)));

    if (isOk)
    {
        if (line.at(1) == ESECTCHAR)
        {
            section = line.substr(2, line.length() - 3);
            isBegin = false;
        }
        else
        {
            section = line.substr(1, line.length() - 2);
            isBegin = true;
        }
    }
    return isOk;
}

bool SysParam::KeyLineProcess(const string& lineVal, string& key,
                              string& value)
{
    bool isOk = true;
    string::size_type pos;
    string line = lineVal;

    CryptUtil::decrypt_pakage(line);

    CleanLineComments(line);
    assert(!line.empty());
    
    pos = line.find(KEYVALDELIM);
    if ((pos == string::npos) || ((pos + 1) > line.length()))
    {
        isOk = false;
    }
    else
    {
        key = line.substr(0, pos);
        value = line.substr(++pos);
        ProcessMacro(value);
        TrimString(key);
        TrimString(value);
    }
    return isOk;
}

void SysParam::CleanLineComments(string& line)
{
    TrimString(line);
    if (!line.empty())
    {
        int pos, iTemp;
        pos = 0;
        string strTemp;
        while(pos >= 0)//增加对转义字符的处理
        {
            iTemp = line.find_first_of(CONF_COMMENTS, pos);
            pos = iTemp;
            if (pos > 0)
            {
                if(line.at(pos - 1) == TRANSCHAR)
                {
                    strTemp = line.substr(0, pos - 1) + line.substr(pos);
                    line = strTemp;
                    continue;
                }
                else
                {
                    strTemp = line.substr(0, pos);
                    line = strTemp;
                }
            }
            break;
        }
    }
    return;
}


SysParam::LINE_TYPE SysParam::LineType(const string& lineVal)
{
    LINE_TYPE lineType = LINE_COMMENT;
    string line = lineVal;
    int pos;

    TrimString(line);
    if (line.empty())
    {
        lineType = LINE_COMMENT;
    }
    else
    {
        
        if (line.at(0) != CONF_COMMENTS)
        {
            CleanLineComments(line);

            if (   (*line.begin() != LSECTCHAR)
                && (line.find_first_of(KEYVALDELIM) != string::npos))
            {
                lineType = LINE_KEYVAL;
                
            }
            else if (   (*line.begin() == LSECTCHAR)
                     && (*line.rbegin() == RSECTCHAR)
                     && (line.size() >= 3))
            {
                lineType = LINE_SECTION;
            }
        }
    }
    return lineType;
}


bool SysParam::ToSectionEnd(const string& sectionname)
{
    bool isBegin, isOk = true;
    string line, section, sectionCurrent, sectionPath, pathParent;
    LINE_TYPE lineType;

    sectionPath += SECTDELIM + sectionname;
    sectionCurrent = sectionname;

    while (isOk && (getline(m_fstreamConfig, line)))
    {
        lineType = LineType(line);
        if (lineType != LINE_SECTION)
            continue;

        isOk = SectionLineProcess(line, section, isBegin);
        if (isOk)
        {
            if (isBegin)
            {
                sectionPath += SECTDELIM + section;
                sectionCurrent = section;
            }
            else
            {
                ExtractSectionPath(sectionPath, pathParent, sectionCurrent);
                if (pathParent.empty())
                    break;
                sectionPath = pathParent;
                ExtractSectionPath(sectionPath, pathParent, sectionCurrent);
                continue;
            }
        }
    }
    return isOk;
}


void SysParam::dump() 
{
  PARAMMIter ptr;
  cout << "PARAM MAP DUMP:" << endl;
  for (ptr = m_paramMap.begin(); ptr != m_paramMap.end(); ptr++)
  {
    cout << "<" << (*ptr).first << ">"
               << (*ptr).second.name << "::" << (*ptr).second.value << endl;
  }
  
  set<string>::iterator setItr; 
  cout <<"SECTION DUMP:" << endl;
  for (setItr = m_sectionDict.begin();setItr != m_sectionDict.end(); setItr++)
  {
    cout << "" << (*setItr) << "" << endl;
  }
  
  map<string,string>::iterator mapItr; 
  cout <<"MACRO_DEFINE:" << endl;
  for (mapItr = m_macroMap.begin();mapItr != m_macroMap.end(); mapItr++)
  {
    cout << "" << (*mapItr).first << "=" << (*mapItr).second << endl;
  }

}



// 设置当前节
bool SysParam::setSectionPath(const string& sectionPath)
{
	m_subSectionPath = sectionPath;
    m_sectionPath = sectionPath;
    
    if (  m_currentSectionPath != sectionPath ) {
        m_currentSectionPath = sectionPath; 
    }

	return true;
}

int SysParam::getSubSection(string& subsection) {
   return getNextSubSection(subsection,false); 
}

int SysParam::getNextSubSection(string& subsection,bool withDeeperLevel ) {

    bool isOk = true;
    //isOk = OpenSection(m_sectionPath);

    set<string>::iterator itr;
    
    if (isOk) {
    	isOk = false;

    	if (m_subSectionPath.size()<=0)
    	   m_subSectionPath =m_sectionPath;

        itr=m_sectionDict.find(m_subSectionPath); 

		for (;;) {       
        	if (itr != m_sectionDict.end())  itr++;
        	
        	if (itr != m_sectionDict.end())  {
        		if ((*itr).find(m_sectionPath) != string::npos) {
        			if (m_subSectionPath != (*itr)) {
        	          m_subSectionPath =(*itr);

        	          if (!withDeeperLevel) { 
        			 	 //discard deeper subsections
        			     if ((*itr).find_first_of(SECTDELIM,m_sectionPath.length()+1) != string::npos)
        			        continue;
        			  }
        	          const char * pos = m_subSectionPath.c_str();
        	          subsection =(m_subSectionPath.c_str() + m_sectionPath.length() +1);
        	          isOk = true;
        	          break;
        			}
        		} 
        	} else {
        		break;
        	}
		}
    }

    return isOk;
}

// 从子配置文件中读取配置信息
// 目前只实现了直接从子文件中读取配置内容的功能
// 尚未实现根据 键值 在子文件中查找配置项的功能(目前的参数 name 没有实际用途)
bool SysParam::getValueFromSubFile(const string &subFileName,const string& name, string& value) const
{
	FILE *fp;
	char cBuffer[256];
	
	
	fp = fopen(subFileName.c_str(),"r");
	if( fp == NULL)
		return false;
	
	// 读取文件信息
	memset(cBuffer,0,256);
	if ( fgets(cBuffer,256,fp)==NULL )
	{
	   fclose(fp);
	   return false;
	}	   
	fclose(fp);
	
	int i=0,j=strlen(cBuffer);
	while(cBuffer[i]==' ') i++;
	while(j>i)
	{
	  if(cBuffer[j]==' ') j--;
	}
	cBuffer[j]=0;
	value = cBuffer+i;
	
	if( value == "" )
		return false;
		
	return true;
}

