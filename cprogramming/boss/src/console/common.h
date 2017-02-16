#ifndef COMMON1_H
#define COMMON1_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unistd.h>
#include <string>

#include "Session.h"
#include "DataBase.h"
#include "Mdb_Exception.h"
#include "SpaceInfo.h"
#include "MemManager.h"
#include "Table.h"

using namespace std;

class Column
{
    public:
        string          m_colName;
        COLUMN_VAR_TYPE m_colType;
    public:
        Column()
        {
            m_colName = "";
            m_colType = VAR_TYPE_UNKNOW;
        }
        ~Column()
        {
            ;
        }
};


vector<string> vecHelp;
bool g_run = true;


ifstream fin;
ofstream fout;

DataBase*   m_db=NULL;
Session*    m_session=NULL;
string      m_dbName;
MemManager* m_memMgr=NULL;
bool        m_lineMode=false;
#define out cout;

//DataBase db;
//Session* session;

//错误编码定义
enum ErrNo
{
    NO_ITEM = -1

};
string toUpper(const string& str)
{
	string strRet;
	for (unsigned int i=0;i<str.length();i++)
	{
		strRet+=toupper(str[i]);
	}
	return strRet;
}
//用于返回的错误参数 
struct ErrParam
{
    string m_errInfo;
    int  m_errNo;
    int    m_errPos;
};

//用于传入db.h以及session.h中函数的参数 
//class InputParam
//{
//    ParamType    m_paramType;
//    string       m_memo;
//}; 
//vector<InputParam>

typedef void (*SingleFunc)(vector<string>& sqlParam, ErrParam& errParam);

map<string, SingleFunc> mapCmdFunc;

/*
通用函数 
*/
/*
trim字符串函数 
*/
string trim(const string& srcStr,const string& trimStr = " ")// string& destStr, 
{   
    string s = srcStr;
    
    if (s.empty())    
    {   
        return s;   
    }   
  
    s.erase(0,s.find_first_not_of(trimStr));   
    s.erase(s.find_last_not_of(trimStr) + 1);   
    
    //destStr = s;
    
    return s;   
}


/*
大小写转换
*/
string capToLower(const string& src)
{
    if(src.size())
    {    
        char* buffer;
        buffer = new char[src.size()];
        memset(buffer,0,sizeof(buffer));    
        
        strcpy(buffer, src.c_str());
        
        //转换大小写
        for(int k=0;k<src.size();k++)
        {
            buffer[k]=tolower(buffer[k]);
        }  
        
        string s = buffer;
        delete []buffer;
        return s;
    }    
    
    return "";
}
  

/*
比较两个vector<string>是否相等
*/
bool equalVecStr(const vector<string>& lVec, const vector<string>& rVec)
{
    if(lVec.size()!=rVec.size())
    {
        //cout << "return false;" << endl;
        return false;
    }
    else
    {
        string lstr,rstr;
        for(int i=0; i<lVec.size(); i++)
        {
            //转换大小写
            lstr = capToLower(lVec[i]);
            rstr = capToLower(rVec[i]);
            if(lstr!=rstr)
            {
                //cout << "return false;" << endl;
                return false;
            }
        }
        //cout << "return true;" << endl;
        return true;
    }
}


/*
分割字符串函数 
*/
int split(const string& src, const string& seperator, 
                        vector<string> &destVector)
{
    destVector.clear();
    
    string source = trim(src, seperator);;
    
    string::size_type pos, pos2;
    string tempString;
    
    while((pos=source.find_first_of(seperator)) != string::npos)
    {
        tempString = source.substr(0, pos);
        destVector.push_back(tempString);
        source.erase(0, pos+1);
        pos = source.find_first_not_of(seperator);
        source.erase(0, pos);
    }
    destVector.push_back(source);
    
    return destVector.size();
}

void merge(vector<string>& vec, string& str)
{
    str = "";
    for(vector<string>::iterator itr=vec.begin();
        itr!=vec.end(); itr++)
    {
        trim(*itr);
        str = str + (*itr) + " ";
    }    
}    

void scan(const string& str, vector<string>& vec)
{  
    vec.clear();
    string tempstr="";
    
    bool isValue = false;
    bool isBracket = false;
    
    int iBracket = 0;
    
    int i = 0, u = 0;
    
    for(i =0; i<str.size(); i++)
    {
        if(str[i]==' ')
        {
            if(tempstr!="" && (! isValue) && (! isBracket))
            {
                for (u = 0; u < tempstr.size() ; u++)
                {
                    if (tempstr[u] != ' ')  
                    {  
                        vec.push_back(tempstr);
                        break;
                    }
                }
                //cout << tempstr << endl;
                tempstr = "";
            }
            else
            {
                tempstr.append(str, i, 1);
            }    
        }
        else if(str[i]=='\'' )
        {
            if (isValue)
                isValue = false;
            else
                isValue = true;    
            
            if(tempstr=="")
            {
                tempstr.append(str, i, 1);
            }              
            else
            {	
            	  if(isBracket)
            	  {
            	      tempstr.append(str, i, 1);
            	  }
            	  else
            	  {
                    tempstr.append(str, i, 1);
                    for (u = 0; u < tempstr.size() ; u++)
                    {
                        if (tempstr[u] != ' ')  
                        {  
                            vec.push_back(tempstr);
                            break;
                        }
                    }
                    //cout << tempstr << endl;
                    tempstr = "";
                }
            } 
        }
        else if (str[i]=='(' || str[i]==')')
        {
            if (str[i]=='(')
            {
            	  isBracket = true;
            	  iBracket++;
                if(tempstr=="")
                {
                    tempstr.append(str, i, 1);
                }
                else
                {
                    if(isValue)
                    {
                       tempstr.append(str, i, 1); 
                    }
                    else
                    {
                    	  if(iBracket <= 0)
                        {
                            for (u = 0; u < tempstr.size() ; u++)
                            {
                                if (tempstr[u] != ' ')  
                                {  
                                    vec.push_back(tempstr);
                                    break;
                                }
                            }
                            //cout << tempstr << endl;
                            tempstr = "";
                            tempstr.append(str, i, 1); 
                        }
                        else
                        {
                            tempstr.append(str, i, 1);
                        }
                    }                   
                }                  
            }
            else
            {
            	  iBracket--;
            	  if (iBracket <= 0)
            	      isBracket = false;
                if(tempstr=="")
                {
                    tempstr.append(str, i, 1);
                }
                else
                {
                    if(isValue)
                    {
                       tempstr.append(str, i, 1); 
                    }
                    else
                    {
                    	  if(iBracket <= 0)
                    	  {
                            tempstr.append(str, i, 1);
                            for (u = 0; u < tempstr.size() ; u++)
                            {
                                if (tempstr[u] != ' ')  
                                {  
                                    vec.push_back(tempstr);
                                    break;
                                }
                            }
                            //cout << tempstr << endl;
                            tempstr = ""; 
                        }
                        else
                        {
                            tempstr.append(str, i, 1);
                        }
                    }                   
                }                                  
            }
        }	 
        else if(str[i]==',')
        {
            
            if(tempstr=="")
            {
                tempstr.append(str, i, 1);
                for (u = 0; u < tempstr.size() ; u++)
                {
                    if (tempstr[u] != ' ')  
                    {  
                        vec.push_back(tempstr);
                        break;
                    }
                }
                //cout << tempstr << endl;
                tempstr = "";
            }  
            else
            {
            	
                if(isValue || isBracket)
                {
                   tempstr.append(str, i, 1); 
                }
                else
                {
                    for (u = 0; u < tempstr.size() ; u++)
                    {
                        if (tempstr[u] != ' ')  
                        {  
                            vec.push_back(tempstr);
                            break;
                        }
                    }
                    //cout << tempstr << endl;
                    tempstr = "";
                    tempstr.append(str, i, 1);
                    for (u = 0; u < tempstr.size() ; u++)
                    {
                        if (tempstr[u] != ' ')  
                        {  
                            vec.push_back(tempstr);
                            break;
                        }
                    }
                    //cout << tempstr << endl;
                    tempstr = "";
                }
            }    
        }        	       
        else
        {
            tempstr.append(str, i, 1);
        }    
            
    }
    for (u = 0; u < tempstr.size() ; u++)
    {
        if (tempstr[u] != ' ')  
        {  
            vec.push_back(tempstr);
            break;
        }
    }    
    //cout << "value--"<<tempstr <<"--end"<< endl;
    
    for (i = 0; i < vec.size(); i++)
        vec[i] = trim(vec[i]);    
    //for(vector<string>::iterator itr=vec.begin();
    //    itr!=vec.end(); itr++)
    //{
    //    cout << *itr << endl;
    //}  
         
}       

//判断一个进程是否还在
bool isPidExists(const int pid)
{
    char cmd[200];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "ps -e -o pid | grep %d", pid);
    
    //cout << cmd << endl;
    
    /*
    char chPid[20];
    memset(chPid, 0, sizeof(chPid));
    sprintf(chPid, "%d", pid);
    */
    
    FILE * handle;
    
    if((handle = popen(cmd,"r")) == NULL)
    {
        cout << "popen err" << endl;
        return false;
    }
	fflush(handle);
	
	bool flag=false;
	char line[300];
    memset(line, 0, sizeof(line));
    
	while(fgets(line, sizeof(line), handle) != (char *)NULL)
	{
	    //cout  << "|" << chPid << "|"  << endl;
	    //cout << "|" << line << "|" << endl;
		//if(strncmp(line,chPid,10)==0)
		if(atoi(line)==pid)
		{
		    //cout << "flag = true" << endl;
		    //cout << "strlen(line)=" << strlen(line) << endl;
		    //cout << "strlen(chPid)=" << strlen(chPid) << endl;
		    /*line后面会多一个回车符，所以strncmp比较不对
		    if(strncmp(line,chPid,10)==0)
		    {
		        cout << "strncmp(line,chPid,10)==0" << endl;
		    }
		    else
		    {
		        cout << "strncmp(line,chPid,10)!=0" << endl;   
		    }*/
		    flag = true;
		    break;
		}
		memset(line, 0, sizeof(line));
	}
	
    if( pclose(handle) < 0)
    {
        cout << "pclose err" << endl;
        return false;
    }
    
	return flag;	
}

#endif

