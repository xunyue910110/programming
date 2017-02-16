// ##########################################
// Source file : FieldValidators.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidators.h"
#include "FieldInfo.h"
#include "FieldValidateRule.h"
#include "FieldValidateRule_1.h"
#include "FieldValidateRule_2.h"
#include "FieldValidateRule_3.h"
#include "FieldValidateRule_4.h"
#include "FieldValidateRule_5.h"
#include "FieldValidateRule_7.h"
#include "FieldValidateRule_8.h"
#include "FieldValidateRule_9.h"
#include "FieldValidateRule_10.h"
//#include "FieldValidateRule_11.h"

//##ModelId=4850D44801A5
FieldValidators::FieldValidators()
{
    m_cdrFlag = CdrFlag::getInstance();
}

//##ModelId=4850D44B00FA
FieldValidators::~FieldValidators()
{
    for(int i=0;i<m_ValidRules.size();i++) 
  	{
		delete m_ValidRules[i];
	}
	m_ValidRules.clear();
}

//##ModelId=4850D45802BF
bool FieldValidators::initialize(const vector<string> &argname,const int iSubChannelNum)
{
    string ruleName;
    string ruleText;
    FieldValidateRule *theValidRule;
    bool flag = true;
    string tmpCondition,tmpRuleExp,tmpCondition1;
    int size, b=0;
    int num1,g,t;
    string tmp1;
    string argCondition,tmpStr;
    vector<string> argValue;
    int ndx1, ndx2;
    string validType;

     
    
    for(int i=0; i <argname.size(); i++)
    {
        ruleText = argname[i];
        
        if (ruleText.compare(0,2,"IF") == 0)
        {
            argCondition="";
            if (!flag)
            {
                argCondition.clear();
                argValue.clear();
                return false;
            }
            flag = false;
            tmpCondition = ruleText.substr(3,ruleText.size()-4); //(VALID_TYPE=system)AND(FIELD=FILE_NO)
            size = tmpCondition.size();
            int x = tmpCondition.find("AND");
            if(x > 0)
            {

                for (num1 = tmpCondition.find("AND"); num1 > 0; )
                {


                        if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+3] != '('))
                        {
                            tmpCondition1 = tmpCondition.substr(num1+3);
                            num1 = tmpCondition1.find("AND");
                            continue;
                        }

                    tmp1 = tmpCondition.substr(0,num1);        //得到"AND"前的内容(VALID_TYPE=system)
                    tmpCondition = tmpCondition.substr(num1+3,tmpCondition.size()-num1-3); //得到"AND"后的内容(FIELD=FILE_NO)
                            
                    g = tmp1.find_last_of("(");
                    tmp1 = tmp1.substr(g+1,tmp1.size()-g-2);
                            
                    argCondition =argCondition + tmp1+";";
                            
                    num1 = tmpCondition.find("AND");
                }
                g = tmpCondition.find_first_of("(");
                t = tmpCondition.find_first_of(")");
                        
                tmp1 = tmpCondition.substr(g+1,t-g-1);
                argCondition = argCondition + tmp1 + ";";
            }
            else
            {
                argCondition = tmpCondition+";";
            }    
        }
        else if(ruleText.compare(0,4,"THEN") == 0)
        {
            continue;
        }
        else if(ruleText.compare(0,4,"RULE") == 0)
        {   
            int x;       
            x = ruleText.find(":");
            tmpStr = ruleText.substr(0,x+1)+argCondition+ruleText.substr(x+1,ruleText.size()-x-1);
       
            argValue.push_back(tmpStr);
 
        }
        else if(ruleText.compare(0,6,"ELSEIF") == 0)
        {
            tmpCondition = ruleText.substr(7,ruleText.size()-8);
    
            argCondition = "";
            
            int y = tmpCondition.find("AND");
            
            if(y > 0)
            {
                
                for (num1 = tmpCondition.find("AND"); num1 > 0; )
                {

                        if((tmpCondition[num1-1] != ')') || (tmpCondition[num1+3] != '('))
                        {
                            tmpCondition1 = tmpCondition.substr(num1+3);
                            num1 = tmpCondition1.find("AND");
                            continue;
                        }

                    tmp1 = tmpCondition.substr(0,num1);        //得到"AND"前的内容
                    tmpCondition = tmpCondition.substr(num1+3,tmpCondition.size()-num1-3); //得到"AND"后的内容
                            
                    g = tmp1.find_last_of("(");
                    tmp1 = tmp1.substr(g+1,tmp1.size()-g-2);
                            
                    argCondition =argCondition + tmp1+";";
                            
                    num1 = tmpCondition.find("AND");
                }
                g = tmpCondition.find_first_of("(");
                t = tmpCondition.find_first_of(")");
                        
                tmp1 = tmpCondition.substr(g+1,t-g-1);
                argCondition = argCondition + tmp1 + ";";
            }
            else
            {
                argCondition = tmpCondition+";";
            }    
        }
        else if(ruleText.compare(0,5,"ENDIF") == 0)
        {
            argCondition.clear();
            flag = true;
        }
        else
        {
            continue;  
        }
        
    }
    
    for(int i = 0; i < argValue.size(); i++)
    {
        tmpRuleExp = argValue[i];
        ruleName = getRuleName(argValue[i]);
        
        if ( (ndx1 = tmpRuleExp.find_first_of("=")) < 0 )
        {
            return false;
        }
        if ( (ndx2 = tmpRuleExp.find_first_of(";")) < 0 )
        {
            return false;
        }
        validType = tmpRuleExp.substr(ndx1+1,ndx2-ndx1-1);
    	
    	if( validType.compare(m_ValidType) != 0 ) 
    	{
    	   continue;
    	}

#ifdef DEBUG
       cout << "ruleText is:" << tmpRuleExp << endl;
#endif
        if (ruleName=="RULE_1")
        {
            theValidRule = new FieldValidateRule_1(iSubChannelNum);
        }
        if (ruleName=="RULE_2")
        {
            theValidRule = new FieldValidateRule_2(iSubChannelNum);
        }
        if (ruleName=="RULE_3")
        {
            theValidRule = new FieldValidateRule_3(iSubChannelNum);
        }
       if (ruleName=="RULE_4")
       {
           theValidRule = new FieldValidateRule_4(iSubChannelNum);
       }
       if (ruleName=="RULE_5")
       {
           theValidRule = new FieldValidateRule_5(iSubChannelNum);
       }
       if (ruleName=="RULE_7")
       {
           theValidRule = new FieldValidateRule_7(iSubChannelNum);
       }
       if (ruleName=="RULE_8")
       {
           theValidRule = new FieldValidateRule_8(iSubChannelNum);
       }
       if (ruleName=="RULE_9")
       {
           theValidRule = new FieldValidateRule_9(iSubChannelNum);
       }
       if (ruleName=="RULE_10")
       {
           theValidRule = new FieldValidateRule_10(iSubChannelNum);
       }
       //if (ruleName=="RULE_11")
       //{
       //    theValidRule = new FieldValidateRule_11(iSubChannelNum);
       //}
        
        else
        {
            continue;
        }

        if (!theValidRule->initialize(tmpRuleExp))
        {
            theErrorMessages->insert(PP_ERROR_INITIALIZE,"valid rule initialize("+tmpRuleExp+").");
        }

        m_ValidRules.push_back(theValidRule);
    }

    return true;
}

//##ModelId=4850D46D00DA
bool FieldValidators::validField()
{
    int i,errorCode,len;

    errorCode=0;
    len=m_ValidRules.size();

	for(i=0;i<len;i++) 
	{
	    errorCode=m_ValidRules[i]->validField();
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
	{
	    return false;
	}
	else
	{
        return true;
    }
}

//##ModelId=4850D48400CB
void FieldValidators::setValidType(string left)
{
     m_ValidType = left;
}

//##ModelId=4850D49B02EE
string FieldValidators::getRuleName(string &theRuleText)
{
	char cTmp[100];
	string theRuleName;

    // example: rule_1;fields;11;ALL;DURATION,value,0~10800

    strcpy(cTmp,theRuleText.c_str());
    theRuleName=string( strtok(cTmp,":") );      //得到冒号前的串

    return theRuleName;
}

