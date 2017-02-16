#include "Inoutputer.h"

Inoutputer::Inoutputer()
{
  m_openFlag = false;
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
  char t_desFullName[512+1];
  if(strlen(r_desPath)>0 && r_desPath[strlen(r_desPath)-1]=='/')
  {
    sprintf(t_desFullName,"%s%s",r_desPath,r_desFileName);
  }else
  {
    sprintf(t_desFullName,"%s/%s",r_desPath,r_desFileName);
  }
  return linkFile(t_desFullName);
}
//add by gaojf 2009-2-21 11:04
bool  Inoutputer::copyFile(const char *r_desFullName) const
{
  const char t_dirSepChar='/';
  char  t_tmpFile[1024+1];
  int   t_tLen,t_sLen;
  if(strrchr(r_desFullName,t_dirSepChar)==NULL)
  {
    sprintf(t_tmpFile,"%%%s",r_desFullName);
  }else
  {
    sprintf(t_tmpFile,"%s",r_desFullName);
    t_tLen = strlen(r_desFullName);
    t_sLen = strlen(strrchr(r_desFullName,t_dirSepChar)+1);
    sprintf(t_tmpFile+t_tLen-t_sLen,"%%%s",strrchr(r_desFullName,t_dirSepChar)+1);
  } 
  if(copy(m_fullName.c_str(),t_tmpFile)!=0)
  {
    #ifdef _DEBUG_
      cout<<"link "<<m_fullName<<" "<<r_desFullName<<" false!"<<endl;
      cout<<"copy "<<m_fullName<<" "<<t_tmpFile<<" false!"<<endl;
    #endif
    return false;
  }else
  {
    if(rename(t_tmpFile,r_desFullName)!=0)
    {
      #ifdef _DEBUG_
        cout<<"rename "<<t_tmpFile<<" "<<r_desFullName<<" false!"<<endl;
      #endif
      return false;
    }
  }
  return true;
}

bool  Inoutputer::linkFile(const char *r_desFullName) const
{
  if(::link(m_fullName.c_str(),r_desFullName)!=0)
  {
    return copyFile(r_desFullName);
  }
  return true;
}
bool  Inoutputer::renameFile(const char *r_desFileName,const char *r_desPath)const 
{
  char t_desFullName[512+1];
  if(strlen(r_desPath)>0 && r_desPath[strlen(r_desPath)-1]=='/')
  {
    sprintf(t_desFullName,"%s%s",r_desPath,r_desFileName);
  }else
  {
    sprintf(t_desFullName,"%s/%s",r_desPath,r_desFileName);
  }
  
  //add by modif by yangyi 为固网融合临时修改
  if (strncmp(r_desFileName,"TTD",3) == 0)
  {
        string t_desFullNameStr;
        t_desFullNameStr = t_desFullName;
        t_desFullNameStr = t_desFullNameStr.substr(0,t_desFullNameStr.length()-22);
        return renameFile(t_desFullNameStr.c_str());
  }
  return renameFile(t_desFullName);  
}
bool  Inoutputer::renameFile(const char *r_desFullName) const
{
  if(::rename(m_fullName.c_str(),r_desFullName)!=0)
  {
    #ifdef _DEBUG_
      cout<<"m_fullName    = "<<m_fullName<<endl;
      cout<<"r_desFullName = "<<r_desFullName<<endl;
    #endif
    if(copyFile(r_desFullName)!=true)
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
  int t_cdrLen;
  if(m_openFlag==false||(m_openMode&ios::in)==0)
  {
    return -1;
  }
  if(!m_fstream.eof())
  {
    m_fstream.getline(r_cdr,r_buffLen,r_lineDelimiter);
    t_cdrLen = strlen(r_cdr);
    if(t_cdrLen>0)
    { //去掉最后换行符
      if(r_cdr[t_cdrLen-1]==r_lineDelimiter)
      {
        r_cdr[t_cdrLen-1]=0;t_cdrLen--;
      }
    }
    if(t_cdrLen==0)
    {
      return read(r_cdr,r_buffLen,r_lineDelimiter) ;
    }else return 1;
  }else return 0;
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
  FILE *t_srcFile,*t_desFile;
  int t_readSize;
  const int BUFFLEN=1024;
  char t_msgBuff[BUFFLEN+1];
  char t_tmpFile[512+1];
  int  t_sLen,t_tLen;
  
  if(strrchr(r_desFile,'/')==NULL)
  {
    sprintf(t_tmpFile,"%%%s",r_desFile);
  }else
  {
    sprintf(t_tmpFile,"%s",r_desFile);
    t_tLen= strlen(r_desFile);
    t_sLen = strlen(strrchr(r_desFile,'/')+1);
    sprintf(t_tmpFile+t_tLen-t_sLen,"%%%s",strrchr(r_desFile,'/')+1);
  } 
  if((t_srcFile=fopen(r_srcFile,"rb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<r_srcFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  if((t_desFile=fopen(t_tmpFile,"wb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<t_tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    fclose(t_srcFile);
    return -1;
  }
  while(!feof(t_srcFile))
  {
    t_readSize=fread(t_msgBuff,1,BUFFLEN,t_srcFile);
    if(t_readSize == 0) break;
    if(fwrite(t_msgBuff,1,t_readSize,t_desFile)!=t_readSize)
    {
      fclose(t_srcFile);
      fclose(t_desFile);
      return -1;
    }
  }
  fclose(t_srcFile);
  fclose(t_desFile);
  if(rename(t_tmpFile,r_desFile)!=0)
  {
    #ifdef _DEBUG_
      cout<<"rename file:"<<t_tmpFile<<" "<<r_desFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  return 0;
}
const char* Inoutputer::getFileName()
{
  return m_sFileName.c_str();
}
const char* Inoutputer::getFullFileName()
{
  return m_fullName.c_str();
}

