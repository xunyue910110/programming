// ##########################################
// Source file : FieldConvertRule_2.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_2.h"
#include "FieldInfos.h"


//##ModelId=4856155D02FD
bool FieldConvertRule_2::initialize(string ruleText)
{
    char tmp[50];
    int num;
	vector<string> tmpStrs,tmpStrs1;
    //rule_2:CALL_TYPE=08;MSRN_FLAG=91;DURATION=1,service_type=000,service_code=22
	StringUtil::split(ruleText,";",tmpStrs);
	
	
	m_conditionFieldName.clear();
	m_conditionFieldValue.clear();
	m_conditionExp.clear();
	m_dstFieldNames.clear();
	m_dstFieldValues.clear();
	m_dstExps.clear();
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	if(tmpStrs.size() == 2) //rule_2:ServiceType=8888;service_type=000,service_code=22
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	 
	    if(tmpStrs[0].compare(0,5,"FIELD")==0)
	    {    
	        m_conditionFieldName.push_back("ALL");
	        m_callType = "ALL";
	    }
	    else if(tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	        m_conditionFieldName.push_back("ALL");
	    }
	    else
	    {
	        strcpy(tmp,tmpStrs[0].c_str());
	        if(strstr(tmp,"!=")!=NULL) 
            {
                m_conditionExp.push_back("!=");
            } 
            else if(strstr(tmp,">=")!=NULL) 
            {
                m_conditionExp.push_back(">=");
            } 
            else if(strstr(tmp,"<=")!=NULL) 
            {
                m_conditionExp.push_back("<=");
            } 
            else if(strstr(tmp,"=")!=NULL) 
            {
                m_conditionExp.push_back("=");
            } 
            else 
            {
                return false;
            }
      
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp[0].c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
                return false;	
            }
            else
            {
                m_conditionFieldName.push_back(tmpStrs1[0]); 
                m_conditionFieldValue.push_back(tmpStrs1[1]);
                m_callType = "ALL";	
            }
        }
     
        tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    for(int i=0;i<tmpStrs1.size();i++)
        {
            if(!getField(tmpStrs1[i])) 
            {
                return false;
            }
        }
        this->getFieldObj();
	    return true;
    }
    else if(tmpStrs.size() > 2)
    {
         tmpStrs[0] = tmpStrs[0].substr(7);
	    
	     if(tmpStrs[0].compare(0,9,"CALL_TYPE")==0)
         {
             tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
             m_callType = tmpStrs1[1];
             num = 0;
         }
         else
         {
             m_callType = "ALL";
       
         strcpy(tmp,tmpStrs[0].c_str());
         if(strstr(tmp,"!=")!=NULL) 
         {
             m_conditionExp.push_back("!=");
         } 
         else if(strstr(tmp,">=")!=NULL) 
         {
             m_conditionExp.push_back(">=");
         } 
         else if(strstr(tmp,"<=")!=NULL) 
         {
             m_conditionExp.push_back("<=");
         } 
         else if(strstr(tmp,"=")!=NULL) 
         {
             m_conditionExp.push_back("=");
         } 
         else 
         {
             return false;
         }
         tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp[0].c_str(),tmpStrs1);
         if(tmpStrs1.size()!=2)
         {
             return false;	
         }
         else
         {
             m_conditionFieldName.push_back(tmpStrs1[0]);
             m_conditionFieldValue.push_back(tmpStrs1[1]);
         }
         num = 1;
        }
         for (int i=1; i<tmpStrs.size()-1; i++)
         {
             memset(tmp,0,strlen(tmp));
	         strcpy(tmp,tmpStrs[i].c_str());
        
             if(strstr(tmp,"!=")!=NULL) 
             {
                 m_conditionExp.push_back("!=");
             } 
             else if(strstr(tmp,">=")!=NULL) 
             {
                 m_conditionExp.push_back(">=");
             } 
             else if(strstr(tmp,"<=")!=NULL) 
             {
                 m_conditionExp.push_back("<=");
             } 
             else if(strstr(tmp,"=")!=NULL) 
             {
                 m_conditionExp.push_back("=");
             } 
             else 
             {
                 return false;
             }
             tmpStrs1.clear();StringUtil::split(tmpStrs[i],m_conditionExp[num++].c_str(),tmpStrs1);
             if(tmpStrs1.size()!=2)
             {
                 return false;	
             }
             else
             {
                 m_conditionFieldName.push_back(tmpStrs1[0]);
                 m_conditionFieldValue.push_back(tmpStrs1[1]);
             }
         }
     
         tmpStrs1.clear();StringUtil::split(tmpStrs[tmpStrs.size()-1],",",tmpStrs1);
    
	     for(int i=0;i<tmpStrs1.size();i++)
         {
             if(!getField(tmpStrs1[i])) 
             {
                return false;
             }
         }
         this->getFieldObj();
	     return true;
     }
     
     return false;
}



//##ModelId=48561568038A
bool FieldConvertRule_2::convertField()
{
    int len1,len2;
	int l_field,l_value;
	char tmp1[100];
	string dstFieldString;
	static string add1("+="),add2("-="),add3("="),add4("=+");
	
	if( this->condJudge() == false)
	{
		return false;	
	}
  
    for(int j=0;j<m_dstFieldNames.size();++j)
    {
        //若赋值表达式的右键是已定义的字段名,则取该字段对应的值付给左键字段
        if( m_iOperExpRightKeySeqs[j]==OUTOF_FIELD_RANGE ) 
        {
        	dstFieldString    = m_dstFieldValues[j];
		} 
		else 
		{
		    dstFieldString    = ( (*m_pFieldInfos)[m_iOperExpRightKeySeqs[j]] )->m_FieldDesValue==NULL?"":( (*m_pFieldInfos)[m_iOperExpRightKeySeqs[j]] )->m_FieldDesValue;
		}

		if(m_dstExps[j].compare(add1)==0)                     // +=
		{                   
		    m_pFieldTargets[j]->setDesValue( (dstFieldString + m_pFieldTargets[j]->m_FieldDesValue).c_str() );
		}
		//按字段的数值加 =+ by chenm 2002-12-25 17:47
		else if(m_dstExps[j].compare(add4)==0)                     
		{                   
		    int iSum;
		    char cSumTmp[20];
		    iSum = atoi( dstFieldString.c_str() )+ atoi( m_pFieldTargets[j]->m_FieldDesValue==NULL?"":m_pFieldTargets[j]->m_FieldDesValue );
		    sprintf(cSumTmp,"%d",iSum);
		    m_pFieldTargets[j]->setDesValue(cSumTmp);
		} 
		else if(m_dstExps[j].compare(add2)==0)                // -=
		{            
		    len2 = m_pFieldTargets[j]->m_iDesValueLen;
		    if(dstFieldString.compare("NATIONAL_86")==0) 
		    {
		        //by chenm 2002-12-9 9:23,对于msrn的去86处理
		        if( len2<12 && m_dstFieldNames[j].compare("msrn")!=0)  
		        {
		            continue;
		        } 
		        else 
		        {
		            dstFieldString="86";
		        }
		    } 
		    else if(dstFieldString.compare("NATIONAL_0086")==0) 
		    {
		        dstFieldString="0086";
		    }
		    len1=dstFieldString.size();

            if(len2>=len1) 
            {
		        if( strncmp(m_pFieldTargets[j]->m_FieldDesValue,dstFieldString.c_str(),len1)==0 ) 
		        {
		            // add by chenm 2003-12-1 对方号码以0086开头的处理
			        // 1、后续号码以'0'开始，直接去掉0086,2、后续号码为'1'开始且非'10'时直接去掉0086，否则保持不变；
					// 3、后续号码为'2'～'9' 保持不变
					if( ( m_dstFieldNames[j].compare(0,11,"other_party")==0
						  || m_dstFieldNames[j].compare(0,11,"OTHER_PARTY")==0 ) 
						&& (dstFieldString.compare("0086")==0) )
					{
						if( len2 >= 6)
						{
							if( strncmp(m_pFieldTargets[j]->m_FieldDesValue+4,"0",1) == 0)
							{
								memcpy(tmp1,m_pFieldTargets[j]->m_FieldDesValue+len1,len2-len1);
					            tmp1[len2-len1]=0;
			
					            m_pFieldTargets[j]->setDesValue(tmp1);
					     	}	
							else if( strncmp(m_pFieldTargets[j]->m_FieldDesValue+4,"1",1)== 0 )
							{
								if( strncmp(m_pFieldTargets[j]->m_FieldDesValue+4,"10",2)!=0 )
	    	    	  		    {
	    	    	  				memcpy(tmp1,m_pFieldTargets[j]->m_FieldDesValue+len1,len2-len1);
					            	tmp1[len2-len1]=0;
			
					            	m_pFieldTargets[j]->setDesValue(tmp1);		
	    	    	  			}	
							} 
						}
					}
					// over 2003-12-1 
					else
					{
						if(len2-len1<=0) 
			            {
		                    continue;
		                }
			            memcpy(tmp1,m_pFieldTargets[j]->m_FieldDesValue+len1,len2-len1);
			            tmp1[len2-len1]=0;
	
			            m_pFieldTargets[j]->setDesValue(tmp1);
			    	}
		        }
		    }
        } 
        else if(m_dstExps[j].compare(add3)==0)                // =
        {           
            m_pFieldTargets[j]->setDesValue(dstFieldString);
            // 如果被赋值的字段有错误，则修改m_cdrFlag->m_fieldName为该字段名称。
            if(m_CdrFlag->m_fieldName.compare(m_dstFieldValues[j])==0) 
            {
            	m_CdrFlag->m_fieldName=m_dstFieldNames[j];
            }
        }

        if(m_dstFieldNames[j].compare(DURATION)==0)            // add by zhangy in 2002.5.16
        {  
            editTime( atoi( m_pFieldTargets[j]->m_FieldDesValue ) );
        }
    }
    
    return true;
}


//##ModelId=48561581003E
bool FieldConvertRule_2::getField(const string &value)
{
    char *tok;
    char tmp[200];
    
    strcpy(tmp, value.c_str());
    
    if( strstr(tmp, "+=") != NULL )
    {
        tok=strtok(tmp,"+=");
        m_dstExps.push_back( string("+=") );
        m_dstFieldNames.push_back( string(tok) );
        
        tok=strtok(NULL,"+=");
        m_dstFieldValues.push_back( string(tok) );
    }
    else if( strstr(tmp, "=+") != NULL )
    {
        tok=strtok(tmp,"=+");
        m_dstExps.push_back( string("=+") );
        m_dstFieldNames.push_back( string(tok) );
        
        tok=strtok(NULL, "=+");
        m_dstFieldValues.push_back( string(tok) );
    }
    else if( strstr(tmp,"-=") != NULL )
    {
       tok=strtok(tmp,"-=");
       m_dstExps.push_back( string("-=") );
       m_dstFieldNames.push_back( string(tok) );
       
       tok=strtok(NULL,"-=");
       m_dstFieldValues.push_back( string(tok) ); 
    }
    else if( strstr(tmp,"=") != NULL )
    {
        tok=strtok(tmp,"=");
        m_dstExps.push_back("=");
        m_dstFieldNames.push_back(string(tok));
        
        tok=strtok(NULL,"=");
        if(strncmp(tok,"EMPTY",5) == 0)
        {
           tok[0]=0; 
        }
        m_dstFieldValues.push_back(tok);
    }
    else
    {
        return false;
    }
    
    return true;
}

void FieldConvertRule_2::getFieldObj()
{
    int i;
    
    m_pFieldConditions.clear();
    for(i=0;i<m_conditionFieldName.size();++i)
	{
    	m_pFieldConditions.push_back( m_pFieldInfos->getFieldInfoByName(m_conditionFieldName[i].c_str()) );
    }  

    m_pFieldTargets.clear();
    for(i=0;i<m_dstFieldNames.size();++i)
    {
    	m_pFieldTargets.push_back(m_pFieldInfos->getFieldInfoByName(m_dstFieldNames[i].c_str()));
    }

	FieldConvertRule::getFieldSequnceByName(m_conditionFieldValue,m_iCondExpRightKeySeqs);
	FieldConvertRule::getFieldSequnceByName(m_dstFieldValues,m_iOperExpRightKeySeqs);
    
	return FieldConvertRule::getFieldObj();		
}

bool FieldConvertRule_2::condJudge()
{
    string dstFieldString;
    int l_field,l_value;
    
	// 条件1：通话类型
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }

    // 条件2：其他字段值
    for (int i=0; i<m_conditionFieldName.size(); ++i)
    {
	    if(m_conditionFieldName[i].compare(CONDITION_ALL) != 0)                // not is "ALL" 
	    {            
            //若条件2的左键不是已定义的字段名,返回false
            if( m_pFieldConditions[i]->m_iDesValueLen==0 )
            {
		        return false;
		    }

	        //若条件2的右键是定义的字段名,则取该字段的值进行比较,否则直接用右键的值进行比较
	        if( m_iCondExpRightKeySeqs[i] == OUTOF_FIELD_RANGE )
	        {
	            dstFieldString = m_conditionFieldValue[i];
	        } 
	        else 
	        {
                dstFieldString = ( (*m_pFieldInfos)[m_iCondExpRightKeySeqs[i]] )->m_FieldDesValue;
            }

			//字段值是"TAG_ALL" added by zaiww 20090925
			//针对只出现tag码,且valueSize为0的情况判断,区别于("=ALL"表示该字段有值且valueSize不为0)
			if(m_conditionFieldValue[i].compare(TAG_ALL)==0)
			{
				//TAG_ALL关键字前必须配置等号(例：=TAG_ALL)
				if(m_conditionExp[i].compare("=")!=0)
				{
					theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "convert rule_2 error! ");
					return false;
				}
				if(m_pFieldConditions[i]->m_tagAll==false)
				{
					return false;
				}
			}
			else
		    {
		    	//  字段值不是"ALL"
		    	if(m_conditionFieldValue[i].compare(CONDITION_ALL)!=0)               
		    	{
		    	    if(m_conditionExp[i].compare("!=")==0) 
		    	    {
		    	        if( strcmp(m_pFieldConditions[i]->m_FieldDesValue,dstFieldString.c_str())==0 ) 
		    	        {
            	            return false;
            	        }
		    	    } 
		    	    else if(m_conditionExp[i].compare("=")==0) 
		    	    {
		    	        if( strcmp(m_pFieldConditions[i]->m_FieldDesValue,dstFieldString.c_str())!=0 ) 
		    	        {
            	            return false;
            	        }
		    	    } 
		    	    else if(m_conditionExp[i].compare(">=")==0) 
		    	    {
		    	        l_field=atoi( m_pFieldConditions[i]->m_FieldDesValue );
		    	        l_value=atoi( dstFieldString.c_str() );
		    	        if( l_field < l_value ) 
		    	        {
		    	            return false;
		    	        }
		    	    } 
		    	    else if(m_conditionExp[i].compare("<=")==0) 
		    	    {
            	        l_field=atoi( m_pFieldConditions[i]->m_FieldDesValue );
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
            	    if(m_conditionExp[i].compare("!=")==0)                     // 如果字段没有值(!=ALL),即为空
            	    {
            	        if( m_pFieldConditions[i]->m_iDesValueLen>0 )  // 有值，不满足条件
            	        {
            	            return false;
            	        }                          
            	    } 
            	    else if(m_conditionExp[i].compare("=")==0)                 // 如果字段有值(=ALL),即不为空
            	    {
            	        if( m_pFieldConditions[i]->m_iDesValueLen<=0 ) // 没有值，不满足条件
            	        {   
            	            return false;
            	        }                          
            	    } 
            	    else 
            	    {
            	        return false;
            	    }
            	}
            }
         } 
         else
         {
            break;
         }
    }
    return true;
}
