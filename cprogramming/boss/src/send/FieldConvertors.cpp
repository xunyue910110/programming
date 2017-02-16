// ##########################################
// Source file : FieldConvertors.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020331
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertors.h"

//##ModelId=3B62167703A1
FieldConvertors::FieldConvertors()
{
}

//##ModelId=3B62167800BD
FieldConvertors::~FieldConvertors()
{
	for(int i=0;i<m_ConvertRules.size();i++) {
		delete m_ConvertRules[i];
	}
	m_ConvertRules.clear();
}

//##ModelId=3B5799C20283
bool FieldConvertors::initialize(const vector<string> &argname)
{
	string ruleText,ruleName;
	FieldConvertRule *theConvertRule;

	for(int i=0;i<argname.size();i++)
	{
		ruleText=argname[i];
		#if _DEBUG_
		  cout<<"CURRENT RULE!!:"<<ruleText<<endl;
		#endif
		ruleName=getRuleName(ruleText);
		if ( ruleName.compare("rule_1")==0 ) 	
		{
			theConvertRule = new FieldConvertRule_1;
		} 
		else if (ruleName.compare("rule_2")==0) 
		{
			theConvertRule = new FieldConvertRule_2;
		} 
		else if (ruleName.compare("rule_3")==0) 
		{
			theConvertRule = new FieldConvertRule_3;
		} 
		else if (ruleName.compare("rule_4")==0) 
		{
			theConvertRule = new FieldConvertRule_4;
		} 
		else if (ruleName.compare("rule_5")==0) 
		{
			theConvertRule = new FieldConvertRule_5;
		} 
		else if (ruleName.compare("rule_6")==0) 
		{
			theConvertRule = new FieldConvertRule_6;
		} 
		else if (ruleName.compare("rule_7")==0) 
		{
			theConvertRule = new FieldConvertRule_7;
		}
		else if (ruleName.compare("rule_8")==0) 
		{
			theConvertRule = new FieldConvertRule_8;
		} 
		else if (ruleName.compare("rule_9")==0) 
		{
			theConvertRule = new FieldConvertRule_9;
		}
		else if (ruleName.compare("rule_10")==0) 
		{
			theConvertRule = new FieldConvertRule_10;
		}
		else if (ruleName.compare("rule_11")==0) 
		{
			theConvertRule = new FieldConvertRule_11;
		}
		else if (ruleName.compare("rule_13")==0) 
		{
			theConvertRule = new FieldConvertRule_13;
		}
		else if (ruleName.compare("rule_14")==0)
		{
			theConvertRule = new FieldConvertRule_14;
		}
		else if (ruleName.compare("rule_15")==0) 
		{
			theConvertRule = new FieldConvertRule_15;
		}
		else if (ruleName.compare("rule_16")==0) 
		{
			theConvertRule = new FieldConvertRule_16;
		}
		else if (ruleName.compare("rule_17")==0) 
		{
			theConvertRule = new FieldConvertRule_17;
		}		
		else if (ruleName.compare("rule_30")==0) 
		{
			theConvertRule = new FieldConvertRule_30;
		}				
		else 
		{
			continue;
		}

		if (!theConvertRule->initialize(ruleText))
			return false;

		m_ConvertRules.push_back(theConvertRule);
	}

	return true;
}

//##ModelId=3BAAE9EC00A1
bool FieldConvertors::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int len;

    len=m_ConvertRules.size();
	for(int i=0;i<len;i++) 
	{
		m_ConvertRules[i]->convertField(theFieldInfoMaps);
	}

	return true;
}

//##ModelId=3BAED5CB0291
string FieldConvertors::getRuleName(string &theRuleText)
{
	char cTmp[1000];
	string theRuleName;

    // example: rule_1;ALL;MSRN_FLAG;         05=91,06=A1,OTHER=81

    strcpy(cTmp,theRuleText.c_str());
    theRuleName=string( strtok(cTmp,";") );      //得到逗号前的串

	return theRuleName;
}


