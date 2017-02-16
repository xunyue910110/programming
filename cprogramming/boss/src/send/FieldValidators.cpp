// ##########################################
// Source file : FieldValidators.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020419
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidators.h"

//##ModelId=3B621699009D
FieldValidators::FieldValidators()
{
   m_cdrFlag = CdrFlag::getInstance();
}

//##ModelId=3B62169901AB
FieldValidators::~FieldValidators()
{
  	for(int i=0;i<m_ValidRules.size();i++) 
  	{
		delete m_ValidRules[i];
	}
	m_ValidRules.clear();
}

//##ModelId=3BAAAA660101
bool FieldValidators::initialize(const vector<string> &argname)
{
    char *tok;
    char tmp[100];
	FieldValidateRule *theValidRule;
	string ruleText,ruleName,validType;

	for(int i=0;i<argname.size();i++)
	{
		ruleText=argname[i];
		ruleName=getRuleName(ruleText);
    	
    	strcpy(tmp,ruleText.c_str());
    	tok=strtok(tmp,";");
    	tok=strtok(NULL,";");
    	if(tok==NULL) 
    	{
    	   return false;
    	}
    	
    	validType = tok;
    	
    	if( validType.compare(m_ValidType) != 0 ) 
    	{
    	   continue;
    	}
    	
    	//根据配置文件中的配置，创建有效性检验规则
		if ( ruleName=="rule_1" ) 	
		{
			theValidRule = new FieldValidateRule_1;
		} 
		else if (ruleName=="rule_2") 	
		{
			theValidRule = new FieldValidateRule_2;
		} 
		else if (ruleName=="rule_3") 
		{
			theValidRule = new FieldValidateRule_3;
		}
		else if (ruleName=="rule_4") 
		{
			theValidRule = new FieldValidateRule_4;
		} 
		else if (ruleName=="rule_5") 
		{
			theValidRule = new FieldValidateRule_5;
		}
		else if (ruleName=="rule_7") 
		{
			theValidRule = new FieldValidateRule_7;
		}
		else if (ruleName=="rule_8") 
		{
			theValidRule = new FieldValidateRule_8;
		}
		else if (ruleName=="rule_9") 
		{
			theValidRule = new FieldValidateRule_9;
		}
		else 
		{
			continue;
		}
    	
		//有效性检验规则初始化
		if( !theValidRule->initialize(ruleText) ) 
		{
		    theErrorMessages->insert(PP_ERROR_INITIALIZE,"valid rule initialize("+ruleText+").");
		}
		m_ValidRules.push_back(theValidRule);
	}

	return true;
}

//##ModelId=3BAAAA7601C2
//根据规则对字段进行校验
bool FieldValidators::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int i,errorCode,len;

    errorCode=0;
    len=m_ValidRules.size();

	for(i=0;i<len;i++) 
	{
	    errorCode=m_ValidRules[i]->validField(theFieldInfoMaps);
	    if(errorCode<0) 
	    {
	       theErrorMessages->insert(PP_ERROR_PARAMETER,"valid rule parameter error!");
	       return false;
	    }

	    if( (errorCode>0) ) 
	    {
	       m_cdrFlag->m_isError    = true;
           m_cdrFlag->m_errorCode  = errorCode;
        }

        if(errorCode==PP_ERROR_FILE_NAME)         // 文件关键字错误
        {   
           return false;
        }
	}
	
	if(errorCode!=0)
	    return false;
	else
    	return true;
}

//##ModelId=3BCC0183008C
//返回配置文件中“=”的值
string FieldValidators::getRuleName(string &theRuleText)
{
	char cTmp[100];
	string theRuleName;

    // example: rule_1;fields;11;ALL;DURATION,value,0~10800

    strcpy(cTmp,theRuleText.c_str());
    theRuleName=string( strtok(cTmp,";") );      //得到逗号前的串

    return theRuleName;

}

//##ModelId=3BCCEE2602B1
void FieldValidators::setValidType(string left)
{
    m_ValidType = left;
}


