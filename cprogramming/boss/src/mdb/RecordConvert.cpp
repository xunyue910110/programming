#include "RecordConvert.h"


RecordConvert::RecordConvert(TableDesc* tableDesc)
{
	m_tableDesc = tableDesc;
	
	m_columnPos.clear();
	m_pkColumnNames.clear();
	MyString s;
	string tmpString;
	for (int i=0; i<(m_tableDesc->m_tableDef).m_columnNum; ++i)
	{
		strcpy(s.m_pStr,(m_tableDesc->m_tableDef).m_columnList[i].m_name);
		m_columnPos.insert(COLOMN_POS_POOL::value_type(s, i));
	}
	for (int i=0; i<(m_tableDesc->m_tableDef).m_keyClumnNum; ++i)
	{
		tmpString = (m_tableDesc->m_tableDef).m_keyColumnList[i];
		m_pkColumnNames.push_back(tmpString);
	}
	
	m_value = NULL;
}	


RecordConvert::~RecordConvert()
{
	m_tableDesc = NULL;
	m_columnPos.clear();
	m_pkColumnNames.clear();
	m_value = NULL;
}


void RecordConvert::setRecordValue(void* value)
{
	m_value = value;
}


void RecordConvert::getRecordValue(void* &value, int &len)
{
	value = m_value;
	len = m_tableDesc->m_pageInfo.m_slotSize;
}


void RecordConvert::getColumnsValue(const vector<string>& columnNames, void* value, int& len)
{
	len = 0;
	int pos = 0;
	MyString s;
	for (vector<string>::const_iterator iter=columnNames.begin(); iter!=columnNames.end(); ++iter)
	{
		strcpy(s.m_pStr,(*iter).c_str());
		COLOMN_POS_POOL_ITR iterm = m_columnPos.find(s);
		if (iterm != m_columnPos.end())
		{
			pos = iterm->second;
			memcpy((char*)value+len, (char*)m_value+(m_tableDesc->m_tableDef).m_columnList[pos].m_offSet, (m_tableDesc->m_tableDef).m_columnList[pos].m_len);
			len += (m_tableDesc->m_tableDef).m_columnList[pos].m_len;
		}
	}
}


void RecordConvert::getPkValue(void* value, int& len)
{
	getColumnsValue(m_pkColumnNames, value, len);
}


void RecordConvert::getPkColumnPos(const int& pkPos, int& colPos)
{
	colPos = -1;
	MyString s;
	
	strcpy(s.m_pStr,(m_tableDesc->m_tableDef).m_keyColumnList[pkPos]);
	COLOMN_POS_POOL_ITR iter=m_columnPos.find(s);
	
	if (iter != m_columnPos.end())
	{
		colPos = iter->second;
	}
}


RecordConvert::RecordConvert(const RecordConvert& recordConvert)
{
	m_tableDesc = recordConvert.m_tableDesc;
	m_columnPos = recordConvert.m_columnPos;
	for (int i=0; i<recordConvert.m_pkColumnNames.size(); i++)
	{
		m_pkColumnNames[i] = recordConvert.m_pkColumnNames[i];
	}
	m_value = recordConvert.m_value;
}


RecordConvert& RecordConvert::operator=(const RecordConvert& recordConvert)
{
	if (this != &recordConvert)
	{
		m_tableDesc = recordConvert.m_tableDesc;
		m_columnPos = recordConvert.m_columnPos;
		for (int i=0; i<recordConvert.m_pkColumnNames.size(); i++)
		{
			m_pkColumnNames[i] = recordConvert.m_pkColumnNames[i];
		}
		m_value = recordConvert.m_value;
	}
	return *this;	
}

