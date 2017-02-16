// ##########################################
// Source file : AsnRawCdrProcessor.cpp
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

#include "AsnRawCdrProcessor.h"
#include "CompositeFieldExtractRule.h"
#include "AsnFieldExtractRule.h"
#include "AsnFormat.h"

//##ModelId=4851D535002E
int AsnRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue, int length )
{
    int len, subLength = 0;
    FieldExtractRule *theField = 0;
    string theCdrSign;
    FieldInfo *theFieldInfo;
    vector <FieldExtractRule *> :: iterator iter;

    m_SrcCdrValue = aCdrValue;
    iter = (m_pCompositeField->m_theChildField_).begin();
    if (((*iter)->getFieldName()).compare(MODULE) != 0)
    {
        m_pCompositeField->extract(aCdrValue, length, *m_theRecordFormat);
        return 0;
    }

    vector <FieldExtractRule *> &rd = m_pCompositeField->m_theChildField_;
    len = rd.size();
    for ( int i = 0; i < len; i++ )
    {
        if ((rd[i]->getFieldName()).compare(MODULE) != 0)
        {
            break;
        }
        theCdrSign = getCdrSign(aCdrValue, length, rd[i], *m_theRecordFormat);
        theField = getNextCompositeField(theCdrSign);

        if (theField != 0)
        {
            if(((CompositeFieldExtractRule *)theField)->m_theChildField_[0]->getFieldName().compare(MODULE) == 0)
            {
                int iOffSet = 0;
                m_theRecordFormat->getCdrSize(theCdrSign.c_str(), (const char *)aCdrValue, iOffSet);
                theCdrSign = getCdrSign(aCdrValue + iOffSet, length - iOffSet, NULL, *m_theRecordFormat);
                vector <FieldExtractRule *>&rd = ((CompositeFieldExtractRule *)theField)->m_theChildField_;
                len = rd.size();
                theField = 0;
	            for ( int i = 0; i < len; i++ )
	            {
		            if ( (rd[i]->getSectionName()).compare(theCdrSign) == 0 )
		            {
			            theField = rd[i];
			            break;
		            }
	            }
                if( theField == 0)
                {
                    continue;
                }
			    
			    m_pFieldModule_Name->setDesValue(theCdrSign);

                m_theRecordFormat->reset(aCdrValue, length, 0, 1);
			    theField->extract(aCdrValue + iOffSet, length - iOffSet, *m_theRecordFormat);
            }
            else
            {
			    m_pFieldModule_Name->setDesValue(theCdrSign);

                m_theRecordFormat->reset(aCdrValue, length, 0, 1);
                theField->extract(aCdrValue, length, *m_theRecordFormat);
            }
        }
    }
    return 0;
}

//##ModelId=4851D5780167
bool AsnRawCdrProcessor::initialize(vector<string> aParameterList)
{
    m_theRecordFormat = new AsnFormat;
    m_theRecordFormat->initialize();

    return RawCdrProcessor::initialize(aParameterList);
}

//##ModelId=4851D59601C5
string AsnRawCdrProcessor::getCdrSign(const unsigned char *srcCdrValue, int CdrLength, FieldExtractRule *aField, RecordFormat &theRecordFormat)
{
    string theSign;
    char signTmp1[10];
    unsigned char signTmp[10];
    int valueLength, valueSize, tagSize;

    if(theRecordFormat.getFormatType() != 1)
    {
        return theSign;
    }

    AsnFormat* cp = (AsnFormat*)(&theRecordFormat);
    tagSize = cp->getAsnTag((unsigned char *)srcCdrValue, signTmp);
    for(int i = 0; i < tagSize; i++)
    {
        sprintf(signTmp1, "%02X", signTmp[i]);
        theSign += signTmp1;
    }
    return theSign;
}

