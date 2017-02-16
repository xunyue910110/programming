// ##########################################
// Source file : FieldConvertRule_12.cpp
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

#include "FieldConvertRule_12.h"
#include "FieldInfos.h"

//##ModelId=485629B101A5
//用于实现浮点型字段值之间的+,-,*,/等操作
bool FieldConvertRule_12::initialize(string ruleText)
{
    char cTmp[50];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2,tmpStrs3;

	// rule_9:FIELD=ALL;cfee=cfee1+cfee2
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(8);
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
	        // 得到条件中的运算符
            strcpy(cTmp,tmpStrs[0].c_str());
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
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
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
	    //拆分 cfee=cfee1+cfee2,num=num1+num2;  
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    for (int i=0; i<tmpStrs1.size(); i++)
	    {
	        tmpStrs2.clear();StringUtil::split(tmpStrs1[i],"=",tmpStrs2);
	    
		    if( tmpStrs2.size() != 2 )
		    {
			    return false;	
		    }
		    else
		    {
			    m_desFieldName.push_back(tmpStrs2[0]);           // get "cfee"
			 
			    // 得到赋值语句中的运算符
        	    strcpy(cTmp, tmpStrs2[1].c_str());
	            if(strstr(cTmp,"+")!=NULL) 
	            {
	                m_desOperat.push_back("+");
	            } 
	            else if(strstr(cTmp,"-")!=NULL) 
	            {
                    m_desOperat.push_back("-");
	            } 
	            else if(strstr(cTmp,"*")!=NULL) 
	            {
                    m_desOperat.push_back("*");
	            } 
	            else if(strstr(cTmp,"/")!=NULL) 
	            {
                    m_desOperat.push_back("/");
	            } 
	            else 
	            {
	                return false;
	            }
	            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
	            tmpStrs3.clear();StringUtil::split(tmpStrs2[1],m_desOperat[i],tmpStrs3);
	            if(tmpStrs3.size()!=2)
	            {
	        	    return false;	
	            }
	            else
	            {
	                m_operatLeftFieldName.push_back(tmpStrs3[0]);    // get "cfee1"
	        	    m_operatRightFieldName.push_back(tmpStrs3[1]);   // get "cfee2"
	            }		        	
		    }
		}
		this->getFieldObj();
		return true;
    }
    else if (tmpStrs.size() == 3)
    {
        tmpStrs[0] = tmpStrs[0].substr(8);
        
        if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
            m_callType = tmpStrs1[1];
        }
        else
        {
            return false;	 
        }
      
        
           	// 得到条件中的运算符
            strcpy(cTmp,tmpStrs[1].c_str());
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
            tmpStrs1.clear();StringUtil::split(tmpStrs[1],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
           else
            {
                m_conditionFieldName = tmpStrs1[0];
        	    m_conditionFieldValue = tmpStrs1[1];
            }	 
        
        //拆分 cfee=cfee1+cfee2  
        
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	    for (int i=0; i<tmpStrs1.size(); i++)
	    {
	        tmpStrs2.clear();StringUtil::split(tmpStrs1[i],"=",tmpStrs2);
	        if( tmpStrs2.size() != 2 )
		    {
			    return false;	
		    }
		    else
		    {
			    m_desFieldName.push_back(tmpStrs2[0]);     // get "cfee"
        
			    // 得到赋值语句中的运算符
        	    strcpy(cTmp,tmpStrs2[1].c_str());
	            if(strstr(cTmp,"+")!=NULL) 
	            {
	                m_desOperat.push_back("+");
	            } 
	            else if(strstr(cTmp,"-")!=NULL) 
	            {
	               m_desOperat.push_back("-");
	            } 
	            else if(strstr(cTmp,"*")!=NULL) 
	            {
	                m_desOperat.push_back("*");
	            } 
	            else if(strstr(cTmp,"/")!=NULL) 
	            {
	                m_desOperat.push_back("/");
	            } 
	            else 
	            {
	                return false;
	            }
	            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
	            tmpStrs3.clear();StringUtil::split(tmpStrs2[1],m_desOperat[i],tmpStrs3);
	            if(tmpStrs3.size()!=2)
	            {
	        	    return false;	
	            }
	            else
	            {
	                m_operatLeftFieldName.push_back(tmpStrs3[0]);   // get "cfee1"
	        	    m_operatRightFieldName.push_back(tmpStrs3[1]);  // get "cfee2"
	            }		        	
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

//##ModelId=485629BB007D
bool FieldConvertRule_12::convertField()
{
    float src1, src2,des;
    char tmp[100];
    
	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}
    
	// 如果找不到目的字段,则创建出来
	for (int i=0; i<m_desOperat.size(); i++)
	{
        if( m_iSrcLeftSeqs[i] == OUTOF_FIELD_RANGE ) 
        {
            src1    =   atof( m_operatLeftFieldName[i].c_str() );     
        } 
        else 
        {
            src1    =   atof(  ( (*m_pFieldInfos)[m_iSrcLeftSeqs[i]] )->m_FieldDesValue );   
        }
    
	    if( m_iSrcRightSeqs[i] == OUTOF_FIELD_RANGE ) 
	    {
            src2    =   atof( m_operatRightFieldName[i].c_str() );     
        } 
        else 
        {
            src2    =   atof( ( (*m_pFieldInfos)[m_iSrcRightSeqs[i]] )->m_FieldDesValue );   
        }   
    
        if(string(m_desOperat[i]).compare("+")==0) 
        {
            des=src1+src2;
        } 
        else if (string(m_desOperat[i]).compare("-")==0) 
        {
            des=src1-src2;
        } 
        else if (string(m_desOperat[i]).compare("*")==0) 
        {
            des=src1*src2;
        } 
        else if (string(m_desOperat[i]).compare("/")==0) 
        {  
            if(src2>0)
                des=src1/src2;
        } 
    
        sprintf(tmp,"%f",des);  
        m_pDesFields[i]->setDesValue(tmp);
    }    
   
    return true;
}

void FieldConvertRule_12::getFieldObj()
{
    int i;

    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  

	  FieldConvertRule::getFieldSequnceByName(m_operatLeftFieldName,m_iSrcLeftSeqs);
	  FieldConvertRule::getFieldSequnceByName(m_operatRightFieldName,m_iSrcRightSeqs);
    
	  m_pDesFields.clear();
    for(i=0;i<m_desFieldName.size();++i)
    {
    	m_pDesFields.push_back(m_pFieldInfos->getFieldInfoByName(m_desFieldName[i].c_str()));
    }	
	
	return FieldConvertRule::getFieldObj();		
}
