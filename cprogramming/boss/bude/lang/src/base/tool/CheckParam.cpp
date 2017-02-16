#include "CheckParam.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>

///路径的分隔符
const char SECTDELIM = '\\';
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




int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout<<"Please input:CheckParam filename."<<endl;
    }
    else
    {
        m_filename = argv[1];
        m_fstreamConfig.clear();
        m_fstreamConfig.open(m_filename.c_str(), ios::in);
        if (m_fstreamConfig.rdstate() == ios::goodbit)
        {
            m_scanPath = "";
            if (ScanSection(m_fstreamConfig))
            {
                if (m_scanPath == "")
                {
                    cout<<"Check successful!!"<<endl;
                }
                else
                {
                    cout<<"There are some error on "<<m_scanPath<<endl;
                }
            }
            else
            {
                cout<<"There are some error on "<<m_scanPath<<endl;
            }
        }
        else
        {
            cout<<"Can not open file:"<<m_filename<<endl;
        }
        m_fstreamConfig.close();
    }
    return 1;
}


void ExtractSectionPath(const string& pathFull, string& pathParent,
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

void TrimString(string& str)
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

bool ScanSection(fstream &conFile)
{
    bool isBegin, isOk = true;
    char linebuf[LINE_BUFFER_SIZE];
    string line, section, key, value;
    LINE_TYPE lineType;

    while (isOk && (conFile.getline(linebuf, LINE_BUFFER_SIZE)))
    {
        line = linebuf;
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
                    //子路径结束:恢复上一层路径
                    string s1,s2 = m_scanPath;
                    s1 = s2.substr(s2.find_last_of(SECTDELIM) + 1);
                    if (s1 != section)
                    {
                        isOk = false;
                        break;
                    }
                    else
                    {
                        m_scanPath = s2.substr(0, s2.find_last_of(SECTDELIM));
                    }
                }
            }
        }
    }
    return isOk;
}

bool SectionLineProcess(const string& lineVal, string& section,
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

bool KeyLineProcess(const string& lineVal, string& key,
                              string& value)
{
    bool isOk = true;
    string::size_type pos;
    string line = lineVal;

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
        TrimString(key);
        TrimString(value);
    }
    return isOk;
}

void CleanLineComments(string& line)
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

LINE_TYPE LineType(const string& lineVal)
{
    LINE_TYPE lineType;
    string line = lineVal;

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

