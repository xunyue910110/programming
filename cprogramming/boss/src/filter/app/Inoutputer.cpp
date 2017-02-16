#include "Inoutputer.h"

Inoutputer::Inoutputer()
{
}
Inoutputer::~Inoutputer()
{
  close();  
}

//设置文件路径、文件名
void  Inoutputer::setName(const char *r_fileName,const char *r_path)
{
  m_sFileName= r_fileName;
  m_pathName = r_path;
  m_fullName = r_path;
  if (strlen(r_path) > 0) 
  {
    if (r_path[strlen(r_path)-1]!='/')
      m_fullName+='/'; 
  }else
  {
    m_fullName+='/'; 
  } 
  m_fullName += r_fileName;
}

void  Inoutputer::setName(const char *r_fullName)
{
  m_sFileName= strrchr(r_fullName,'/')+1;
  m_pathName = r_fullName;
  m_pathName[strlen(r_fullName)-strlen(m_sFileName)-1]=0;
  m_fullName = r_fullName;
}

bool  Inoutputer::open(const int &r_mode) 
{
  if(m_openFlag==true)
  {
    m_fstream.close();
    m_fstream.clear();
    m_openFlag = false;
  }
  m_fstream.open(m_fullName.c_str(),r_mode);
  if(!m_fstream)
  {
    return false;
  }
  m_openMode = r_mode;
  m_openFlag = true;
  return true;
}
bool  Inoutputer::is_open()
{
  return m_openFlag;
}
bool  Inoutputer::close()  
{
  if(m_openFlag==true)
  {
    m_fstream.close();
    m_fstream.clear();
    m_openFlag = false;
  }
  return true;
}
bool  Inoutputer::unlinkFile()  const
{
  if(::unlink(m_fullName.c_str())!=0)
  {
    return false;
  }
  return true;
}
bool  Inoutputer::linkFile(const char *r_desFileName,const char *r_desPath) const 
{
  char t_desFullName[256+1];
  if(strlen(r_desPath)>0 && r_desPath[strlen(r_desPath)-1]=='/')
  {
    sprintf(t_desFullName,"%s%s",r_desPath,r_desFileName);
  }else
  {
    sprintf(t_desFullName,"%s/%s",r_desPath,r_desFileName);
  }
  return linkFile(t_desFullName);
}
bool  Inoutputer::linkFile(const char *r_desFullName) const
{
  if(::link(m_fullName.c_str(),r_desFullName)!=0)
  {
    if(copy(m_fullName.c_str(),r_desFullName)!=0)
    {
      return false;
    }
  }
  return true;
}
bool  Inoutputer::renameFile(const char *r_desFileName,const char *r_desPath)const 
{
  char t_desFullName[256+1];
  if(strlen(r_desPath)>0 && r_desPath[strlen(r_desPath)-1]=='/')
  {
    sprintf(t_desFullName,"%s%s",r_desPath,r_desFileName);
  }else
  {
    sprintf(t_desFullName,"%s/%s",r_desPath,r_desFileName);
  }
  return renameFile(t_desFullName);  
}
bool  Inoutputer::renameFile(const char *r_desFullName) const
{
  if(::rename(m_fullName.c_str(),r_desFullName)!=0)
  {
    //cout<<"m_fullName    = "<<m_fullName<<endl;
    //cout<<"r_desFullName = "<<r_desFullName<<endl;
    if(copy(m_fullName.c_str(),r_desFullName)!=0)
    {
      return false;
    }
    if(unlinkFile()==false)
    {
      return false;
    }
  }
  return true;
}
 //返回值：  -1－错误，0－文件结束，1－成功
int  Inoutputer::read(char * r_cdr,const int &r_buffLen,
                       const char &r_lineDelimiter) 
{
  if(m_openFlag==false||(m_openMode&ios::in)==0)
  {
    return -1;
  }
  if(!m_fstream.eof())
  {
    m_fstream.getline(r_cdr,r_buffLen,r_lineDelimiter);
    if(strlen(r_cdr)==0) return 0;
    else return 1;
  }else
    return 0;
}
bool  Inoutputer::write(const char *r_cdr) 
{
  if(m_openFlag==false)
  {
    if(open(ios::out)==false)
      return false;
  }
  if(m_openFlag==false||(m_openMode&ios::out)==0)
  {
    return false;
  }
  m_fstream<<r_cdr;
  return true;
}
int Inoutputer::copy(const char *r_srcFile,const char *r_desFile) const
{
  FILE *srcFile,*desFile;
  int readSize;
  const int BUFFLEN=1024;
  char msgBuff[BUFFLEN+1];
  char tmpFile[256+1];
  int  sLen,tLen;
  
  if(strrchr(r_desFile,'/')==NULL)
  {
    sprintf(tmpFile,"%%%s",r_desFile);
  }else
  {
    sprintf(tmpFile,"%s",r_desFile);
    tLen= strlen(r_desFile);
    sLen = strlen(strrchr(r_desFile,'/')+1);
    sprintf(tmpFile+tLen-sLen,"%%%s",strrchr(r_desFile,'/')+1);
  } 
  if((srcFile=fopen(r_srcFile,"rb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<r_srcFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  if((desFile=fopen(tmpFile,"wb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    fclose(srcFile);
    return -1;
  }
  while(!feof(srcFile))
  {
    readSize=fread(msgBuff,1,BUFFLEN,srcFile);
    if(readSize == 0) break;
    if(fwrite(msgBuff,1,readSize,desFile)!=readSize)
    {
      fclose(srcFile);
      fclose(desFile);
      return -1;
    }
  }
  fclose(srcFile);
  fclose(desFile);
  if(rename(tmpFile,r_desFile)!=0)
  {
    #ifdef _DEBUG_
      cout<<"rename file:"<<tmpFile<<" "<<r_desFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  return 0;
}
const char* Inoutputer::getFileName()
{
  return m_sFileName.c_str();
}

bool Inoutputer::copyFile(const char *r_srcFile,const char *r_desFile) const
{
  return copy(r_srcFile,r_desFile);
}

