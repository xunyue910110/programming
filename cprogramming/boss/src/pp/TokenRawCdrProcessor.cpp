// ##########################################
// Source file : TokenRawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "TokenRawCdrProcessor.h"
#include "FieldInfo.h"
#include "CompositeFieldExtractRule.h"

//##ModelId=4851D7D702AF
int TokenRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue, int length)
{
    int len,len1;
    FieldExtractRule *theField = 0;
    string theCdrSign;
    unsigned char theValue[1024];
    char valueTmp[1024];
    FieldInfo *theFieldInfo;
    vector <FieldExtractRule *> :: iterator iter;
    iter=(m_pCompositeField->m_theChildField_).begin();
    if (((*iter)->getFieldName()).compare(MODULE) != 0)
    {
        if(!m_pCompositeField->extract(aCdrValue, length, *m_theRecordFormat))
        {
    	    return -1;
    	}
    	else
    	{
            return 0;
    	}
    }
    vector <FieldExtractRule *> &rd = m_pCompositeField->m_theChildField_;
    len = rd.size();

    for (int i = 0; i < len; i++)
    {
        if ((rd[i]->getFieldName()).compare(MODULE) != 0)
        {
            break;
        }

        // 取得cdr标志,如 "01"
        TokenFieldExtractRule *cp = (TokenFieldExtractRule*)rd[i];
        len1 = cp->getValue(aCdrValue, theValue, length);
        sprintf(valueTmp, "%s", theValue);
        theCdrSign = valueTmp;

        // 取得要处理的字段对象(即给定的话单类型字段对象)
        theField = getNextCompositeField(theCdrSign);
        if (theField != 0)
        {
			m_pFieldModule_Name->setDesValue(theCdrSign);

            theField->extract(aCdrValue, length, *m_theRecordFormat);
        }
    }
    return 0;
}

//##ModelId=4851D8180261
bool TokenRawCdrProcessor::initialize(vector<string> aParameterList)
{
    return RawCdrProcessor::initialize(aParameterList);
}

//##ModelId=4851D831006D
int TokenRawCdrProcessor::getCdrSign()
{
    return 0;
}

