// ##########################################
// Source file : FieldConvertRule_5.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_5.h"
#include "FieldInfos.h"
#include "RecordFormat.h"

//##ModelId=4856172803D8
//RULE_5:FIELD=ALL;ntemp=substr(other_party,0,3),calledf=substr(CALLED_NUMBER,0,4)
bool FieldConvertRule_5::initialize(string ruleText)
{
    int len,ndx;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2;
	
	m_srcField.clear();
	m_desField.clear();
	m_substart.clear();
	m_subLength.clear();

	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	tmpStrs[0] = tmpStrs[0].substr(7);
	
	if (tmpStrs.size() == 2)
	{
	    if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    {
	        m_callType = "ALL";
	        m_conditionFieldName = "ALL";
	    }
	    else if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	        {
	            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	            m_callType = tmpStrs1[1];
	            m_conditionFieldName = "ALL";
	        }
	    else
	        {
	            //得到条件中的运算符
	            strcpy(cTmp,tmpStrs[0].c_str());
	            if(strstr(cTmp,"!=")!=NULL) 
                {
                    m_conditionExp = "!=";
                } 
                else if(strstr(cTmp,">=")!=NULL) 
                {
                    m_conditionExp = ">=";
                }
                else if(strstr(cTmp,"<=") != NULL)
                {
                    m_conditionExp = "<=";
                } 
                else if(strstr(cTmp,"=") != NULL)
                {
                    m_conditionExp = "=";
                }
                else 
                {
                    return false;
                }
                
                // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
                tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
                if (tmpStrs1.size()!=2)
                {
        	        return false;	
                }
                else
                {
                    m_conditionFieldName = tmpStrs1[0];
        	        m_conditionFieldValue = tmpStrs1[1];
        	        m_callType = "ALL";
                }	
            }    
	    
	    // 提取ntemp=substr(other_party,0,3),calledf=substr(CALLED_NUMBER,0,4)
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],"),",tmpStrs1);
	    for (int i=0; i<tmpStrs1.size(); i++)
	    {
            len = tmpStrs1[i].size();
	        if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
	        {
		        return false;
		    }
            m_desField.push_back( lrTrim(tmpStrs1[i].substr(0,ndx)) );
	        tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	        

            // 得到了substr(calling_bak,1,calling_size)
            len = tmpStrs1[i].size();
	        if ( ( ndx = tmpStrs1[i].find("(") ) < 0 )
	        {
		        return false;
	        }
	        // 得到了括号中的内容: calling_bak,1,calling_size
	        if ( ( len = tmpStrs1[i].find(")") ) < 0 )
	        {
		        len= tmpStrs1[i].size();
	        }

	        tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );

	        tmpStrs2.clear();StringUtil::split(tmpStrs1[i],",",tmpStrs2,true);
	
	        if( tmpStrs2.size() == 3 )
	        {
	            m_srcField.push_back(tmpStrs2[0]);
		        m_substart.push_back(tmpStrs2[1]);
		        m_subLength.push_back(tmpStrs2[2]); //如果m_subLength为关键字"CHARACTER",则长度为从0开始到第一个字符。		
	        }
	        else
	        {
		        return false;	
	        }
	
       }
       this->getFieldObj();
       return true;
	}
	else if (tmpStrs.size() == 3)
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
    
	            //得到条件中的运算符
	            if(strstr(cTmp,"!=")!=NULL) 
                {
                    m_conditionExp = "!=";
                } 
                else if(strstr(cTmp,"=")!=NULL) 
                {
                    m_conditionExp = "=";
                }
                else if(strstr(cTmp,">=")!=NULL)
                {
                    m_conditionExp = ">=";
                } 
                else if(strstr(cTmp,"<=")!=NULL)
                {
                    m_conditionExp = "<=";
                }
                else 
                {
                    return false;
                }
            
                // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
                tmpStrs1.clear();StringUtil::split(tmpStrs[1],m_conditionExp.c_str(),tmpStrs1);
                if (tmpStrs1.size()!=2)
                {
        	        return false;	
                }
                else
                {
                    m_conditionFieldName = tmpStrs1[0];
        	        m_conditionFieldValue = tmpStrs1[1];
                }
         
            
            // 提取ntemp=substr(other_party,0,3),calledf=substr(CALLED_NUMBER,0,4)
	        tmpStrs1.clear();StringUtil::split(tmpStrs[2],"),",tmpStrs1);
	        for (int i=0; i<tmpStrs1.size(); i++)
	        {
                len = tmpStrs1[i].size();
	            if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
	            {
		            return false;
		        }
                if (i==0)
                {
	                m_desField.push_back(lrTrim( tmpStrs1[i].substr(0,ndx)));
	                tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	            }
	            else
	            {
	                m_desField.push_back( lrTrim( tmpStrs1[i].substr(1,ndx-1)));
	                tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	            }

                // 得到了substr(calling_bak,1,calling_size)
                len = tmpStrs1[i].size();
	            if ( ( ndx = tmpStrs1[i].find("(") ) < 0 )
	            {
		            return false;
	            }
	            // 得到了括号中的内容: calling_bak,1,calling_size
	            tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	         
	            tmpStrs2.clear();StringUtil::split(tmpStrs1[i],",",tmpStrs2);
	
	            if( tmpStrs2.size() == 3 )
	            {
	                m_srcField.push_back(tmpStrs2[0]);
		            m_substart.push_back(tmpStrs2[1]);
		            m_subLength.push_back(tmpStrs2[2]); //如果m_subLength为关键字"CHARACTER",则长度为从0开始到第一个字符。		
	            }
	            else
	            {
		            return false;	
	            }
               }
               this->getFieldObj();
                return true;
            
	}
	else
	{
	    return false;
	}
}
	 
	 
//##ModelId=4856172F0290
bool FieldConvertRule_5::convertField()
{
    int i,j,k,len,substart;
    int l_field,l_value;
    char tmp[100],tmp1[100];

	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

    for (int i=0; i<m_srcField.size(); i++)
    {
        if( m_pSrcFields[i]->m_iDesValueLen == 0 )
        {
	        return false;
	    }

        if( m_iSubStartSeqs[i] != OUTOF_FIELD_RANGE ) 
        {
            substart = atoi( ((*m_pFieldInfos)[m_iSubStartSeqs[i]])->m_FieldDesValue);
        } 
        else 
        {
            substart = atoi(m_substart[i].c_str());
        }

        strcpy(tmp,m_pSrcFields[i]->m_FieldDesValue);
        // add by zhangy in 2002.5.25 start -----------------------------
        // 例：号码为：0252299394A323 求得的len为10。
        if(m_subLength[i].compare("CHARACTER")==0 ) 
        {
            len=strlen(tmp);
            for(j=0;j<len;j++)
            {
                if( tmp[j]>0x39 || tmp[j]<0x30 ) 
                {
                    break;
                }
            }
            if( j > substart )
            {
                len = j - substart;
            }
            else
            {
                len = j;
            }
            if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER)
                 &&(m_srcField[i]==m_CdrFlag->m_fieldName) )
            {
                m_CdrFlag->m_errorCode=0;
                m_CdrFlag->m_isError=false;
            }
            // add by zhangy in 2002.5.25 end -----------------------------
        }
        else if( m_subLength[i].compare("CHAR2ZERO")==0)
        {
    	    // add by chenm 2004-4-12 针对中兴综合接入网的话单,把非法字符转换成0
    	    len=strlen(tmp);
            for(k=0;k<len;k++)
            {
                if( tmp[k]== 'A' ) 
                {
                    tmp[k]= '0';
                }
            }
            if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER) 
                &&(m_srcField[i]==m_CdrFlag->m_fieldName) )
            {
                m_CdrFlag->m_errorCode=0;
                m_CdrFlag->m_isError=false;
            }	
        } 
        // add by lix 20090909 针对彩铃名字中含有逗号，把逗号改空格
        else if(m_subLength[i].compare("DOT2ZERO")==0)
        {
        	len=strlen(tmp);
        	 for(k=0;k<len;k++)
        	 {
        	 	    if(tmp[k]==0x2C)
        	 	    {
        	 	    	tmp[k]=' ';
        	 	    }	
        	 }  
        	 for(k=0;k<len-1;k++)
        	 {
        	 	    if((tmp[k]==0xA3)&&(tmp[k+1]==0xAC))
        	 	    {
        	 	    			tmp[k]=' ';
        	 	    			tmp[k+1]=' ';
        	 	    }
        	 }
        	if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER)
                 &&(m_srcField[i]==m_CdrFlag->m_fieldName) )
            {
                m_CdrFlag->m_errorCode=0;
                m_CdrFlag->m_isError=false;
            }
        }
        else if( m_iSubLengthSeqs[i]==OUTOF_FIELD_RANGE ) 
        {
            len = atoi(m_subLength[i].c_str());
        } 
        else 
        {
            len = atoi( ((*m_pFieldInfos)[m_iSubStartSeqs[i]])->m_FieldDesValue);
        }
	
	    // 如果超长
	    if( strlen(tmp)<(substart+len))
	    {
		    len = strlen(tmp) - substart;	
	    }
	    if( len<0 )
	    {
	        return false;
	    }
        memcpy(tmp1,tmp+substart,len);
        tmp1[len]=0;
        
        m_pDesFields[i]->setDesValue(tmp1);

    }
    return true;
    
}

void FieldConvertRule_5::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    
    int i;
    m_pSrcFields.clear();
    for(i=0;i<m_srcField.size();++i)
    {
    	m_pSrcFields.push_back(m_pFieldInfos->getFieldInfoByName(m_srcField[i].c_str()));
    }

	m_pDesFields.clear();
    for(i=0;i<m_desField.size();++i)
    {
    	m_pDesFields.push_back(m_pFieldInfos->getFieldInfoByName(m_desField[i].c_str()));
    }	

	FieldConvertRule::getFieldSequnceByName(m_substart,m_iSubStartSeqs);
	FieldConvertRule::getFieldSequnceByName(m_subLength,m_iSubLengthSeqs);
    
    
	return FieldConvertRule::getFieldObj();		
}
