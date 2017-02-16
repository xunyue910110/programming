// ##########################################
// Source file : FieldConvertRule_16.cpp
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

#include "FieldConvertRule_16.h"
#include "AlertCode.h"
#include <unistd.h>
#include <sys/stat.h>

string FieldConvertRule_16::m_lastFileName = "";
string FieldConvertRule_16::m_thisFileName = "";
string FieldConvertRule_16::m_lastTmpFileName = "";
string FieldConvertRule_16::m_thisTmpFileName = "";
FILE * FieldConvertRule_16::m_fDes=0;
string FieldConvertRule_16::m_PREFIX = "FORCECUT_";
//##ModelId=3BAAEABE023D
bool FieldConvertRule_16::initialize(string ruleText)
{
	char cTmp[50];

	vector<string> tmpStrs,tmpStrs1;
    //rule_2;08;MSRN_FLAG=91;//data//aa//cc//
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_16',get "08"
    
    // 规则2中除呼叫类型外的条件初始化:MSRN_FLAG=91
    if(strcmp(tmpStrs[2].c_str(),"ALL")==0) 
    {
    	m_conditionFieldName = "ALL";
    } 
    else 
    {
        // 得到条件中的运算符
        strcpy(cTmp,tmpStrs[2].c_str());
        if(strstr(cTmp,"!=")!=NULL) 
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
        // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
        splitStrByToken(tmpStrs[2],m_conditionExp.c_str(),tmpStrs1);
        if(tmpStrs1.size()!=2)
        {
        	return false;	
        }
        else
        {
        	m_conditionFieldName  = tmpStrs1[0];
        	m_conditionFieldValue = tmpStrs1[1];	
        }	
	}
	m_tmpPath =  tmpStrs[3];
	m_desPath =  tmpStrs[3];
	if( m_tmpPath.compare(m_tmpPath.length()-2,2,"//") != 0)
	{
	    m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
    }
    else
    {
        m_tmpPath.erase(m_tmpPath.find_last_of("//")-1,m_tmpPath.npos);
        m_tmpPath.erase(m_tmpPath.find_last_of("//")+1,m_tmpPath.npos);
    }
	return true;
}

FieldConvertRule_16::~FieldConvertRule_16()
{
    if( m_fDes != 0 )
	{ 
	   string tmpFileName = m_tmpPath + m_PREFIX + m_PpLog->getSrcFileName();
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
	        string desFileName = m_desPath + m_PREFIX + m_PpLog->getSrcFileName();
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

bool FieldConvertRule_16::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,len1,len2;
	int l_field,l_value;
	char tmp[100],tmp1[100];
	static string dstFieldString,dstFieldName;
	FieldInfo *tmpFieldInfo;
	static string callType("CALL_TYPE"),all("ALL");
	static string add1("+="),add2("-="),add3("="),add4("=+");
	string cmd,inputStr;
	struct stat attr;
	
	if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
		return false;
	
	// 条件1：通话类型
	if(m_callType.compare(all)!=0) 
	{
	   if(tmpFieldInfo->m_FieldDesValue.compare(m_callType)!=0) 
	   {
	      return false;
	   }
	}

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all) != 0)                // not is "ALL" 
	{            
	    //若条件2的右键是定义的字段名,则取该字段的值进行比较,否则直接用右键的值进行比较
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldValue)) == 0 ) 
	    {
	        dstFieldString = m_conditionFieldValue;
	    } 
	    else 
	    {
            dstFieldString = tmpFieldInfo->m_FieldDesValue;
        }
        
        //若条件2的左键不是已定义的字段名,返回false
        if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		//  字段值不是"ALL"
		if(m_conditionFieldValue.compare(all)!=0)               
		{
		    if(m_conditionExp.compare("!=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(dstFieldString)==0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare("=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(dstFieldString)!=0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare(">=")==0) 
		    {
		        l_field=atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
		        l_value=atoi( dstFieldString.c_str() );
		        if( l_field < l_value ) 
		        {
		            return false;
		        }
		    } 
		    else if(m_conditionExp.compare("<=")==0) 
		    {
                l_field=atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
		        l_value=atoi( dstFieldString.c_str() );
		        if( l_field > l_value ) 
		        {
		            return false;
		        }
		    } 
		    else 
		    {
		        return false;
		    }

        }                              
        else                                               // 字段值是"ALL"
        {                                          
            if(m_conditionExp.compare("!=")==0)                     // 如果字段没有值(!=ALL),即不为空
            {
                if(tmpFieldInfo->m_FieldDesValue.size()>0)  // 有值，不满足条件
                    return false;                          
            } 
            else if(m_conditionExp.compare("=")==0)                 // 如果字段有值(=ALL),即为空
            {
                if(tmpFieldInfo->m_FieldDesValue.size()<=0) // 没有值，不满足条件
                    return false;                          
            } 
            else 
            {
                return false;
            }
        }
    }
    //zaiww added 20060908 将"手机号,开始日期,开始时间,通话时长"输出到与原始文件同名的文件中
    //rule_16;ALL;DURATION>=45;//gboss//dev//billing//src//pp//pptest//zaiww_test//
	string MSISDN = "msisdn";
	string st_date = "START_DATE";
	string st_time = "START_TIME";
	string du      = "DURATION";
	
	//拼出输出字符串
	inputStr.clear();
	tmpFieldInfo=getFieldMap(theFieldInfoMaps,MSISDN);
	inputStr =  tmpFieldInfo->m_FieldDesValue;
	
	tmpFieldInfo=getFieldMap(theFieldInfoMaps,st_date);
	inputStr +=  "," + tmpFieldInfo->m_FieldDesValue;
	
	tmpFieldInfo=getFieldMap(theFieldInfoMaps,st_time);
	inputStr +=  "," + tmpFieldInfo->m_FieldDesValue;
	
	tmpFieldInfo=getFieldMap(theFieldInfoMaps,du);
	inputStr +=  "," + tmpFieldInfo->m_FieldDesValue;
	
	inputStr +=  "\n";
	
	//正在处理的文件
	m_thisFileName = m_PREFIX + m_PpLog->getSrcFileName();
	
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
            theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"ConvertRule_16:open output file("+m_thisTmpFileName+")  error!");
	        return false;		
	    }

	}
	//输出字符串
	if ( fputs(inputStr.c_str(),m_fDes) == EOF ) 
	{
	    theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"ConvertRule_16:write output file("+m_thisTmpFileName+")  error!");
	    return false;	
	}

	m_lastFileName = m_thisFileName;
	return true;
}

