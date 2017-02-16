// ##########################################
// Source file : SasnRawCdrProcessor.cpp
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

#include "SasnRawCdrProcessor.h"
#include "CompositeFieldExtractRule.h"

//##ModelId=3B60213F0198
int SasnRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue, int length)
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
			m_pFieldModule_Name->setDesValue(theCdrSign);

            m_theRecordFormat->reset(aCdrValue, length, 0, 1);
            theField->extract(aCdrValue, length, *m_theRecordFormat);
        }
    }
    return 0;
}

//##ModelId=3B66B75301F2
string SasnRawCdrProcessor::getCdrSign(const unsigned char *srcCdrValue	, int CdrLength, FieldExtractRule *aField, RecordFormat &theRecordFormat	)
{
	string theSign;
	char signTmp1[10];
	unsigned char signTmp[10];
	int valueLength, valueSize, tagSize;

 	if(theRecordFormat.getFormatType() != 1) 
 	{
 		return theSign;
 	}

    SasnFormat* cp  = (SasnFormat*)(&theRecordFormat);
	tagSize = cp->getAsnTag((unsigned char *)srcCdrValue, signTmp);
	for(int i = 0; i < tagSize; i++) 
	{
		sprintf(signTmp1, "%02X", signTmp[i]);
		theSign += signTmp1;
	}
	return theSign;
}

//##ModelId=3B96EBE302A1
bool SasnRawCdrProcessor::initialize( vector<string> aParameterList)
{
	m_theRecordFormat = new SasnFormat;
	m_theRecordFormat->initialize();

	return RawCdrProcessor::initialize(aParameterList);
}

