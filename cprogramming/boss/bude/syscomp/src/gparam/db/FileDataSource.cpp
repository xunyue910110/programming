// ############################################
// Source file : FileDataSource.cpp
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <unistd.h>

#include "util/Util.h"
#include "FileDataSource.h"

FileDataSource::FileDataSource(const FileDataSource &right)
{
  *this = right;
}
void FileDataSource::operator=(const FileDataSource& right)
{
  m_attribute = right.m_attribute;
  m_sourceID  = right.m_sourceID;
  m_parameterLog = ParameterLog::getInstance();
  m_dp = right.m_dp;
  m_filePath = right.m_filePath;
}

//##ModelId=3F72753A0329
FileDataSource::FileDataSource()
{ 
}

//##ModelId=3F727543039A
FileDataSource::~FileDataSource()
{   
}

//##ModelId=3F72721700CC
bool FileDataSource::initialize(const vector<string> &name)
{   
    int i=0;
     
    if(name.size()!=2)
        return false;
        
    m_sourceID    = name[i++];
    m_filePath     = name[i++]; 
    checkPath(m_filePath); 
    
    if( m_parameterLog->m_debug>=2) {
        m_parameterLog->setMessage("m_sourceID="+m_sourceID+",m_filePath="+m_filePath+"\n",0); 
    }
    return true;
}

//##ModelId=3F7278EB0093
int FileDataSource::getRecord(vector <string> &cdr)
{   
    int i;    
             
    if( (i=m_PFileProcess.getCdr(",",cdr,false)) <= 0) { 
        m_PFileProcess.close(); 
    } 
    
    return i;
}

//##ModelId=3F754EA20078  传入参数为文件名  打开并返回记录数
long FileDataSource::reset(const string &attribute)
{    
    char tmp[10];
    int countTotal=0;
    string srcfile,bakfile,cmd;  
    vector <string> outs;
    
    m_attribute = attribute; 
    
    srcfile = m_filePath + m_attribute; 
    bakfile = srcfile    + ".bak";  
    
    if( access(srcfile.c_str(),F_OK) == 0 ) 
    {  
        cmd = "mv " + srcfile + " " + bakfile; 
        if( system(cmd.c_str()) )    {
            m_parameterLog->setMessage("文件"+srcfile+"改名失败！\n",1);
            cout<<"文件"<<srcfile<<"改名失败!"<<__FILE__<<__LINE__<<endl;
            return -1; 
        }
    }
    
    if( !m_PFileProcess.open(bakfile,"r") )  { 
        m_parameterLog->setMessage("文件"+bakfile+"打开失败！\n",1);
        cout<<"文件"<<srcfile<<"打开失败!"<<__FILE__<<__LINE__<<endl;
        return -1; 
    }  
    
    if( m_parameterLog->m_debug>=2) { 
        sprintf(tmp,"%d",countTotal);
        m_parameterLog->setMessage("\n文件名："+m_attribute+"\n",0); 
    }
            
    cmd =  "wc " + bakfile + " 2>/dev/null|awk '{print $1}'";   
    //modi by cy 2004.11.19 
	  if( command(cmd,outs) == -1 ) 
	  {
	  	cout<<"command(cmd,outs) 执行失败！"<<endl;
	  	return -1;
	  }  

    
    countTotal = atoi(outs[0].c_str());
    if( m_parameterLog->m_debug>=2) { 
        sprintf(tmp,"%d",countTotal);
        m_parameterLog->setMessage(string(" 记录数 = ")+tmp+", ",0);  
    }         
        
    return countTotal;
}

//##ModelId=3F9F4F3B0140
void FileDataSource::close()
{
}

//##ModelId=3FD7CA720345
bool FileDataSource::needUpdata(const string &srcFile)
{                   
    string file     = m_filePath + srcFile; 
    
    if( access(file.c_str(),F_OK) == 0 ) { 
        return true;
    }  
    
    return false;
}

