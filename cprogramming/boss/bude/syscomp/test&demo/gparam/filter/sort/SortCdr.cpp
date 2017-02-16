// ##########################################
// Source file : SortCdr.cpp
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020423
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "SortCdr.h"

//##ModelId=3BDD07E502EF
void SortCdr::setInputField(const string fieldName,const string position)
{
    InputField theInputField;
    
    theInputField.m_fieldName  = fieldName;
    theInputField.m_position   = atoi( position.c_str() );
    
    m_inputField.push_back(theInputField);
}


//##ModelId=3BDF727A00E1
void SortCdr::setOutputField(const string fieldName,const string position)
{
    InputField theInputField;
    
    theInputField.m_fieldName  = fieldName;
    theInputField.m_position   = atoi( position.c_str() );
    
    m_outputField.push_back(theInputField);
}

//##ModelId=3BDFD69000D1
int SortCdr::getField(const char *cdrBuffer, int position, char *fieldValue)
{
    int n,len,offset,offset1,fieldLen;

    n        =  1;
    offset   =  0;
    fieldLen = -1;
    
    len=strlen(cdrBuffer);

    while( ( offset1=getNextToken(cdrBuffer,offset,len) ) <= len )
    {
       fieldLen=offset1-offset;
       if(n==position) 
       {
          memcpy(fieldValue,cdrBuffer+offset,fieldLen);
          fieldValue[fieldLen]=0;
          break;
       }
       offset1=getNextToken(cdrBuffer,offset,len) ;
       offset+=(fieldLen+1);
       n++;
    }
    
    if(offset1>=len) 
    {
       fieldLen--;
    }
    fieldValue[fieldLen]=0;

    return fieldLen;
}//getField

//##ModelId=3BE20AB002F0
int SortCdr::getNextToken(const char *cdrBuffer,int offset,int len)
{
    int i;
    char token=',';
    
    for( i=offset; i<len; i++ )
    {
       if(cdrBuffer[i]==token)
          return i;
    }
    if(i==offset)
       return len+1;
    
    return len;
}

//##ModelId=3BE286F10295
bool SortCdr::initialize(string theFilterRule,PM_Filter *thePM) 
{
    m_FilterCdr=0;
    
    m_PM = thePM;
    
    strcpy(m_homeAreaCode,"022");
    
    if(theFilterRule.compare("Rule_1")==0) 
    {
       m_FilterCdr         = new FilterRule_1;
    } 
    else if(theFilterRule.compare("Rule_2")==0) 
    {
       m_FilterCdr         = new FilterRule_2;
    } 
    else 
    {
       theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "config file error!");
       return false;
    }
    
    if ( m_FilterCdr == 0 ) 
    {
       theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create FilterCdr object!");
       return false;
    }
    
    return true;
}

//##ModelId=3BE360D90005
SortCdr::~SortCdr()
{
   if(m_FilterCdr!=0)
      delete m_FilterCdr;
}

//##ModelId=3BE89AA50016
bool SortCdr::setFieldValue(int position, string value, bool flag)
{
    char token[2];
    char topTmp[1024],tailTmp[1024];
    int n,len,offset,offset1,fleldLen;

    n         =  1;
    offset    =  0;
    fleldLen  = -1;
    strcpy(token,",");
    
    
    len=strlen(m_cdrBuffer);

    if(position==0)                  // position=0 ，表示字段要添加(是boss.conf中配的)
    {                
        strcat(m_cdrBuffer,value.c_str());
        strcat(m_cdrBuffer,token);
    } 
    else if (position>0) 
    {                                // position>0 ，表示字段要修改
        if(value.size()==0) {        // add by zhangy in 2002.4.23
            return true;             // add by zhangy in 2002.4.23
        }                            // add by zhangy in 2002.4.23
        while( ( offset1=getNextToken(m_cdrBuffer,offset,len) ) <= len )
        {
            fleldLen=offset1-offset;
            if(n==position) {
               break;
            }
            offset+=(fleldLen+1);
            n++;
        }
        if(offset1==-1) 
        {
            return false;
        }
        if(!flag) { // true 无条件修改
            if(fleldLen > 0 ) {     // 如果原字段有值，则不做修改
                return true;
            }
        }

        memcpy(topTmp,m_cdrBuffer,offset);
        topTmp[offset]=0;
        
        memcpy(tailTmp,m_cdrBuffer+offset1,len-offset1);
        tailTmp[len-offset1]=0;
        
        sprintf(m_cdrBuffer,"%s%s%s",topTmp,value.c_str(),tailTmp);
    } 
    else 
    {
        return false;
    }

    return true;
}//setFieldValue

//##ModelId=3BDFD1E5010F
int SortCdr::getFields(const char *cdrBuffer)
{
    int len,count;
    char fieldBuffer[255],*tmp;
    static string IMSI("IMSI"),ERROR_CODE("ERROR_CODE"),START_DATE("START_DATE");
    static string CALL_TYPE("CALL_TYPE"),OTHER_PARTY("OTHER_PARTY");
    static string START_TIME("START_TIME"),OUT_TRUNKID("OUT_TRUNKID");
    static string IN_TRUNKID("IN_TRUNKID"),MSISDN("MSISDN");
    static string RATE_IND("RATE_IND"),MSC("MSC"),DATE_TIME("DATE_TIME");  
    static string ROAM_TYPE("ROAM_TYPE"),SOURCE_TYPE("SOURCE_TYPE");
    
    count = 0;
    len   = m_inputField.size();
    strcpy(m_cdrBuffer,cdrBuffer);
    
    strcpy(m_errorCode,  (char *)"000");             // edit by zhangy in 20021116
    memset(fieldBuffer,  0,sizeof(fieldBuffer)   );
    memset(m_IMSI,       0,sizeof(m_IMSI)        );
    memset(m_startDate,  0,sizeof(m_startDate)   );
    memset(m_startTime,  0,sizeof(m_startTime)   );
    memset(m_otherParty, 0,sizeof(m_otherParty)  );
    memset(m_dateTime,   0,sizeof(m_dateTime)    );
    memset(m_outTrunkId, 0,sizeof(m_outTrunkId)  );
    memset(m_inTrunkId,  0,sizeof(m_inTrunkId)   );
    memset(m_msisdn,     0,sizeof(m_msisdn)      );
    memset(m_rateInd,    0,sizeof(m_rateInd)     );
    memset(m_Msc,        0,sizeof(m_Msc)         );  
    memset(m_roamType,   0,sizeof(m_roamType)    ); 
    memset(m_sourceType, 0,sizeof(m_sourceType)  ); 
    
    for(int i=0;i<len;i++)
    {
    	  fieldBuffer[0]=0;
       if( m_inputField[i].m_fieldName.compare(IMSI) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_IMSI,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(ERROR_CODE) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_errorCode,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(START_DATE) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_startDate,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(CALL_TYPE) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          m_callType = atoi(fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(OTHER_PARTY) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_otherParty,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(START_TIME) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_startTime,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(OUT_TRUNKID) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_outTrunkId,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(IN_TRUNKID) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_inTrunkId,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(MSISDN) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          if( strncmp(fieldBuffer,"8613",4) == 0 ) {
             tmp=fieldBuffer+2;
          } else {
             tmp=fieldBuffer;
          }
          strcpy(m_msisdn,tmp);
       } else if( m_inputField[i].m_fieldName.compare(RATE_IND) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_rateInd,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(MSC) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_Msc,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(DATE_TIME) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_dateTime,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(ROAM_TYPE) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_roamType,fieldBuffer);
       } else if( m_inputField[i].m_fieldName.compare(SOURCE_TYPE) == 0 ) {
          getField( cdrBuffer, m_inputField[i].m_position, fieldBuffer );
          strcpy(m_sourceType,fieldBuffer);
       }
    }
    
    if(strlen(m_dateTime)==0) 
    {
       sprintf(m_dateTime,"%s%s",m_startDate,m_startTime);
    }
//   cout<<m_callType<<"\t"<<m_IMSI<<"\t"<<m_msisdn<<"\t"<<m_otherParty;
//   cout<<"\t"<<m_startDate<<"\t"<<m_startTime<<"\t"<<m_outTrunkId<<"\t"<<m_inTrunkId<<endl;

    return atoi(m_errorCode);
}//getFields

//##ModelId=3C02EC6B00DD
char * SortCdr::addFields(int errorNo)     // 用于追加空字段 addFields(0); addFields(INVALID_IMSI);
{
    char tmp[1],err[4];
    int i,len,position=0;
    static string ERR("ERROR_CODE"),MSISDN("MSISDN");

    if(errorNo!=0) 
    {
        len = m_inputField.size();
        for(i=0;i<len;i++)
        {
            if (m_inputField[i].m_fieldName.compare(ERR) == 0 ) 
            {
                position = m_inputField[i].m_position;
                break;
            }
        }
        sprintf(err,"%03d",errorNo);
        setFieldValue(position,err,true);
    }

    tmp[0]=0;
    len=m_outputField.size();
    for(i=0;i<len;i++)
    {
        setFieldValue( m_outputField[i].m_position , tmp , true);   // 无条件替换
    }

    return m_cdrBuffer;
}//addFields

//##ModelId=3C1DE24C0154
bool SortCdr::isTrashCdr(int &callType)
{
    callType = m_callType;

    return m_FilterCdr->isTrashCdr(m_callType,m_IMSI,m_otherParty,m_msisdn);
}

//##ModelId=3C1F30E40310
string SortCdr::countSum(int channel)
{
    char chan[10];
    string channelNo;

    sprintf(chan,"%d",channel);
    channelNo   = chan;
    
    if(channel<=theSortLog->getChannelCount()) 
    {
        theSortLog->addChannelSum(channel);
    } 
    else 
    {
        channelNo = CHANNEL_NOINFO;
    }

    return channelNo;
}

//##ModelId=3C1F4FF201B3
void SortCdr::bindInfo(ServInfo &servInfo,
                       ServInfo &servInfo_other)
{
    int len;
    char tmp[30];
    
    len=m_outputField.size();
    for(int i=0;i<len;i++)
    {
       tmp[0]=0;
       if ( m_outputField[i].m_fieldName.compare("MSISDN") == 0 ) {
         //changed by liux 国际手机号码字段有10位，强制覆盖原MSISDN 20041021
          if( strncmp(m_msisdn,"13",2)==0 && strlen(m_msisdn) !=11 )
             setFieldValue( m_outputField[i].m_position , servInfo.m_phoneNumber ,true );
          else
              setFieldValue( m_outputField[i].m_position , servInfo.m_phoneNumber ,false );
       }
       if ( m_outputField[i].m_fieldName.compare("IMSI") == 0 ) {
          setFieldValue( m_outputField[i].m_position , servInfo.m_imsi ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("SERVICE_TYPE") == 0 ) {
          tmp[0]=servInfo.m_servType;
          tmp[1]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("SERVICE_ID") == 0 ) {
          strcpy(tmp,servInfo.m_servId);
          setFieldValue( m_outputField[i].m_position , tmp,true );
       }
       if ( m_outputField[i].m_fieldName.compare("GROUP_ID") == 0 ) {
          sprintf(tmp,"%d",servInfo.m_groupId);
          setFieldValue( m_outputField[i].m_position , tmp,true );
       }
       if ( m_outputField[i].m_fieldName.compare("CHANNEL_NO") == 0 ) {
          sprintf(tmp,"%d",servInfo.m_channelNo);
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("CUST_ID") == 0 ) {
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("OFFICE_CODE") == 0 ) {
          strcpy(tmp,servInfo.m_eparchy);
          tmp[3]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("BRANDTYPE") == 0 ) {
          tmp[0]=servInfo_other.m_servType;
          tmp[1]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       
    }
}


//##ModelId=3C1F4FF30056
void SortCdr::bindInfo(ServInfo2 &servInfo2,
                       ServInfo  &servInfo_other)
{
    int len;
    char tmp[30];
    
    len=m_outputField.size();
    for(int i=0;i<len;i++)
    {
       tmp[0]=0;
       if ( m_outputField[i].m_fieldName.compare("MSISDN") == 0 ) {
          setFieldValue( m_outputField[i].m_position , servInfo2.m_phoneNumber ,false);
       }
       if ( m_outputField[i].m_fieldName.compare("SERVICE_TYPE") == 0 ) {
          tmp[0]=servInfo2.m_servType;
          tmp[1]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("SERVICE_ID") == 0 ) {
          strcpy(tmp,servInfo2.m_servId);
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("GROUP_ID") == 0 ) {
          sprintf(tmp,"%d",servInfo2.m_groupId);
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("CHANNEL_NO") == 0 ) {
          sprintf(tmp,"%d",servInfo2.m_channelNo);
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("CUST_ID") == 0 ) {
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("OFFICE_CODE") == 0 ) {
          strcpy(tmp,servInfo2.m_eparchy);
          tmp[3]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
       if ( m_outputField[i].m_fieldName.compare("BRANDTYPE") == 0 ) {
          tmp[0]=servInfo_other.m_servType;
          tmp[1]=0;
          setFieldValue( m_outputField[i].m_position , tmp ,true);
       }
    }
}

int SortCdr::getError()
{
    return atoi(m_errorCode);
}

