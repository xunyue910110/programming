#include "FieldInfos.h"
#include "Pp_Exception.h"
#include "RepeatableFieldInfo.h"

FIELDINFOS_MAP FieldInfos::m_fieldInfosMap;

FieldInfos::FieldInfos()
{
	m_iFieldCount = 0;
	m_iRepeatableFieldCount = 0;
	m_fieldNameMap.clear();
}

FieldInfo* FieldInfos::getFieldInfoByName(const char *pFieldName)
{
	FIELD_NAME_MAP_ITR itr;
	int iSequnceNum;
	MyString strFieldName = pFieldName;
	
	itr = m_fieldNameMap.find(strFieldName); 
	if( itr!=m_fieldNameMap.end() )
	{
		iSequnceNum = itr->second;
	}
	else
	{
		iSequnceNum = m_iFieldCount;
		m_fieldNameMap.insert(FIELD_NAME_MAP::value_type(strFieldName,iSequnceNum));
		m_pFieldInfos[iSequnceNum] = new FieldInfo();	
		++m_iFieldCount;
	}
	
	return (FieldInfo *)m_pFieldInfos[iSequnceNum];
}

FieldInfo *FieldInfos::operator[](const int iSequnce)
{
	if( iSequnce < m_iFieldCount 
		&& iSequnce < DIVIDE)
	{
		return m_pFieldInfos[iSequnce];	
	}

	if( iSequnce >= DIVIDE 
		&& iSequnce < m_iRepeatableFieldCount+DIVIDE)
	{
		return m_pRepeatableFieldInfos[iSequnce-DIVIDE];	
	}
	
	throw Pp_Exception (__FILE__, __LINE__,"字段下标 %d 未定义!", iSequnce);		

}

bool FieldInfos::reset()
{
	for(int i=0;i<m_iFieldCount;++i)
	{
		m_pFieldInfos[i]->reset();
	}
	for(int i=0;i<m_iRepeatableFieldCount;++i)
	{
		m_pRepeatableFieldInfos[i]->reset();
	}

	return true;
}

FieldInfo * FieldInfos::getRepeatableFieldInfoByName(const char *pFieldName)
{
	FIELD_NAME_MAP_ITR itr;
	int iSequnceNum;
	MyString strFieldName = pFieldName;
	
	itr = m_fieldNameMap.find(strFieldName); 
	if( itr!=m_fieldNameMap.end() )
	{
		iSequnceNum = itr->second - DIVIDE;
	}
	else
	{
		iSequnceNum = m_iRepeatableFieldCount ;
		m_fieldNameMap.insert(FIELD_NAME_MAP::value_type(strFieldName,iSequnceNum+ DIVIDE));
		m_pRepeatableFieldInfos[iSequnceNum] = new RepeatableFieldInfo();	
		++m_iRepeatableFieldCount ;
	}
	
	return m_pRepeatableFieldInfos[iSequnceNum];	
}

// 如果已经定义的字段 则返回字段下标 否则返回-1
int FieldInfos::getSequnceByName(const char *pFieldName)
{
	FIELD_NAME_MAP_ITR itr;
	int iSequnceNum;
	MyString strFieldName = pFieldName;
	
	itr = m_fieldNameMap.find(strFieldName); 
	if( itr!=m_fieldNameMap.end() )
	{
		iSequnceNum = itr->second;
	}
	else
	{
		iSequnceNum = OUTOF_FIELD_RANGE;	
	}
	return iSequnceNum;	
}

int FieldInfos::getFieldNum()
{
	return 	m_iFieldCount;
}

int FieldInfos::getRepeatableFieldNum()
{
	return 	m_iRepeatableFieldCount;
}	

const char * FieldInfos::getFieldNameBySeq(const int iSequnce)
{
	FIELD_NAME_MAP_ITR itr;
	
	for(itr=m_fieldNameMap.begin();itr!=m_fieldNameMap.end();++itr)
	{
		if(itr->second == iSequnce)
		{
			return itr->first.c_str();	
		}	
	}
	return STR_OUTOF_FIELD_RANGE;
}

FieldInfos * FieldInfos::getInstance(const int iChannelNum)
{
	FIELDINFOS_MAP_ITR itr;

	itr = m_fieldInfosMap.find(iChannelNum);
	
	if(itr==m_fieldInfosMap.end())
	{
		FieldInfos *pFieldInfos=new FieldInfos();
		m_fieldInfosMap.insert(FIELDINFOS_MAP::value_type(iChannelNum,pFieldInfos));
		return pFieldInfos;
	}
	else
	{
		return itr->second;	
	}				
}
