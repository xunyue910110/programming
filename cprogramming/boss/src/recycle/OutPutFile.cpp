
#include "OutPutFile.h"
#include "base/StringUtil.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

bool OutPutFile::initialize(vector<OutputRule>* outputRuleList,
     const char *dealTime,const int maxCdrNum,RecConfig * config)
{ 
    m_pOutputRule = outputRuleList;
    m_dealTime    = dealTime;
    m_maxCdrNum   = maxCdrNum;
    m_config       = config;
    m_channelList.clear();
    if (m_maxCdrNum <= 0)   m_maxCdrNum   = MAXCDRNUM;
    Channe_Def tmpChannel;
    for (vector<OutputRule>::iterator itr = m_pOutputRule->begin();itr != m_pOutputRule->end();itr++)
    {
        tmpChannel.m_fileNo = -1;
        tmpChannel.m_cdrNum = 0;
     tmpChannel.m_ifFull= false;
        tmpChannel.m_stdFilePath = itr->m_outputFilePath;
        tmpChannel.m_bakFilePath = itr->m_bakFilePath;
     //tmpChannel.m_billFilePath = itr->m_billFilePath;
        tmpChannel.m_tmpFileList.clear();
        m_channelList.push_back(tmpChannel);
    }
    
    return true;
}

bool OutPutFile::setDealTime(const char *dealTime)
{    
    m_dealTime    = dealTime;
    Channe_Def tmpChannel;
    for (vector<OutputRule>::iterator itr = m_pOutputRule->begin();
         itr != m_pOutputRule->end();itr++)
    {
        tmpChannel.m_fileNo = -1;
        tmpChannel.m_cdrNum = 0;
        tmpChannel.m_stdFilePath = itr->m_outputFilePath;
        tmpChannel.m_bakFilePath = itr->m_bakFilePath;
        tmpChannel.m_tmpFileList.clear();
        m_channelList.push_back(tmpChannel);
    }
    return true;
}

bool OutPutFile::putCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC])
{
    /*int matchNum = 0;
    for (m_ruleItr = m_pOutputRule->begin();m_ruleItr != m_pOutputRule->end();m_ruleItr++)
    {
        for (vector<string>::iterator itr = m_ruleItr->m_sourceType.begin();itr != m_ruleItr->m_sourceType.end();itr++)
        {
            if (strcmp(itr->c_str(), cdr[m_ruleItr->m_sourceTypeIndex-1])==0)
            {
                matchNum ++;
                cout << " matchNum : " << matchNum << endl;
            }
        }   
    }
    
    if (matchNum == 0)
    {
        m_errMsg = "未知数据源SOURCE_TYPE : ";
        m_errMsg += cdr[m_ruleItr->m_sourceTypeIndex-1];
        LOG_TRACE((*theLogger),m_errMsg.c_str());
        theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
    }
    else if (matchNum > 1)
    {
        m_errMsg = "数据源SOURCE_TYPE : ";
        m_errMsg += cdr[m_ruleItr->m_sourceTypeIndex-1];
        m_errMsg += " 重复配置";
        LOG_TRACE((*theLogger),m_errMsg.c_str());
        theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
    }
    */
    
    bool ret = false;
    for (m_ruleItr = m_pOutputRule->begin();m_ruleItr != m_pOutputRule->end();m_ruleItr++)
    {
        for (vector<string>::iterator itr = m_ruleItr->m_sourceType.begin();
             itr != m_ruleItr->m_sourceType.end();itr++)
        {
            if (strcmp(itr->c_str(), cdr[m_ruleItr->m_sourceTypeIndex-1])==0)
            {
                ret = true;
                break;
            }
        }   
        if (ret == true)    break;
    }
    
    if (m_ruleItr == m_pOutputRule->end())
    {   //对应的sourceType找不到，则将该话单输出到错单文件中去
        LOG_TRACE_P1((*theLogger),"未知数据源SOURCE_TYPE:%s",cdr[m_ruleItr->m_sourceTypeIndex-1]);
        char test_tmp[1000];
        sprintf(test_tmp,"测试话单:%s,%s,%s,%s,%s,%s,%s,%s",cdr[0],cdr[1],cdr[2],cdr[3],cdr[4],cdr[5],cdr[6],cdr[7]);
        theErrorMessages->insert(ERROR_OUTPUTCDR,test_tmp);
        theErrorMessages->insert(ERROR_OUTPUTCDR,"对应的sourceType找不到，则将该话单输出到错单文件中去");
        return false;
    }
    
    //将cdr压入m_lable对应的通道中，标准化并输出
    m_lable = m_ruleItr - m_pOutputRule->begin();
    m_channelItr = m_channelList.begin()+m_lable;
    
    if (writeCdr(cdr) == false)
    {
        theErrorMessages->insert(ERROR_OUTPUTCDR,"输出错误!");
        return false;
    }

    return true;
}

bool OutPutFile::writeCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC])
{
    vector<FieldOutputRule>::iterator itr;
    int     firstField;
    char   *p=NULL;
    
    m_cdrString="";
    firstField =1;   //表示下一个输出字段是否为第一个字段,目的给后面的字段加上分割符号)1：是

    m_channelItr->m_ifFull = false;
//    cout << endl;
    for (itr =m_ruleItr->m_filedOutputRules.begin();itr!=m_ruleItr->m_filedOutputRules.end();itr++)
    {
        if(itr->m_fieldId <= 0)
            m_fieldValue = "";
        else
            m_fieldValue=cdr[itr->m_fieldId - 1];
//        cout <<itr->m_fieldId << ":" << m_fieldValue << ",";
        p=NULL;
        switch(itr->m_mode) {
            case LEFT_ZEAO:   //左补零
                sprintf(m_format,"%%0%ds",itr->m_fieldLen);
                sprintf(m_fieldBuff,m_format,m_fieldValue.c_str());
                break;
            case LEFT_SPACE:  //左补空格
                sprintf(m_fieldBuff,"%s",m_fieldValue.c_str());
                break;
            case RIGHT_SPACE: //右补空格
                sprintf(m_fieldBuff,"%s",m_fieldValue.c_str());
                break;
            case TRIM_SPACE:  //截取空格
                if(itr->m_fieldId <= 0)
                    sprintf(m_fieldBuff,"%s",m_fieldValue.c_str());
                else
                {
                    StringUtil::trim(cdr[itr->m_fieldId - 1]);
                    p = cdr[itr->m_fieldId - 1];
                    sprintf(m_fieldBuff,"%s",p);
                }
                break;
            case NO_DEAL:     //不变输出 
                sprintf(m_fieldBuff,"%s",m_fieldValue.c_str());
                break;
            default:          //不变输出 
                sprintf(m_fieldBuff,"%s",m_fieldValue.c_str());
                break;
        }

        if (m_ruleItr->m_format==SEPARATE) //分割符格式
        {   
            if (firstField!=1)
                m_cdrString+=m_ruleItr->m_fieldSeparator;
        }
        m_cdrString+=m_fieldBuff;
        firstField=0;
    }
    m_cdrString+=m_ruleItr->m_lineSeparator;
    int i =0 ;
    m_cdrString_raw  = "";
    for(i=0;i<m_config->m_cdrFormat[m_config->m_cdrFormatIndex].m_maxFieldNum;++i) {
      m_cdrString_raw += cdr[i];
      if (i != (m_config->m_cdrFormat[m_config->m_cdrFormatIndex].m_maxFieldNum -1))
        m_cdrString_raw += ",";
    }
    
    if (!(m_channelItr->m_ofstream.rdbuf()->is_open()))
    {
        string tmpStr;
        char   tmpFileNo[10+1];
        RedoFileName redoFileName;
        m_channelItr->m_fileNo++;
        sprintf(tmpFileNo,"%04d",m_channelItr->m_fileNo);
        if(m_config->m_needRollbackBill)
            m_channelItr->m_fileName = m_ruleItr->m_fileHead+ m_config->m_channelNo + "_" +m_dealTime+"."+tmpFileNo+"."+POSTFIX_REDO;
        else
            m_channelItr->m_fileName = m_ruleItr->m_fileHead+ m_config->m_channelNo + "_" +m_dealTime+"."+tmpFileNo+"."+POSTFIX_RCYL;
        //Modified by Sunam 2006-3-14 14:35
        //m_channelItr->m_tmpFileList.push_back(m_channelItr->m_fileName);
        redoFileName.m_stdFileName = m_channelItr->m_fileName;
        
        tmpStr = m_channelItr->m_bakFilePath+PREFIX+m_channelItr->m_fileName;
        
        string rawFileName = tmpStr + ".RAW";
        m_channelItr->m_ofstream_raw.open(rawFileName.c_str(), ios::out);
        if (!m_channelItr->m_ofstream_raw)
        {
            theErrorMessages->insert(ERROR_OUTPUTCDR,rawFileName + " fail!");   
            return false;
        }
        
        //Modified by Sunam 2006-3-14 11:51 FOR DR
        //tmpStr = m_channelItr->m_bakFilePath+PREFIX+m_channelItr->m_fileName;
/*        if (m_config->m_drOutPut)
        {
            sprintf(tmpFileNo,"%08d",m_config->m_sequenceNo);
            //tmpStr = m_channelItr->m_bakFilePath + PREFIX + m_ruleItr->m_fileHead + m_config->m_channelNo + "_" + tmpFileNo + StringUtil::toString(m_config->m_cycleDate).substr(2,4) + m_dealTime.substr(2,4);
            tmpStr = m_ruleItr->m_fileHead + m_config->m_channelNo + "_" + tmpFileNo + StringUtil::toString(m_config->m_cycleDate).substr(2,4) + m_dealTime.substr(2,4);
            sprintf(tmpFileNo,"%04d",m_channelItr->m_fileNo);
            tmpStr = tmpStr + "." + tmpFileNo + "." + POSTFIX;
            redoFileName.m_orgFileName = tmpStr;
            tmpStr = m_channelItr->m_bakFilePath + PREFIX + tmpStr;
        }
        else
        {*/
            redoFileName.m_orgFileName = m_channelItr->m_fileName;
//        }
        m_channelItr->m_tmpFileList.push_back(redoFileName);

        m_channelItr->m_ofstream.open(tmpStr.c_str(), ios::out);
        if (!m_channelItr->m_ofstream)
        {
            theErrorMessages->insert(ERROR_OUTPUTCDR,tmpStr + " fail!");   
            return false;
        }
    }
    
    //Added by Sunam 2006-3-14 11:47 UID文件处理
/*    if (m_config->m_drOutPut)
    {
        if (!(m_uidFileOfstream.rdbuf()->is_open()))
        {
            char tmp[200];
            string uidFileName;
            memset(tmp,0,sizeof(tmp));
            sprintf(tmp,"%s%03s%08d%s%s",m_ruleItr->m_fileHead.c_str(),m_config->m_channelNo.c_str(),m_config->m_sequenceNo,StringUtil::toString(m_config->m_cycleDate).substr(2,4).c_str(),m_dealTime.substr(2,4).c_str());
            m_uidFileName = "UIR";
            m_uidFileName = m_uidFileName + tmp;
            uidFileName = m_config->m_drOutPutPath + PREFIX + m_uidFileName;
            m_uidFileOfstream.open(uidFileName.c_str(), ios::out);
        }
        //输出uid信息
    }*/
    
    m_channelItr->m_ofstream<<m_cdrString;
    
    m_channelItr->m_ofstream_raw << m_cdrString_raw << endl;

    m_channelItr->m_cdrNum++;
    if (m_channelItr->m_cdrNum >= m_maxCdrNum)
    {
        m_channelItr->m_ifFull = true;
        //达到m_maxCdrNum话单数后，提交
        m_channelItr->m_cdrNum = 0;
        m_channelItr->m_ofstream.close();
        m_channelItr->m_ofstream_raw.close();
        m_channelItr->m_ofstream.clear();
        m_channelItr->m_ofstream_raw.clear();
    }
    return  true;
}


bool OutPutFile::commit()
{
    bool isNeedRawFile;
    string bakFileName,stdFileName,tmpFileName,rawFileName,tmpFileName_raw,bakFileName_raw,tmpRawFile;
    
    for (m_channelItr = m_channelList.begin();m_channelItr!= m_channelList.end();m_channelItr++)
    {
        if (m_channelItr->m_fileNo < 0)
        {
            continue;
        }
        for (vector<RedoFileName>::iterator itr=m_channelItr->m_tmpFileList.begin();itr != m_channelItr->m_tmpFileList.end();itr++)
        {
            bakFileName_raw = m_channelItr->m_bakFilePath+(*itr).m_stdFileName + ".RAW";
            bakFileName = m_channelItr->m_bakFilePath+(*itr).m_orgFileName;
            stdFileName = m_channelItr->m_stdFilePath+(*itr).m_orgFileName;
            tmpRawFile = m_config->m_rawDataPathTemp +(*itr).m_stdFileName + ".RAW";
            rawFileName = m_config->m_rawDataPath +(*itr).m_stdFileName + ".RAW";
#ifdef _DEBUG_
cout <<"bakFileName_raw:" <<bakFileName_raw <<endl;
cout <<"bakFileName:" <<bakFileName <<endl;
cout <<"stdFileName:" <<stdFileName <<endl;
cout <<"tmpRawFile:" <<tmpRawFile <<endl;
cout <<"rawFileName:" <<rawFileName <<endl;
#endif            
            if (!access(bakFileName_raw.c_str(),F_OK))
            {
                m_errMsg = "文件" + bakFileName_raw + "已存在!";
                LOG_TRACE((*theLogger),m_errMsg.c_str());
                theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
                return false;
            }
            
            if (!access(bakFileName.c_str(),F_OK))
            {
                m_errMsg = "文件" + bakFileName + "已存在!";
                LOG_TRACE((*theLogger),m_errMsg.c_str());
                theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
                return false;
            }
            
            if (!access(stdFileName.c_str(),F_OK))
            {
                m_errMsg = "文件" + stdFileName + "已存在!";
                LOG_TRACE((*theLogger),m_errMsg.c_str());
                theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
                return false;
            }
            
            if (!access(rawFileName.c_str(),F_OK))
            {
                m_errMsg = "文件" + stdFileName + "已存在!";
                LOG_TRACE((*theLogger),m_errMsg.c_str());
                theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
                return false;
            }
        }
    }
    
    for (m_channelItr = m_channelList.begin();m_channelItr!= m_channelList.end();m_channelItr++)
    {
        if (m_channelItr->m_ofstream.rdbuf()->is_open())
        {
            m_channelItr->m_ofstream.close();
            m_channelItr->m_ofstream.clear();
        }
        if (m_channelItr->m_ofstream_raw.rdbuf()->is_open())
        {
            m_channelItr->m_ofstream_raw.close();
            m_channelItr->m_ofstream_raw.clear();
        }
        if (m_channelItr->m_fileNo < 0)
        {
            continue;
        }
        
        for (vector<RedoFileName>::iterator itr=m_channelItr->m_tmpFileList.begin();itr != m_channelItr->m_tmpFileList.end();itr++)
        {
            tmpFileName = m_channelItr->m_bakFilePath+PREFIX+(*itr).m_orgFileName;
            tmpFileName_raw = m_channelItr->m_bakFilePath+PREFIX+(*itr).m_stdFileName + ".RAW";
            bakFileName_raw = m_channelItr->m_bakFilePath+(*itr).m_stdFileName + ".RAW";
            bakFileName = m_channelItr->m_bakFilePath+(*itr).m_orgFileName;
            stdFileName = m_channelItr->m_stdFilePath+(*itr).m_orgFileName;
            //rawFileName = m_config->m_rawDataPath +(*itr) + ".RAW";
#ifdef _DEBUG_
cout <<"tmpFileName:" <<tmpFileName <<endl;
cout <<"tmpFileName_raw:" <<tmpFileName_raw <<endl;
cout <<"bakFileName_raw:" <<bakFileName_raw <<endl;
cout <<"bakFileName:" <<bakFileName <<endl;
cout <<"stdFileName:" <<stdFileName <<endl;
#endif    

            if(m_config->m_rawDataPath.length() > 0)
                    {
                        isNeedRawFile = true;
                rawFileName = m_config->m_rawDataPath +(*itr).m_stdFileName + ".RAW";
            }
            else
            {
                isNeedRawFile = false;
            }
            
            if (rename(tmpFileName.c_str(),bakFileName.c_str()) != 0)
            {
                unlink(tmpFileName.c_str());
                return false;
            }
            /*
            if (rename(tmpFileName_raw.c_str(),rawFileName.c_str()) != 0)
            {
                unlink(tmpFileName_raw.c_str());
                return false;
            }
            */
            if (rename(tmpFileName_raw.c_str(),bakFileName_raw.c_str()) != 0)
            {
                unlink(tmpFileName_raw.c_str());
                return false;
            }
            if (isNeedRawFile)
            {
                if (link(bakFileName_raw.c_str(),rawFileName.c_str()) != 0)
                {
                    if(copy(bakFileName_raw.c_str(),tmpRawFile.c_str())!=0)
                    {
                        #ifdef _DEBUG_
                            cout<<"link "<<bakFileName_raw<<" "<<tmpRawFile<<" false!"<<endl;
                            cout<<"copy "<<bakFileName_raw<<" "<<tmpRawFile<<" false!"<<endl;
                        #endif
                        unlink(bakFileName_raw.c_str());
                        return false;
                    }
                      
                    if (rename(tmpRawFile.c_str(),rawFileName.c_str()))
                    {
                        #ifdef _DEBUG_
                            cout<<"rename "<<tmpRawFile<<" "<<rawFileName<<" false!"<<endl;
                          #endif
                        return false;
                    }
                }
            }
            
            if (bakFileName != stdFileName)
            {
                if (link(bakFileName.c_str(),stdFileName.c_str()) != 0)
                {
                    unlink(bakFileName.c_str());
                    return false;
                }
            }
            LOG_TRACE_P1((*theLogger),"清单:%s",stdFileName.c_str());
            LOG_TRACE_P1((*theLogger),"原始清单:%s",rawFileName.c_str());
        }
        //m_channelItr->m_tmpFileList.clear();
    }
    
    //Added by Sunam 2006-3-14 13:37 For Dr 提交uid文件
    /*if (m_uidFileOfstream.rdbuf()->is_open())
    {
        m_uidFileOfstream.close();
        m_uidFileOfstream.clear();
        tmpFileName = m_config->m_drOutPutPath + PREFIX + m_uidFileName;
        stdFileName = m_config->m_drOutPutPath + m_uidFileName;
        if (rename(tmpFileName.c_str(),stdFileName.c_str()) != 0)
        {
            unlink(tmpFileName.c_str());
            return false;
        }
    }*/
    
    return true;
}

bool OutPutFile::rollBack()
{
    string tmpFileName;
    for (m_channelItr = m_channelList.begin();m_channelItr!= m_channelList.end();m_channelItr++)
    {
        if (m_channelItr->m_fileNo < 0)
        {
            continue;
        }
        for (vector<RedoFileName>::iterator itr=m_channelItr->m_tmpFileList.begin();itr != m_channelItr->m_tmpFileList.end();itr++)
        {
            tmpFileName = m_channelItr->m_bakFilePath+PREFIX+(*itr).m_stdFileName;
            if(unlink(tmpFileName.c_str()))
            {
                return false;
            }
            tmpFileName = m_channelItr->m_bakFilePath+PREFIX+(*itr).m_orgFileName;
            if(unlink(tmpFileName.c_str()))
            {
                return false;
            }
        }
    }
    return true;
}

bool OutPutFile::end()
{
    m_pOutputRule = NULL;
    for (vector<Channe_Def>::iterator itr = m_channelList.begin();
         itr != m_channelList.end();itr++)
    {
        itr->m_tmpFileList.clear();
    }
    return true;    
}

int OutPutFile::copy(const char *r_srcFile,const char *r_desFile) const
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