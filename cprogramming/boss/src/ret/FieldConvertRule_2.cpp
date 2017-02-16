// ##########################################
// Source file : FieldConvertRule_2.cpp
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

#include "FieldConvertRule_2.h"

//##ModelId=3BAAEABE023D
bool FieldConvertRule_2::initialize(string ruleText)
{
	char cTmp[50];
	vector<string> tmpStrs,tmpStrs1;
    //rule_2;08;MSRN_FLAG=91;DURATION=1,service_type=000,service_code=22
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_2',get "08"
    
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
	// 得到规则2中的 具体操作部分:DURATION=1,service_type=000,service_code=22
	splitStrByToken(tmpStrs[3],",",tmpStrs1);
	for(int i=0;i<tmpStrs1.size();i++)
    {
        if(!getField(tmpStrs1[i])) 
        {
            return false;
        }
    }
	return true;
}

bool FieldConvertRule_2::getField(const string value)
{
    char *tok;
    char tmp[200];

    strcpy(tmp,value.c_str());

    if( strstr(tmp,"+=")!=NULL )            // 添加(+=)
    {        
        tok=strtok(tmp,"+=");
        m_dstExps.push_back( string("+=") );
        m_dstFieldNames.push_back( string(tok) );

        tok=strtok(NULL,"+=");
        m_dstFieldValues.push_back( string(tok) );
    } 
    else if( strstr(tmp,"=+")!=NULL )            // 按字段的数值加(=+)
    {        
        tok=strtok(tmp,"=+");
        m_dstExps.push_back( string("=+") );
        m_dstFieldNames.push_back( string(tok) );

        tok=strtok(NULL,"=+");
        m_dstFieldValues.push_back( string(tok) );
    } 
    else if( strstr(tmp,"-=")!=NULL )        // 删除(-=)
    {  
        tok=strtok(tmp,"-=");
        m_dstExps.push_back( string("-=") );
        m_dstFieldNames.push_back( string(tok) );

        tok=strtok(NULL,"-=");
        m_dstFieldValues.push_back( string(tok) );
    } 
    else if( strstr(tmp,"=")!=NULL )          // 赋值(=)
    {   
        tok=strtok(tmp,"=");
        m_dstExps.push_back("=");
        m_dstFieldNames.push_back(tok);

        tok=strtok(NULL,"=");
        if(strncmp(tok,"EMPTY",5)==0) 
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

//##ModelId=3BAAEAFF02AF
bool FieldConvertRule_2::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,len1,len2;
	int l_field,l_value;
	char tmp[100],tmp1[100];
	static string dstFieldString,dstFieldName;
	FieldInfo *pFieldInfo;
	FieldInfo *tmpFieldInfo;
	static string callType("CALL_TYPE"),all("ALL");
	static string add1("+="),add2("-="),add3("="),add4("=+");
	
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

    len=m_dstFieldNames.size();
    for(int i=0;i<len;i++)
    {
        dstFieldName="";
        
        //若赋值表达式的右键是已定义的字段名,则取该字段对应的值付给左键字段
        if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_dstFieldValues[i]))!=0 ) 
        {
        	dstFieldString    = tmpFieldInfo->m_FieldDesValue;
		    dstFieldName = m_dstFieldValues[i];
		} 
		else 
		{
		    dstFieldString    = m_dstFieldValues[i];
		}

        // 若赋值表达式的左键不是已定义的字段名,则创建该字段
        if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_dstFieldNames[i]))==0 ) 
        {
            pFieldInfo = new FieldInfo(0);
//            pFieldInfo->m_IsFormat 	    = true;
			pFieldInfo->m_FieldName_ 	= m_dstFieldNames[i];
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dstFieldNames[i].c_str(),pFieldInfo) );

			tmpFieldInfo = getFieldMap(theFieldInfoMaps,m_dstFieldNames[i]);
		}

		if(m_dstExps[i].compare(add1)==0)                     // +=
		{                   
		    tmpFieldInfo->m_FieldDesValue  = dstFieldString + tmpFieldInfo->m_FieldDesValue;
		}
		//按字段的数值加 =+ by chenm 2002-12-25 17:47
		else if(m_dstExps[i].compare(add4)==0)                     
		{                   
		    int iSum;
		    char cSumTmp[20];
		    iSum = atoi( dstFieldString.c_str() )+ atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
		    sprintf(cSumTmp,"%d",iSum);
		    tmpFieldInfo->m_FieldDesValue  = cSumTmp;
		} 
		else if(m_dstExps[i].compare(add2)==0)                // -=
		{            
		    len2=tmpFieldInfo->m_FieldDesValue.size();
		    if(dstFieldString.compare("NATIONAL_86")==0) 
		    {
		        //by chenm 2002-12-9 9:23,对于msrn的去86处理
		        if( len2<12 && ((tmpFieldInfo->m_FieldName_).compare("msrn")!=0) ) 
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
		        if(tmpFieldInfo->m_FieldDesValue.compare(0,len1,dstFieldString)==0) 
		        {
		            // add by chenm 2003-12-1 对方号码以0086开头的处理
			        // 1、后续号码以'0'开始，直接去掉0086,2、后续号码为'1'开始且非'10'时直接去掉0086，否则保持不变；
					// 3、后续号码为'2'～'9' 保持不变
					if( ( (tmpFieldInfo->m_FieldName_).compare(0,11,"other_party")==0
						  || (tmpFieldInfo->m_FieldName_).compare(0,11,"OTHER_PARTY")==0 ) 
						&& (dstFieldString.compare("0086")==0) )
					{
						if( len2 >= 6)
						{
							if( tmpFieldInfo->m_FieldDesValue.compare(4,1,"0") == 0)
							{
								strcpy(tmp,tmpFieldInfo->m_FieldDesValue.c_str());
					            memcpy(tmp1,tmp+len1,len2-len1);
					            tmp1[len2-len1]=0;
			
					            tmpFieldInfo->m_FieldDesValue = tmp1;
					     	}	
							else if( tmpFieldInfo->m_FieldDesValue.compare(4,1,"1")== 0 )
							{
								if( tmpFieldInfo->m_FieldDesValue.compare(4,2,"10")!=0 )
	    	    	  		    {
	    	    	  				strcpy(tmp,tmpFieldInfo->m_FieldDesValue.c_str());
					            	memcpy(tmp1,tmp+len1,len2-len1);
					            	tmp1[len2-len1]=0;
			
					            	tmpFieldInfo->m_FieldDesValue = tmp1;		
	    	    	  			}	
							} 
						}
					}
					// over 2003-12-1 
					else
					{
						strcpy(tmp,tmpFieldInfo->m_FieldDesValue.c_str());
			            if(len2-len1<=0) 
			            {
		                    continue;
		                }
			            memcpy(tmp1,tmp+len1,len2-len1);
			            tmp1[len2-len1]=0;
	
			            tmpFieldInfo->m_FieldDesValue = tmp1;
			    	}
		        }
		    }

        } 
        else if(m_dstExps[i].compare(add3)==0)                // =
        {           
            tmpFieldInfo->m_FieldDesValue   = dstFieldString;
            // 如果被赋值的字段有错误，则修改m_cdrFlag->m_fieldName为该字段名称。
            if(m_CdrFlag->m_fieldName.compare(dstFieldName)==0) 
            {
                 m_CdrFlag->m_fieldName=tmpFieldInfo->m_FieldName_;
            }
        }

        if(m_dstFieldNames[i].compare("DURATION")==0)            // add by zhangy in 2002.5.16
        {  
            editTime( atoi( tmpFieldInfo->m_FieldDesValue.c_str() ),theFieldInfoMaps );
        }
    }

	return true;
}
