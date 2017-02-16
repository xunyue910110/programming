// ##########################################
// Source file : FixedRawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.18
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20020108
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################


#include "FixedRawCdrProcessor.h"
#include "FixedFieldExtractRule.h"

//##ModelId=3B60212E0324
int FixedRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue,int length,
						multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,len1;
	FieldExtractRule *theField=0;
	static string theCdrSign;
	FieldInfo *pFieldInfo;
	char tmp[10],tmp1[3],theSign[10];
	vector <FieldExtractRule *> :: iterator iter;

	iter=(m_theCompositeField.m_theChildField_).begin();
	if ( ((*iter)->getFieldName()).compare("Module")!=0 ) 
	{
	    m_theCompositeField.extract(aCdrValue,length,theFieldInfoMaps,*m_theRecordFormat);

		return 0;
	}

    vector <FieldExtractRule *> &rd=m_theCompositeField.m_theChildField_;
    len=rd.size();
    len=1;

	for ( int i=0;i<len; i++ )
	{
		if ( (rd[i]->getFieldName()).compare("Module")!=0  ) 
		{
			break;
		}

        // 取得cdr标志，如 "01"
        FixedFieldExtractRule *cp = (FixedFieldExtractRule*)rd[i];
        len1           = cp->m_BitLength_/8;
        if(len1<=0) 
        {
            return -1;  
        }
        memcpy(tmp,(char *)(aCdrValue) + cp->m_StartByte_,len1);
        theSign[0]=0;

        for(int j=0;j<len1;j++)
        {
            sprintf(tmp1,"%02X",tmp[j]);
            strcat(theSign,tmp1);
        }
        theSign[len1*2]=0;
        theCdrSign=theSign;
        // 取得要处理的字段对象(即给定的话单类型字段对象)
        theField=getNextCompositeField(theCdrSign);
		if (theField!=0) 
		{
			pFieldInfo = new FieldInfo(0);
			pFieldInfo->m_FieldName_		= string("Module_name");
			pFieldInfo->m_FieldLength		= len1;
			pFieldInfo->m_FieldDesValue	    = theCdrSign;
//			pFieldInfo->m_IsFormat			= true;
			//strcpy((char *)(pFieldInfo->m_FieldSrcValue),tmp);
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );
			theField->extract(aCdrValue,length,theFieldInfoMaps,*m_theRecordFormat);
		}
	}

	return 0;
}

//##ModelId=3B66B7490053
int FixedRawCdrProcessor::getCdrSign()
{
	// 当不同的通话类型话单的字段格式定义不同时，本方法用于选取其中的一种格式
//	cout<<"get fixed cdr sign . "<<endl;
	return 0;
}

//##ModelId=3B96EBE4023F
bool FixedRawCdrProcessor::initialize( vector<string> aParameterList)
{
	return RawCdrProcessor::initialize(aParameterList);
}


