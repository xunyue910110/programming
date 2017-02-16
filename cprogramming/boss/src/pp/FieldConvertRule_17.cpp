// ##########################################
// Source file : FieldConvertRule_17.cpp
// System      : Mobile Billing System
// Version     : 0.1.16
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020526
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_17.h"
#include "FieldInfos.h"
#include "AlertCode.h"
#include <unistd.h>
#include <sys/stat.h>

string FieldConvertRule_17::m_lastFileName = "";
string FieldConvertRule_17::m_thisFileName = "";
string FieldConvertRule_17::m_lastTmpFileName = "";
string FieldConvertRule_17::m_thisTmpFileName = "";
FILE * FieldConvertRule_17::m_fDes=0;
string FieldConvertRule_17::m_PREFIX = "FORCECUT_";
//##ModelId=3BAAEABE023D
bool FieldConvertRule_17::initialize(string ruleText)
{
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;
    //rule_2:CALL_TYPE=08;MSRN_FLAG=91;//data//aa//cc//
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		return false;	
	}
	
	// 提取规则中的信息
	tmpStrs[0]=tmpStrs[0].substr(8);
	if (tmpStrs.size() == 2)
	{
	    if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    {
	        m_callType = "ALL";
	        m_conditionFieldName = "ALL";
	    }
	    else if(tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        if (tmpStrs1.size() == 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_callType = tmpStrs1[1];
	            m_conditionFieldName = "ALL";
	        }
	    }
	    else
	    {
	        strcpy(cTmp,tmpStrs[0].c_str());
	        if (strstr(cTmp,"!=")!=NULL)
	        {
	            m_conditionExp = "!=";
	        }
	        else if(strstr(cTmp,">=")!=NULL)
	        {
	            m_conditionExp = ">=";
	        }
	        else if(strstr(cTmp,"<=")!=NULL)
	        {
	            m_conditionExp = "<=";
	        }
	        else if(strstr(cTmp,"=")!=NULL)
	        {
	            m_conditionExp = "=";
	        }
	        else
	        {
	            return false;
	        }
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
	        
	        m_conditionFieldName = tmpStrs1[0];
	        m_conditionFieldValue = tmpStrs1[1];
	        m_callType = "ALL";
	    }
	    m_tmpPath =  tmpStrs[1];
	    m_desPath =  tmpStrs[1];
	    if( m_tmpPath.compare(m_tmpPath.length()-2,2,"//") != 0)
	    {
	        m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
        }
        else
        {
            m_tmpPath.erase(m_tmpPath.find_last_of("//")-1,m_tmpPath.npos);
            m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
        }
        getFieldObj();
	    return true;
	}
	else if(tmpStrs.size() == 3)
	{
	    if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];   
	    }
	    else
	    {
	        return false;
	    }
	    
	    strcpy(cTmp,tmpStrs[1].c_str());
	    if (strstr(cTmp,"!=")!=NULL)
	    {
	        m_conditionExp = "!=";
	    }
	    else if(strstr(cTmp,">=")!=NULL)
	    {
	        m_conditionExp = ">=";
	    }
	    else if(strstr(cTmp,"<=")!=NULL)
	    {
	        m_conditionExp = "<=";
	    }
	    else if(strstr(cTmp,"=")!=NULL)
	    {
	        m_conditionExp = "=";
	    }
	    else
	    {
	        return false;
	    }
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],m_conditionExp.c_str(),tmpStrs1);
	        
	    m_conditionFieldName = tmpStrs1[0];
	    m_conditionFieldValue = tmpStrs1[1];
	    
	    m_tmpPath =  tmpStrs[2];
	    m_desPath =  tmpStrs[2];
	    if( m_tmpPath.compare(m_tmpPath.length()-2,2,"//") != 0)
	    {
	        m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
        }
        else
        {
            m_tmpPath.erase(m_tmpPath.find_last_of("//")-1,m_tmpPath.npos);
            m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
        }
        getFieldObj();
	    return true;
	}
	else
	{
	    return false;
	}
}


FieldConvertRule_17::~FieldConvertRule_17()
{
    if( m_fDes != 0 )
	{ 
	   string tmpFileName = m_tmpPath + m_PREFIX + string(m_PpLog->getSrcFileName());
	   //关闭输出文件 
	   if( fclose(m_fDes)!=0 )
	   {
	        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                     "程序退出时,关闭文件"+tmpFileName+"失败!");
        }
        m_fDes = 0;
	   //把正在处理的临时文件提交
	   struct stat attr;
	   stat(tmpFileName.c_str(), &attr);
	   if(attr.st_size==0)          // 如果文件长度为零，删除该文件
	   {                  
	        string rmCommand="rm "+tmpFileName;
	        system( rmCommand.c_str() );
	   }
	   else
	   {
	        string desFileName = m_desPath + m_PREFIX + string(m_PpLog->getSrcFileName());
	        if( link(tmpFileName.c_str(),desFileName.c_str())!=0 ) 
	        {
	            string repeatDesName;
	            repeatDesName = desFileName + ".01";
	            //如果是由于stdpath目录下有重名文件而link不成功的话,该成"*.01"重试,如果还不成功,则告警
	            if( link(tmpFileName.c_str(),repeatDesName.c_str())!=0 )
	            { 
		    	    theErrorMessages->insert(PP_ERROR_LINK_FILE,"File"+tmpFileName+"不能移动到输出目录"+m_desPath);
	        	}
	        	else
	        	{
	        		theErrorMessages->insert(PP_ERROR_LINK_FILE,"File"+tmpFileName+"在输出目录"+m_desPath+"有重复!新文件改名成:"+repeatDesName+"输出");
	        		if( unlink( tmpFileName.c_str() )!=0)   //删除
                    {         
                       theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                                  "临时文件"+tmpFileName+"不能删除!");
		            }
	        	}
	        }
	        else if( unlink( tmpFileName.c_str() )!=0)   //删除
            {         
               theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                          "临时文件"+tmpFileName+"不能删除!");
		    }
	    }
	}
}

bool FieldConvertRule_17::convertField()
{
	string inputStr;
	struct stat attr;
	
	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

    //zaiww added 20060908 将"手机号,开始日期,开始时间,通话时长"输出到与原始文件同名的文件中
    //rule_16;ALL;DURATION>=45;//gboss//dev//billing//src//pp//pptest//zaiww_test//
	//拼出输出字符串
	inputStr.clear();
	inputStr =  m_pFieldMsisdn->m_FieldDesValue==NULL?"":m_pFieldMsisdn->m_FieldDesValue;
	inputStr +=  string(",") + (m_pFieldStartDate->m_FieldDesValue==NULL?"":m_pFieldStartDate->m_FieldDesValue);
	inputStr +=  string(",") + (m_pFieldStartTime->m_FieldDesValue==NULL?"":m_pFieldStartTime->m_FieldDesValue);
	inputStr +=  string(",") + (m_pFieldDu->m_FieldDesValue==NULL?"":m_pFieldDu->m_FieldDesValue);
	
	inputStr +=  "\n";
	
	//正在处理的文件
	m_thisFileName = m_PREFIX + string(m_PpLog->getSrcFileName());
	
	//正在处理的临时输出文件,带路径
	m_thisTmpFileName  =  m_tmpPath + m_thisFileName;
	
	if( m_thisFileName != m_lastFileName )
	{
	    if( m_fDes != 0 )
	    { 
	       //关闭输出文件
	       if( fclose(m_fDes)!=0 )
	       {
	            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                         "致命错误：关闭文件"+m_lastFileName+"失败，程序退出 !");
                exit(1);
           }
           m_fDes=0;
           
	       //把上次的临时文件提交
	       m_lastTmpFileName  =  m_tmpPath + m_lastFileName;
	       stat(m_lastTmpFileName.c_str(), &attr);
	       if(attr.st_size==0)          // 如果文件长度为零，删除该文件
	       {                  
	            string rmCommand="rm "+m_lastTmpFileName;
	            system( rmCommand.c_str() );
	       }
	       else
	       {
	            string desFileName = m_desPath + m_lastFileName;
	            if( link(m_lastTmpFileName.c_str(),desFileName.c_str())!=0 ) 
	            {
		            string repeatDesName;
		            repeatDesName = desFileName + ".01";
		            //如果是由于stdpath目录下有重名文件而link不成功的话,该成"*.01"重试,如果还不成功,告警
		            if( link(m_lastTmpFileName.c_str(),repeatDesName.c_str())!=0 )
		            { 
			    	    theErrorMessages->insert(PP_ERROR_LINK_FILE,"File "+m_lastTmpFileName+"不能移动到输出目录"+m_desPath);
		        	}
		        	else
		        	{
		        		theErrorMessages->insert(PP_ERROR_LINK_FILE,"File "+m_lastFileName+"在输出目录"+m_desPath+"有重复!新文件改名成:"+repeatDesName+"输出");
			    	    if( unlink( m_lastTmpFileName.c_str() )!=0)   //删除
                        {         
                           theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                                      "临时文件"+m_lastTmpFileName+"不能删除!");
		                }
		        	}  
		        }
		        else if( unlink( m_lastTmpFileName.c_str() )!=0)   //删除
                {         
                   theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                              "临时文件"+m_lastTmpFileName+"不能删除!");
		        }       
		    }           
	    }               
        
        //打开输出文件
        if ( (m_fDes=fopen(m_thisTmpFileName.c_str(),(char *)"w")) == NULL )
        {
            theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"ConvertRule_17:open output file("+m_thisTmpFileName+")  error!");
	        return false;		
	    }

	}
	//输出字符串
	if ( fputs(inputStr.c_str(),m_fDes) == EOF ) 
	{
	    theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"ConvertRule_17:write output file("+m_thisTmpFileName+")  error!");
	    return false;	
	}

	m_lastFileName = m_thisFileName;
	
	return true;
}

void FieldConvertRule_17::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    m_pFieldMsisdn    = m_pFieldInfos->getFieldInfoByName(MSISDN);
    m_pFieldDu        = m_pFieldInfos->getFieldInfoByName(DURATION);

	return FieldConvertRule::getFieldObj();		
}
