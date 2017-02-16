// ##########################################
// Source file : WriteStandardCdr.cpp
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020424
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <unistd.h>
#include <sys/stat.h>

#include "WriteStandardCdr.h"

//##ModelId=3C200277019A
WriteStandardCdr::WriteStandardCdr()
{
   m_tmpFileHead = "#";
   m_invalidFp = 0;
}

//##ModelId=3C366A1D0147
bool WriteStandardCdr::setPrefix(string p,string n,string i,string u)
{
   m_prefixP.push_back(p);
   m_prefixN.push_back(n);
   m_prefixI.push_back(i);
   m_prefixU.push_back(u);

   return true;
}

//##ModelId=3BDD07F302EF 三个都是输入参数
bool WriteStandardCdr::setPath(const string channelNo, 
                               const string channelPath,
                               const int    chNo)
{
	  int ch;
    char tmp[20];
    bool flag=true;
    FilterFp theFilterFp;

    theFilterFp.m_filterFp     = 0;
    theFilterFp.m_channelNo    = channelNo;
    theFilterFp.m_filterPath   = channelPath;

    strcpy(tmp,channelNo.c_str());
    for(int i=0;i<strlen(tmp);i++)
    {
        if( !isdigit(tmp[i]) ) {
            flag=false;
        }
    }

    if(flag) {                                          // 省内
    	ch=atoi(channelNo.c_str());
        sprintf(tmp,"%02d_",ch);
        theFilterFp.m_prefix = m_prefixP[chNo] + tmp;
    } else if(channelNo.compare("ir")==0) {             // 国际
        theFilterFp.m_prefix = m_prefixI[chNo] + "00_";
    } else if(channelNo.compare("nr")==0) {             // 国内
        theFilterFp.m_prefix = m_prefixN[chNo] + "00_";
    } else if(channelNo.compare("noinfo")==0) {         // 无主
        theFilterFp.m_prefix = m_prefixU[chNo] + "00_";
    } else if(channelNo.compare("special")==0) {        // 特殊
        theFilterFp.m_prefix = "X00_";
    } else if(channelNo.compare("pocNoinfo")==0) {      // POC无主
        theFilterFp.m_prefix = "C00_";
    } else if(channelNo.compare("error")==0) {          // 错误
        theFilterFp.m_prefix = "E00_";
    } else if(channelNo.compare("pocTrash")==0) {       // POC无效话单 add by 2002.4.24
        theFilterFp.m_prefix.clear();
    } else if(channelNo.compare("ptcTrash")==0) {       // PTC无效话单 add by 2002.4.24
        theFilterFp.m_prefix.clear();
    }  else if(channelNo.compare("smTrash")==0) {        // 短信无效话单 add by 2002.6.03
        theFilterFp.m_prefix.clear();
    }  else if(channelNo.compare("trash")==0) {          // 其他无效话单 add by 2002.4.24
        theFilterFp.m_prefix.clear();
    } else if(channelNo.compare("scp")==0) {          // 神州行业务话单 add by xiel 2004/05/10
        theFilterFp.m_prefix = m_prefixU[chNo] + "99_";
    } else if(channelNo.compare("prov")==0) {          // 神州行业务话单 add by xiel 2004/05/10
        theFilterFp.m_prefix.clear();
    } else if(channelNo.compare("pbx")==0) {          // 专网业务话单 add by liux 2004/09/10
        theFilterFp.m_prefix.clear();
    } else {
        return true;
    }

    m_FilterFp.push_back(theFilterFp);

    return true;
}

//##ModelId=3BDD5FF50350
bool WriteStandardCdr::write(string & channel,  char* cdr )
{
    FILE *Fp=0;
    int offset;
    char topTmp[4];

    if( (Fp=getFilterFp(channel,m_no) )==0 ) {
       theErrorMessages->insert(PP_ERROR_WRITE_FILE,"cannot get channel "+channel+"!");
       return false;
    }
    strcat(cdr,"\n");
	if ( fputs(cdr,Fp) < 0 ) {
	   theErrorMessages->insert(PP_ERROR_WRITE_FILE,"cannot write file !");
	   sleep(1);
	   return false;
	}
	return true;
}

//##ModelId=3BDF9E95021F
FILE *WriteStandardCdr::getFilterFp(string & channelNo,const int chNo)
{
    string filename;

    vector<FilterFp> &FilterFp = m_fp[chNo];
    for(int i=0;i<FilterFp.size();i++)
    {
        if( FilterFp[i].m_channelNo.compare(channelNo)==0 ) {
            return FilterFp[i].m_filterFp;
        }
    }

    return 0;
}

//##ModelId=3BDF9F090262
void WriteStandardCdr::close()
{
   int len;
   struct stat attr;
   string tmpName,fileName;

   vector<FilterFp> &FilterFp = m_fp[m_no];
   len = FilterFp.size();

   for(int i=0;i<len;i++)
   {
        fclose(FilterFp[i].m_filterFp);

        tmpName  = FilterFp[i].m_filterPath + m_tmpFileHead
                    + FilterFp[i].m_prefix  + m_fileName;
        fileName = FilterFp[i].m_filterPath + FilterFp[i].m_prefix + m_fileName;

        if(stat(tmpName.c_str(), &attr)!=0)
            return;

	    if(attr.st_size==0) {                  // 如果文件长度为零，删除该文件
	       if( unlink( tmpName.c_str() )!=0) {         //删除临时话单文件
             theErrorMessages->insert(PP_ERROR_UNLINK_FILE,"不能删除"+tmpName+"文件!");
#ifdef DEBUG
    cout<<"不能删除"+tmpName+"文件!"<<endl;
#endif
		   }
	    } else {
	        rename(tmpName.c_str(),fileName.c_str());
	    }
   }
}

//##ModelId=3BDFADD802BD
bool WriteStandardCdr::reset(const string fileName,const int chNo)
{
    char chan[3];
    int len,index;
    string thefilename;
    
    m_no  = chNo;
    
    len   = fileName.size();
    thefilename = fileName;
    index = fileName.find_last_of("/");
    
    m_fileName  = thefilename.substr(index+1,len-index-1);//去了路径
    theSortLog->setSrcFileName(m_fileName);
    
    vector<FilterFp> &FilterFp = m_fp[m_no];
    len = FilterFp.size();
    
    for(int i=0;i<len;i++) 
    {
       thefilename =  FilterFp[i].m_filterPath + m_tmpFileHead
                    + FilterFp[i].m_prefix     + m_fileName;
    
       FilterFp[i].m_filterFp=fopen(thefilename.c_str(),"w");
       
       if ( FilterFp[i].m_filterFp == NULL ) 
       {
           theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"打开文件"+thefilename+"失败!");
#ifdef DEBUG
    cout<<"打开文件"+thefilename+"失败!"<<endl;
#endif
		       return false;		//打开文件错误
	     }
    }

    return true;
}

