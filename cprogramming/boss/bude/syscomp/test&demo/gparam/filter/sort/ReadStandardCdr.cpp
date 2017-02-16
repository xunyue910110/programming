// ##########################################
// Source file : ReadStandardCdr.cpp
// System      : Mobile Billing System
// Version     : 0.1.9
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020606
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "ReadStandardCdr.h"

//##ModelId=3BD944980123
bool ReadStandardCdr::initialize(string errorPath)
{
   m_errorPath = errorPath;
#ifdef DEBUG
    cout<<"error path is "<<m_errorPath<<endl;
#endif

   return true;
}

//##ModelId=3BDD6168000A
bool ReadStandardCdr::getCdr(char * cdrBuffer,const int n)
{
FP_BEGIN(getCdr)
	if ( fgets(cdrBuffer,n,m_fileFp)==NULL )
	   return false;
FP_END(getCdr)
   return true;
}

//##ModelId=3BDEA6330382
bool ReadStandardCdr::reset(const string fileName)
{
    string thefilename;
    
    m_fileName  = fileName;
    thefilename = m_fileName;
    m_fileFp    = 0;
    
    m_fileFp=fopen(thefilename.c_str(),"r");
    if ( m_fileFp == NULL ) 
    {
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"open "+thefilename+" error!");
        return false;		//打开文件错误
    }
    
    return true;
}

//##ModelId=3BDEA63303BF
bool ReadStandardCdr::close(int flag)   // flag =0 ,not error; =1, is error =2 not error and not move src file
{
    DIR *dp;
    int status;
    int index,len;
    string FileName;
    string desFileName,fileName,cmd;

    len       = m_fileName.size();
    index     = m_fileName.find_last_of("/");
    fileName  = m_fileName.substr(index+1,len-index-1);

    desFileName = m_errorPath ;

    if(m_fileFp !=0) {
        fclose(m_fileFp);
        m_fileFp=0;
    }
// add by zhangy in 2002.6.6 ==== start ======
    if(flag==2) {
        return true;
    }
// add by zhangy in 2002.6.6 ==== end ======
    if(flag !=0 ) {
        dp=opendir(m_errorPath.c_str());
        if(dp==NULL) {
            theErrorMessages->insert(PP_ERROR_LINK_FILE,"错误文件目录"+m_errorPath+"不存在!");
            #ifdef DEBUG
                cout<<"错误文件目录"+m_errorPath+"不存在!"<<endl;
            #endif
            return false;
        }
        if(closedir(dp)!=0) {
            theErrorMessages->insert(PP_ERROR_LINK_FILE,"错误文件目录"+m_errorPath+"操作失败!");
            #ifdef DEBUG
                cout<<"错误文件目录"+m_errorPath+"操作失败!"<<endl;
            #endif
            return false;
        }
        cmd="mv "+m_fileName+" "+desFileName;
        if( ( status=system(cmd.c_str()) ) < 0 ) {   // 将标准话单move到ferr中
		    theErrorMessages->insert(PP_ERROR_LINK_FILE,"不能将文件"+m_fileName+"拷贝成"+desFileName+"!");
            #ifdef DEBUG
                cout<<"status is "<<status<<",不能将文件"+m_fileName+"拷贝成"+desFileName+"!"<<endl;
            #endif
		    return false;
	    }
    } else {
        if( ( ( status=remove(m_fileName.c_str()) ) ) != 0 ) {          //删除标准话单
            theErrorMessages->insert(PP_ERROR_UNLINK_FILE,"不能删除"+m_fileName+"文件!");
            #ifdef DEBUG
                 cout<<",不能删除"+m_fileName+"文件!"<<endl;
            #endif
            return false;
        }
    }


    return true;
}

