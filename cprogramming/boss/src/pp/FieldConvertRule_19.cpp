// ##########################################
// Source file : FieldConvertRule_19.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080924
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_19.h"
#include "PreProcApplication.h"
#include "FieldInfos.h"


//##ModelId=485647D400DA
bool FieldConvertRule_19::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
    char tmp[100],tmp1[100];
    int num;

    m_vectFieldMaps.clear();
    // rule_10:CALL_TYPE=03;FIELD=ALL;FORW              # 通用呼转话单的拆分
    StringUtil::split(ruleText,";",tmpStrs);

    if( tmpStrs.size() < 2)
    {
        // 小于2个分号的,不符合规则的书写格式
        return false;
    }

	// 提取规则中的信息
	if(tmpStrs.size() == 2) //RULE_19:FORW
	{
	    tmpStrs[0] = tmpStrs[0].substr(8);
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
      
            // splitStrByToken(tmpStrs[0],m_conditionExp[0].c_str(),tmpStrs1);
            tmpStrs1.clear();
            StringUtil::split(tmpStrs[0],m_conditionExp[0].c_str(),tmpStrs1);

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
   
        tmpStrs1.clear();
        StringUtil::split(tmpStrs[1],",",tmpStrs1);
        m_mscType=tmpStrs[1];
	      return true;
    }
    else if(tmpStrs.size() > 2)
    {
         tmpStrs[0] = tmpStrs[0].substr(8);
	    
	       if(tmpStrs[0].compare(0,9,"CALL_TYPE")==0)
         {
            tmpStrs1.clear();
            StringUtil::split(tmpStrs[0],"=",tmpStrs1);

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
             //   splitStrByToken(tmpStrs[i],m_conditionExp[num++].c_str(),tmpStrs1);
             tmpStrs1.clear();
             StringUtil::split(tmpStrs[i],m_conditionExp[num++].c_str(),tmpStrs1);
        
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
     
          tmpStrs1.clear();
          StringUtil::split(tmpStrs[tmpStrs.size()-1],",",tmpStrs1);
   
	        return true;
     }
     //this->getFieldObj();
     return false;
}

////##ModelId=485647DB009C
bool FieldConvertRule_19::convertField()
{
    FieldInfo *aFieldInfo=0;
    static string all("ALL");
    static string callType("CALL_TYPE"),t1;
    //if( (aFieldInfo=getFieldMap(theFieldInfoMaps, callType))==0 ) 
  	if((aFieldInfo=m_pFieldInfos->getFieldInfoByName(CALL_TYPE))==0)
	  {
	    return 0;
	  }
	
	  // 条件1：通话类型
	  if(m_callType.compare(all) != 0)
    {
    	 t1=aFieldInfo->m_FieldDesValue;
       if(t1.compare(m_callType)!=0)
       {
           return 0;
       }
    }
    
    if ( m_mscType.compare("FORW") == 0 )
    { 
        splitforw();
        return 0;
    }
    return 0;
}

//##ModelId=4856481D0138
bool FieldConvertRule_19::splitforw()
{
    static string callType("CALL_TYPE");
    static string otherParty("other_party");
    static string thirdParty("third_party");
    string party_tmp,t1,third_partty_tmp,call_type_tmp;
   

    FieldInfo *m_callType=0, *m_otherParty=0, *m_thirdParty=0;

    //要交换的字段
    //if( (m_callType=getFieldMap(tmpVectFieldMap, callType)) == 0 )
    if((m_callType=m_pFieldInfos->getFieldInfoByName(callType.c_str()))== 0)
    {
        return false;
    }

    //if( (m_otherParty=getFieldMap(tmpVectFieldMap, otherParty)) == 0 )
    if((m_otherParty=m_pFieldInfos->getFieldInfoByName(otherParty.c_str()))== 0)
    {
        return false;
    }

    //if( (m_thirdParty=getFieldMap(tmpVectFieldMap, thirdParty)) == 0 )
    if((m_thirdParty=m_pFieldInfos->getFieldInfoByName(thirdParty.c_str()))== 0)
    {
        return false;
    }

    call_type_tmp = m_callType->m_FieldDesValue;
    string li_call_type = "20";
    m_callType->setDesValue(li_call_type.c_str());
    party_tmp = m_otherParty->m_FieldDesValue;
    third_partty_tmp = m_thirdParty->m_FieldDesValue;
    
    m_otherParty->setDesValue(third_partty_tmp.c_str());
    m_thirdParty->setDesValue(party_tmp.c_str());
       
    m_PpLog->WriteCdrtofile();
    m_PpLog->addMtcSum();
    m_PpLog->addTotalCdrs();
    m_callType->setDesValue(call_type_tmp.c_str());
    m_otherParty->setDesValue(party_tmp.c_str());
    m_thirdParty->setDesValue(third_partty_tmp.c_str());

    return true;
}


FieldConvertRule_19::~FieldConvertRule_19()
{
}
