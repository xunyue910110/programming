// ##########################################
// Source file : SortApplication.cpp
// System      : Mobile Billing System
// Version     : 0.1.15
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020606
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <unistd.h>

#include "SortApplication.h"

SortLog g_SortLog;
SortLog * const theSortLog = &g_SortLog;

//##ModelId=3BD9346B0255
bool SortApplication::run(const string fileName,const int datasource)
{
    int flag;
    double st;
    char tmp[10];
    char *cdrTmp=0;
    int len,callType;
    string channelNo;
    SortCdr *theSortCdr=0;
    int error=0;

//FP_BEGIN(reset)
    // edit by zhangy in 2002.5.24 ==== start ======
    flag = reset(fileName,datasource);
    if ( flag == -1 ) {     // 不能忽略的错误，话单文件必须移到错误文件目录中
        theErrorMessages->insert(PP_ERROR_RESET,"reset file "+fileName+" error!");
        close(1);
        return false;
    } else if(flag == 0) {  // 可忽略的错误，话单文件等待下次继续处理
        theErrorMessages->insert(PP_ERROR_RESET,"reset file "+fileName+" error!");
        close(2);           // edit by zhangy in 2002.6.6
        sleep(10);          // add  by zhangy in 2002.6.6
        return false;
    }
    // edit by zhangy in 2002.5.24 ==== end ======

    theSortCdr = m_SortCdr[datasource];

//FP_END(reset)
    while( m_ReadStandardCdr.getCdr(m_cdrBuffer,CDR_BUFFER_SIZE - 1) )
    {
        len=strlen(m_cdrBuffer);

        for(int i=len-4;i<len;i++)                   // 删除尾部的控制字符
        {
            if(m_cdrBuffer[i]<0x20) 
            {
               m_cdrBuffer[i]=0;
               break;
            }
        }
        if( !checkTokenCount(',',datasource) )       // 校验分隔符数量
        {
        	//cout<<fileName<<"话单字段数错误!"<<endl;
            theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, fileName+"话单字段数错误!");
            error=1;
            break;
        }
//FP_BEGIN(sssss)

       // 返回值为话单错误代码，“0”表示无错误

        theSortCdr->getFields(m_cdrBuffer);//得到各输入字段(配置中关心的字段)

        if (theSortCdr->isTrashCdr(callType)) 
        {
            if ( callType == 11 ) {
                channelNo = CHANNEL_POCTRASH;           // add  by zhangy in 2002.4.24
            } else if ( callType == 12 ) {
                channelNo = CHANNEL_PTCTRASH;           // add  by zhangy in 2002.4.24
            } else if ( callType == 7 ) {
                channelNo = CHANNEL_SMTRASH;            // add  by zhangy in 2002.6.3
            } else if ( callType == 8 ) {
                channelNo = CHANNEL_SMTRASH;            // add  by zhangy in 2002.6.3
            } else {
                channelNo = CHANNEL_TRASH;              // edit by zhangy in 2002.4.24
            }
            theSortLog->addTrashSum();
            m_WriteStandardCdr.write(channelNo,m_cdrBuffer);
            
        } 
        else if (theSortCdr->getError()>0) 
        {
            theSortLog->addErrorSum();
            cdrTmp = theSortCdr->addFields(0);
            
            channelNo = CHANNEL_ERROR;                  // edit by zhangy in 2002.4.24
            m_WriteStandardCdr.write(channelNo,cdrTmp);
        } 
        else 
        {
            channelNo=theSortCdr->getChannel(cdrTmp);
            m_WriteStandardCdr.write(channelNo,cdrTmp);
        }
//FP_END(sssss)
        m_runCDRs++;
        theSortLog->addTotalCdrs();

    }

#ifdef DEBUG
    theSortLog->showLog();
#endif

    close(error);

#ifdef DEBUG
if(error!=0)
{
    cout<<"话单文件字段数错误,话单文件移到错误目录中,请查验!"<<endl;
} 
else 
{
    cout<<"count is "<<m_runCDRs<<", times is "<<m_runTimes<<endl;
}
#endif

    return true;
}

//##ModelId=3BD9342C03D1
bool SortApplication::initialize(const int channelNo,SysParam *theSysParam, string root)
{
    int len;                                    // 2002.4.9新加行
    int channel;
    char tmp[10];
    int chNo,count;
    string appName;                             // 2002.4.9新加行
    string p,n,i,u;
    string xname,xvalue;
    SortCdr *theSortCdr=0;
    char chanNo[4],hostname[20];
    string filterPath,subFilterPath;
    string name,value,theFilterRule;
    vector<FilterFp> * thetp=0;

    len=root.size();                            // 2002.4.9新加行
    appName = root.substr(1,len-1);             // 2002.4.9新加行

    chNo        = 0;
    channel     = 0;
    name        = "sort_rule";
    sprintf(chanNo,"%d",chNo);
    filterPath  = root + "\\DataSource\\" + chanNo;
    //一个进程，多个通道
    while ( theSysParam->getValue(filterPath,name,value) )
    {
        if ( (theSortCdr = createRuleObject(value)) == 0 ) {
           theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create SortCdr object!");
           return false;
        }

        name  = "filter_rule";
        if(!theSysParam->getValue(filterPath,name,value)) {
           theErrorMessages->insert( EAPPFRM_SYSVAL,name );
           return false;
        }
        theFilterRule = value;

        p.clear();
        name  = "prefix_p";
        if(theSysParam->getValue(filterPath,name,value)) {
           p = value;
        }

        n.clear();
        name  = "prefix_nr";
        if(theSysParam->getValue(filterPath,name,value)) {
           n = value;
        }

        i.clear();
        name  = "prefix_ir";
        if(theSysParam->getValue(filterPath,name,value)) {
           i = value;
        }

        u.clear();
        name  = "prefix_u";
        if(theSysParam->getValue(filterPath,name,value)) {
           u = value;
        }

        name  = "field_count";
        if(!theSysParam->getValue(filterPath,name,value)) {
           theErrorMessages->insert( EAPPFRM_SYSVAL,name );
           return false;
        }
        count = atoi( value.c_str() );
        m_fieldCount.push_back( count );

        // initialize SorCdr's input field
        subFilterPath = filterPath + "\\input_field";

        if(!theSysParam->setSectionPath(subFilterPath)) {
           theErrorMessages->insert( EAPPFRM_SYSVAL,subFilterPath );
           return false;
        }
        while ( theSysParam->getSectionValue(xname, xvalue) )
        {
           theSortCdr->setInputField(xname, xvalue);
        }

        // initialize SorCdr's output field
        subFilterPath = filterPath + "\\output_field";

        if(!theSysParam->setSectionPath(subFilterPath)) {
           theErrorMessages->insert( EAPPFRM_SYSVAL,subFilterPath );
           return false;
        }
        while ( theSysParam->getSectionValue(xname, xvalue) )
        {
           theSortCdr->setOutputField(xname, xvalue);
        }

        if(!theSortCdr->initialize(theFilterRule,&m_PM)) {
          theErrorMessages->insert(PP_ERROR_INITIALIZE,"Initialize SortCdr error!");
          return false;
        }

        m_WriteStandardCdr.setPrefix(p,n,i,u);

        // initialize WriteStandardCdr
        filterPath=root+"\\channelNo"; //分发的通道
        if(!theSysParam->setSectionPath(filterPath)) 
        {
           theErrorMessages->insert( EAPPFRM_SYSVAL,filterPath );
           return false;
        }

        channel = 0;
        m_WriteStandardCdr.m_FilterFp.clear();
        while ( theSysParam->getSectionValue(xname, xvalue) )
        {
            strcpy(tmp,xname.c_str());
            if(isdigit(tmp[0]))
            {
                channel++;
            }
            m_WriteStandardCdr.setPath(xname,xvalue,chNo);//三个都是输入参数
        }
        (m_WriteStandardCdr.m_fp).push_back(m_WriteStandardCdr.m_FilterFp);

        m_SortCdr.push_back(theSortCdr);
        
        //下一个通道
        chNo++;
        sprintf(chanNo,"%d",chNo);
        filterPath  = root + "\\DataSource\\" + chanNo;
        name        = "sort_rule";
    }

    m_WriteStandardCdr.m_FilterFp.clear();
    theSortLog->initialize(channel);








    // initialize readStandardCdr
    filterPath  = root ;
    name        = "file_error_path";
    value.clear();
    if( !theSysParam->getValue(filterPath,name,value) ) 
    {
        theErrorMessages->insert( EAPPFRM_SYSVAL,name );
        return false;
    }

#ifdef DEBUG
cout<<"will enter m_ReadStandardCdr.initialize !"<<endl;
#endif

    if(!m_ReadStandardCdr.initialize(value)) 
    {
        theErrorMessages->insert(PP_ERROR_INITIALIZE,"Initialize ReadStandardCdr error!");
        return false;
    }


#ifdef DEBUG
cout<<"m_ReadStandardCdr.initialize ok!"<<endl;
#endif

    filterPath  = root ;
    name        = "updata_CDRs_limen";
    value.clear();
    if( theSysParam->getValue(filterPath,name,value) ) 
    {
        theSortLog->setUpdataCDRsLimen( atoi(value.c_str()) );
    } 
    else 
    {
        theSortLog->setUpdataCDRsLimen( 100000 );
    }

    name        = "updata_time_limen";
    value.clear();
    if( theSysParam->getValue(filterPath,name,value) ) 
    {
        theSortLog->setUpdataTimeLimen( atoi(value.c_str()) );
    } 
    else 
    {
        theSortLog->setUpdataTimeLimen( 600 );
    }

    //gethostname(); 
    m_PM.setHostName("");
    m_PM.setAppName(appName);               // 2002.4.9修改行
    m_PM.setChannelNo(0);
    
    

    if(!m_PM.initialization()) 
    {
       theErrorMessages->insert( PP_ERROR_INITIALIZE,"cannot initialize PM filter!" );

#ifdef DEBUG
cout<<"PM init error!"<<endl;
#endif

       return false;
    }
    
    
#ifdef DEBUG
cout<<"PM init ok!"<<endl;
#endif
    return true;
}//initialize



//##ModelId=3BDE8EC60389
SortApplication::SortApplication()
{
    m_runCDRs   = 0;
    m_runTimes  = 0;
}

//##ModelId=3BDE8EE502DA
SortApplication::~SortApplication()
{
    for(int i=0;i<m_SortCdr.size();i++)
    {
       delete m_SortCdr[i];
    }
    
    m_PM.end();
    
}

//##ModelId=3BDE9A5C03B6
SortCdr *SortApplication::createRuleObject(const string theSortRule)
{
    SortCdr *theSortCdr;

    if(theSortRule.compare("Rule_1")==0) {
        theSortCdr         = new SortRule_1;
    } else if(theSortRule.compare("Rule_2")==0) {
        theSortCdr         = new SortRule_2;
    } else if(theSortRule.compare("Rule_3")==0) {
        theSortCdr         = new SortRule_3;
    } else if(theSortRule.compare("Rule_4")==0) {
        theSortCdr         = new SortRule_4;
    } else if(theSortRule.compare("Rule_5")==0) {
        theSortCdr         = new SortRule_5;
    } else if(theSortRule.compare("Rule_6")==0) {
        theSortCdr         = new SortRule_6;
    }  else if(theSortRule.compare("Rule_Gprs")==0) {
        theSortCdr         = new SortRule_Gprs;
    }  else {
        #ifdef DEBUG
            cout<<"创建分拣对象失败!"<<endl;
        #endif
        theSortCdr=0;
    }


    return theSortCdr;

}

//##ModelId=3BDFAE3D0182
int SortApplication::reset(const string fileName,const int datasource)  // edit by zhangy in 2002.5.24
{
    if(!m_ReadStandardCdr.reset(fileName))
       return -1;                   // edit by zhangy in 2002.5.24，不可忽略错误

    if(!m_WriteStandardCdr.reset(fileName,datasource))
       return -1;                   // edit by zhangy in 2002.5.24，不可忽略错误

    theSortLog->clear();

#ifdef DEBUG
cout<<"updata..."<<flush;
#endif

    if(!updata()) 
    {
        theErrorMessages->insert( PP_ERROR_RESET,"updata PM filter error!" );
#ifdef DEBUG
cout<<"error!"<<endl;
#endif
        return 0;                   // edit by zhangy in 2002.5.24 ，可忽略错误
    }

#ifdef DEBUG
cout<<"ok!"<<endl;
#endif

    return 1;                       // edit by zhangy in 2002.5.24，成功
}

//##ModelId=3BDFB2340055
void SortApplication::close(int flag )
{
    m_ReadStandardCdr.close(flag);
    m_WriteStandardCdr.close();
}


//##ModelId=3C1CA2E40056
bool SortApplication::updata()
{
    // 从文件中取更新标志 change by liux 20040808
    //if(m_PM.needUpdate())
    
    if(m_PM.needUpdateForRate())
       return m_PM.update();

    
    //return true;
}

bool SortApplication::checkTokenCount(char token,const int datasource)
{
    int j=0;
    int len=strlen(m_cdrBuffer);

    for(int i=0;i<len;i++)
    {
       if(m_cdrBuffer[i]==token) {
            j++;
       }
    }

    if( m_cdrBuffer[len-1]!=token && j==m_fieldCount[datasource] ) {
        return false;
    }

    if( j == (m_fieldCount[datasource] - 1) ) {
        m_cdrBuffer[len]=token;
        m_cdrBuffer[len+1]=0;
    }

    if( j < (m_fieldCount[datasource] - 1) ) {
        return false;
    }

    return true;
}

