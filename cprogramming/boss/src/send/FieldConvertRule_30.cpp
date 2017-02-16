// ##########################################
// Source file : FieldConvertRule_30.cpp
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Xulr
// E_mail      : Xulr@lianchuang.com
// Create      : 20100401
// Update      : 20100401
// Copyright(C): 2010 by Xulr, Linkage.
// ##########################################

#include "FieldConvertRule_30.h"

//##ModelId=3C7F4D3A0076
bool FieldConvertRule_30::initialize(string ruleText)
{
	int len,ndx,argsize,consize,i;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2;
	
	// rule_30;01;ALL;a=CONV2(b,16,10),a=LPAD(b,10,0),a=RPAD(b,10,' '),a=RPAD(b,10,\32)
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
		  m_callType = tmpStrs[1]; //跳过 'rule_9',get "01"
		
		  if(strcmp(tmpStrs[2].c_str(),"ALL")==0) 
      {
      	m_conditionFieldName = "ALL";
      } 
		  //tmpStrs[0] = tmpStrs[0].substr(8);
	    //if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    //{
	    //    m_callType = "ALL";
	    //    m_conditionFieldName = "ALL";
	    //}
	    //else if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    //{
	    //    splitStrByToken(tmpStrs[0],"=",tmpStrs1);
	    //    m_callType = tmpStrs1[1];
	    //    m_conditionFieldName = "ALL";
	    //}
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
            else if(strstr(cTmp,"<=")!= NULL)
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
            splitStrByToken(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
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
			// 提取 a=CONV2(b,16,10,1),a=PAD(b,10,0,1),a=PAD(b,10,' ',0),a=PAD(b,10,\32,1)
			string stmp,sfunc;
			int ipos;
			
			splitStrByToken(tmpStrs[3],")",tmpStrs1);
			consize = tmpStrs1.size();
			ipos = 0;
			
			for(i=0;i<consize;i++)
			{
				//分析a=CONV2(b,16,10
				if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
					return false;
				
				//取a
				stmp = tmpStrs1[i].substr(0,ndx);
				if(stmp[0]==',')
				{
					stmp.erase(0,1);//删除','
				}
				m_desfield.push_back(stmp);
				//取CONV2
				ipos = ndx + 1;
				if ( ( ndx = tmpStrs1[i].find("(") ) < 0 )
					return false;
				sfunc = tmpStrs1[i].substr(ipos,ndx - ipos);
				m_func.push_back(sfunc);
				//取参数b,16,10
				stmp = tmpStrs1[i].substr(ndx+1,tmpStrs1[i].length()-ndx);
				
				splitStrByToken(stmp,",",tmpStrs2);
				if(tmpStrs2.size() < 3)
					return false;
					
				m_srcfield.push_back(tmpStrs2[0]);
				m_arg1.push_back(tmpStrs2[1]);
				stmp=tmpStrs2[2];
				if(sfunc.compare("PAD")==0)
				{
					stmp=tmpStrs2[2].substr(0,1);
					//特殊符号处理
					if(stmp[0]=='\'' || stmp[0]=='\"')
						stmp=tmpStrs2[2].substr(1,1);
					if(stmp[0]=='\\')
						stmp[0]=atoi(tmpStrs2[2].substr(1,tmpStrs2[2].length()-1).c_str());
					stmp[1]=0;
				}
				
				m_arg2.push_back(stmp);
				
				if(tmpStrs2.size() > 3)
					m_arg3.push_back(tmpStrs2[3]);
				else
					m_arg3.push_back("0");
			}
	
   return true;
}

//##ModelId=3C7F578300D6
bool FieldConvertRule_30::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int i,len,consize,substart;
    char tmp[100],tmp1[100];
    string srcString,desString;
    FieldInfo *tmpFieldInfo,*srcFieldInfo,*desFieldInfo;
    static string callType("CALL_TYPE"),all("ALL");

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
	if(m_conditionFieldName.compare(all)!=0)                           // not is "ALL"   
	{           
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		if(m_conditionFieldValue.compare(all)!=0) 
		{                                                 // 字段值不是"ALL"
		    if(m_conditionExp.compare("!=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)==0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare("=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) 
		        {
                    return false;
                }
		    } 
		    else 
		    {
		        return false;
		    }
        } 
        else 
        {                                                    // 字段值是"ALL"
            if(m_conditionExp.compare("!=")==0) 
            {                                                // 如果字段没有值(!=ALL),即不为空
                if(tmpFieldInfo->m_FieldDesValue.size()>0)
                    return false;                            // 有值，不满足条件
            } 
            else if(m_conditionExp.compare("=")==0) {                 // 如果字段有值(=ALL),即为空
                if(tmpFieldInfo->m_FieldDesValue.size()<=0)
                    return false;                            // 没有值，不满足条件
            } 
            else 
            {
                return false;
            }
        }
    }
	
	char chch;
	consize = m_srcfield.size();
	
	for(i=0;i<consize;i++)
	{
		desString = "";
		
		if( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_srcfield[i]))==0 ) 
	     	srcString = m_srcfield[i];
	   else
	   	srcString = srcFieldInfo->m_FieldDesValue;
   	
   	if(m_func[i].compare("CONV2")==0)
   	{
   		desString=Conv2(srcString.c_str(),atoi(m_arg1[i].c_str()),atoi(m_arg2[i].c_str()),atoi(m_arg3[i].c_str()));
   	}
   	else if(m_func[i].compare("PAD")==0)
   	{
   		chch = m_arg2[i].at(0);
   		desString=padchar(srcString,atoi(m_arg1[i].c_str()),chch,atoi(m_arg3[i].c_str()));
   	}  
	
		if( (desFieldInfo=getFieldMap(theFieldInfoMaps,m_desfield[i]))==0 ) 
	   {
		 	   desFieldInfo = new FieldInfo(0);
		   	desFieldInfo->m_FieldName_ 	= m_desfield[i];
		   	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_desfield[i].c_str(),desFieldInfo) );
	   }
	   
	   desFieldInfo->m_FieldDesValue=desString;
   }
   
   return true;
}
