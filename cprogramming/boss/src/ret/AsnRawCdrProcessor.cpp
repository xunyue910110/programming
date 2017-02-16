// ##########################################
// Source file : AsnRawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.20
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20011130
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "AsnRawCdrProcessor.h"

//##ModelId=3B978CA501F4
AsnRawCdrProcessor::~AsnRawCdrProcessor()
{
}

//##ModelId=3B60213F0198
int AsnRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue,int length,
					  multimap<MyString,FieldInfo *> &m_theFieldInfoMaps)
{
	int len;
	FieldExtractRule *theField=0;
	string theCdrSign;
	FieldInfo *theFieldInfo;
	vector <FieldExtractRule *> :: iterator iter;

	m_SrcCdrValue = aCdrValue;
	iter=(m_theCompositeField.m_theChildField_).begin();
	if ( ((*iter)->getFieldName()).compare("Module")!=0 )
	{
		m_theCompositeField.extract(aCdrValue,length,m_theFieldInfoMaps,*m_theRecordFormat);

		return 0;
	}

    vector <FieldExtractRule *> &rd=m_theCompositeField.m_theChildField_;
    len=rd.size();
	for ( int i=0;i<len; i++ )
	{
		if ( (rd[i]->getFieldName()).compare("Module")!=0  )
		{
			break;
		}
		theCdrSign=getCdrSign(aCdrValue,length,rd[i],*m_theRecordFormat);
		theField=getNextCompositeField(theCdrSign);

		if (theField!=0)
		{
            // add by chenm for eri的复合话单 可能会有两个Module的话单情况2006-12-28
            if( ((CompositeFieldExtractRule *)theField)->m_theChildField_[0]->getFieldName().compare("Module")==0 )
            {
                int iOffSet = 0;
                m_theRecordFormat->getCdrSize(theCdrSign.c_str(),(const char *)aCdrValue,iOffSet);
                theCdrSign=getCdrSign(aCdrValue+iOffSet,length-iOffSet,NULL,*m_theRecordFormat);
                vector <FieldExtractRule *>&rd=((CompositeFieldExtractRule *)theField)->m_theChildField_;
                len=rd.size();
                theField = 0;
	            for ( int i=0;i<len; i++ )
	            {
		            if ( (rd[i]->getSectionName()).compare(theCdrSign)==0 )
		            {
			            theField = rd[i];
			            break;
		            }
	            }
                if( theField == 0)
                    continue;
			    theFieldInfo = new FieldInfo(2+1);
			    theFieldInfo->m_FieldName_		= string("Module_name");
			    theFieldInfo->m_FieldLength		= 2;
			    theFieldInfo->m_FieldDesValue	= theCdrSign;
//			    theFieldInfo->m_IsFormat			= true;
			    strcpy((char *)(theFieldInfo->m_FieldSrcValue),theCdrSign.c_str());
			    m_theFieldInfoMaps.insert(multimap<MyString,FieldInfo *>::value_type(theFieldInfo->m_FieldName_.c_str(),theFieldInfo));

			    m_theRecordFormat->reset(aCdrValue,length,0,1);
			    theField->extract(aCdrValue+iOffSet,length-iOffSet,m_theFieldInfoMaps,*m_theRecordFormat);
            }
            else
            { // over 2006-12-28
    			theFieldInfo = new FieldInfo(2+1);
    			theFieldInfo->m_FieldName_		= string("Module_name");
    			theFieldInfo->m_FieldLength		= 2;
    			theFieldInfo->m_FieldDesValue	    = theCdrSign;
//    			theFieldInfo->m_IsFormat			= true;
    			strcpy((char *)(theFieldInfo->m_FieldSrcValue),theCdrSign.c_str());
    			m_theFieldInfoMaps.insert(multimap<MyString,FieldInfo *>::value_type(theFieldInfo->m_FieldName_.c_str(),theFieldInfo));

    			m_theRecordFormat->reset(aCdrValue,length,0,1);
    			theField->extract(aCdrValue,length,m_theFieldInfoMaps,*m_theRecordFormat);
            }
		}
	}
	return 0;
}

//##ModelId=3B66B75301F2
string AsnRawCdrProcessor::getCdrSign(const unsigned char *srcCdrValue	,int CdrLength,
					   	  FieldExtractRule *aField, RecordFormat &theRecordFormat	)
{
	string theSign;
	char signTmp1[10];
	unsigned char signTmp[10];
	int valueLength,valueSize,tagSize;

 	if(theRecordFormat.getFormatType()!=1) 
 	{
 		return theSign;
 	}

    AsnFormat* cp  = (AsnFormat*) (&theRecordFormat);
	tagSize        = cp->getAsnTag((unsigned char *)srcCdrValue,signTmp);
	for(int i=0;i<tagSize;i++) 
	{
		sprintf(signTmp1,"%02X",signTmp[i]);
		theSign+=signTmp1;
	}

	return theSign;
}

//##ModelId=3B96EBE302A1
bool AsnRawCdrProcessor::initialize( vector<string> aParameterList)
{
	m_theRecordFormat=new AsnFormat;
	m_theRecordFormat->initialize();

	return RawCdrProcessor::initialize(aParameterList);
}

