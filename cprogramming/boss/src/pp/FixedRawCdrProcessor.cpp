// ##########################################
// Source file : FixedRawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "FixedRawCdrProcessor.h"
#include "FixedFieldExtractRule.h"
#include "FieldInfo.h"
#include "CompositeFieldExtractRule.h"
//##ModelId=4851D6570203
int FixedRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue, int length)
{
    int len,len1;
    FieldExtractRule *theField = 0;
    static string theCdrSign;
    FieldInfo *theFieldInfo;
    char tmp[10], tmp1[3], theSign[10];
    vector <FieldExtractRule *> :: iterator iter;
    

    iter=(m_pCompositeField->m_theChildField_).begin();

    if (((*iter)->getFieldName()).compare(MODULE) != 0)
    {
        m_pCompositeField->extract(aCdrValue, length, *m_theRecordFormat);
        return 0;
    }

    vector <FieldExtractRule *> &rd = m_pCompositeField->m_theChildField_;
    len = rd.size();
    len = 1;

    for (int i = 0; i < len; i++)
    {
        if ((rd[i]->getFieldName()).compare(MODULE) != 0)
        {
            break;
        }

        // 取得cdr标志,如 "01"
        FixedFieldExtractRule *cp = (FixedFieldExtractRule*)rd[i];
        len1 = cp->m_BitLength_ / 8;
        if(len1 <= 0)
        {
            return -1;
        }
        memcpy(tmp, (char *)(aCdrValue) + cp->m_StartByte_, len1);
        theSign[0] = 0;

        for(int j = 0; j < len1; j++)
        {
            sprintf(tmp1, "%02X", tmp[j]);
            strcat(theSign, tmp1);
        }
        theSign[len1 * 2] = 0;
        theCdrSign = theSign;

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

//##ModelId=4851D6B700CB
bool FixedRawCdrProcessor::initialize(vector<string> aParameterList)
{
    return RawCdrProcessor::initialize(aParameterList);
}

//##ModelId=4851D6D5035B
int FixedRawCdrProcessor::getCdrSign()
{
    return 0;
}

