// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// Editors:     Yang Zhang  <zhangy@lianchuang.com>
// Update       20011030
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//


#include "config-all.h"
#include "supdebug.h"
#include "SysParam.h"

#include "zy_common.h"
#include "ErrorHandle.h"
#include "MyApplication.h"



#include <assert.h>
#include <unistd.h>

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>
USING_NAMESPACE(std)


//-------------------------------------------------------------------------//
// application object, which will replaced by your application
//-------------------------------------------------------------------------//
MyApplication g_application;
Application * const theApp = &g_application;


bool MyApplication::processopt(int optopt, const char *optarg)
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
    // process opt
	  bool rc = true;
    switch (optopt)
    {
        //-----------------------------------------------------------------------//
        // TODO: Add your specialized option process code here
        // case 'f':
        //   isFileDefiend = true;
        //   filename = optarg;
        //   break;
        //-----------------------------------------------------------------------//
        
        case 'c':
        case ':':
        case '?':
        default:
          rc = Application::processopt(optopt, optarg);
    }
    return rc;
}//processopt


//##ModelId=3BCAA280003A
bool MyApplication::initialization()
{
    char channelNo[4];

    string root,filterPath, section, name, value;

    //-----------------------------------------------------------------------//
    // call setoptstr before initialization()
    // set optstring -- reference unix getopt() call
    // the 'c' option is reserved for channel no option,
    // you can add your owner option
    //-----------------------------------------------------------------------//
    setoptstr("c:");

#ifdef DEBUG
    cout << "Begin Init" << endl;
#endif

    // normally, call standard initialization first
    if (!Application::initialization())
    	return false;

	
    m_fsortmethod = Directory::SF_MODIFYTIME;
    // set application run method if need, all default is false

#ifndef DEBUG
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif

    //-----------------------------------------------------------------------//
    // ADD YOUR INITIALIZATION CODE HERE
    //-----------------------------------------------------------------------//

    //============= get src path start ========================

    int chanNo=0;
    m_SortApplication=0;

    root=SECTDELIM+string(m_name);

    name="source_path";
    
    sprintf(channelNo,"%d",chanNo);
    filterPath=root+SECTDELIM+"DataSource"+SECTDELIM+channelNo;
    while( m_theSysParam->getValue(filterPath,name,value) )
    {
       chanNo++;
       sprintf(channelNo,"%d",chanNo);
       filterPath=root+SECTDELIM+"DataSource"+SECTDELIM+channelNo;

       m_srcPath.push_back(value);//可能会有多个源目录
    }

    m_pathNo       = 0;
    m_fileSum      = 0;
    m_sourcePath   = getNextPath();

    //============= get src path end ========================

    m_SortApplication = new SortApplication;

    if(m_SortApplication==0) 
    {
       m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT,"cannot create SortApplication Object!");
       return false;
    }

	  if ( !m_SortApplication->initialize(m_channelNo,m_theSysParam,root) )
	  {
	      m_theErrorMessages->insert(PP_ERROR_INITIALIZE,"cannot initialize SortApplication!");
		    return false;
	  }

    return true;
}

//##ModelId=3BCAA28000B2
bool MyApplication::end()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and call the base class
    //-----------------------------------------------------------------------//
    return Application::end();
}

//##ModelId=3BCAA2800076
bool MyApplication::recovery()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
    
    // TO BE DETERMINED
    Application::recovery();
    return true;
}

//##ModelId=3BCAA28000C6
bool MyApplication::loopProcess()
{
    int i=0;
    int fileCount;
    char fileName[80];
    string fileNameWithPath;

    memset(fileName,0,sizeof(fileName));
    if(m_fileSum >= 10) {

       m_fileSum=0;
       m_sourcePath = getNextPath();
    }

    fileCount=m_srcPath.size();//源目录数
    while( !getFile(fileName,80) )
    {
    	 //此时这个目录下没有文件了
       if(i>=fileCount) 
       {
          i=0;
          m_status = PROC_IDLE;
          m_objectName.clear();
          updateStatus();
          
          #ifdef DEBUG
             cout<<"sleep "<<SLEEP_TIME<<"s..."<<endl;
          #endif
          
          sleep(SLEEP_TIME);
          return  Application::loopProcess();
       }
       i++;
       m_fileSum=0;
       m_sourcePath = getNextPath();
       memset(fileName,0,sizeof(fileName));
    }

    if(strncmp(fileName,"#",1)!=0) 
    {
	      fileNameWithPath = m_sourcePath + fileName;
	      m_status         = PROC_RUN;
	      m_objectName     = fileName;
	      updateStatus();
	      
	      
FP_BEGIN(run)

        if( !m_SortApplication->run(fileNameWithPath,m_pathNo) ) 
        {
           m_status     = PROC_WARN;
           m_objectName = string("处理文件 ") + fileNameWithPath + " 失败!";
           updateStatus();
           
           #ifdef DEBUG
              cout<<m_objectName<<endl;
              cout<<"sleep "<<SLEEP_TIME<<"s..."<<endl;
           #endif
           sleep(SLEEP_TIME);
        }

FP_END(run)

        writeAlert();
        writeLog();
    }
    
    m_fileSum++;

    return Application::loopProcess();
}//loopProcess

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//

void MyApplication::writeLog()
{
    int prov;
    char Tmp[256];
    
    prov=theSortLog->getTotalCdrs() - theSortLog->getNrSum()
                                    - theSortLog->getIrSum()
                                    - theSortLog->getTrashSum()
                                    - theSortLog->getErrorSum()
                                    - theSortLog->getNoinfoSum()
                                    - theSortLog->getPocSum()
                                    - theSortLog->getSpecialSum()
                                    - theSortLog->getPocNoinfoSum();
    
    sprintf( Tmp,"%s@%d@%d@%d@%d@%d@%d@%d@%d@%d@%d",
                 (theSortLog->getSrcFileName()).c_str(),
                 theSortLog->getTotalCdrs(),
                 prov,
                 theSortLog->getNrSum(),
                 theSortLog->getIrSum(),
                 theSortLog->getTrashSum(),
                 theSortLog->getNoinfoSum(),
                 theSortLog->getErrorSum(),
                 theSortLog->getSpecialSum(),
                 theSortLog->getPocSum(),
                 theSortLog->getPocNoinfoSum() );
    m_logstr = Tmp;
}

MyApplication::~MyApplication()
{
    if(m_SortApplication!=0)
        delete m_SortApplication;
}

string MyApplication::getNextPath()
{
    int len;
    
    m_pathNo++;
    if(m_pathNo+1>m_srcPath.size()) 
    {
       m_pathNo = 0;
    }
    
    setPathIn(m_srcPath[m_pathNo].c_str());
    
    return m_srcPath[m_pathNo];
}

int MyApplication::getFile(char *filename, const int length)
{
    int i,ret;
    
    i=0;
    ret=1;
    while (i<=ret)
    {
        if ((ret = m_dir.getFile(filename, length)) != 0) 
        {
           if(strncmp(filename,"#",1)!=0) 
           {
              return ret;
           }
        }
        i++;
    }
    return 0;
}

void MyApplication::setPathIn(const char *path)
{
    m_dir.setPath(path);
    m_dir.scanFiles(m_fsortmethod);
}

