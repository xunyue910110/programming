#include "DataBuffer.h"
#include <iostream>
using namespace std;


static Array * g_Array = NULL;
int sort_cmp( const void *a, const void *b)
{
	if (g_Array->getDataType() == dtString)
    {
      return (strcmp((char*)a,(char*)b));
    }
    else if (g_Array->getDataType() == dtInteger)
    {
        if ( *(int*)a < *(int*)b)
             return -1;
        else if ( *(int*)a == *(int*)b)
             return 0;
        else if ( *(int*)a > *(int*)b)
             return 1;
    }
	else if (g_Array->getDataType() == dtInt64)
	{
		if (*(Int64*)a < *(Int64*)b)
             return -1;
        else if (*(Int64*)a == *(Int64*)b)
             return 0;
        else if (*(Int64*)a > *(Int64*)b)
             return 1;
	}

    return 0;
}

Array::Array(const Array & array)
{
	m_Address = NULL;
   *this = array;
}

Array& Array::operator = (const Array& array)
{
	if (this == &array)
		return *this;

    destory();
    
    m_Size     = array.m_Size;
    m_UsedSize = array.m_UsedSize;
    m_Length   = array.m_Length;
	m_InternalLength = array.m_InternalLength;
    m_Type     = array.m_Type;
    
    allocate();
    
    memcpy(m_Address,array.m_Address,m_InternalLength * m_Size);  
    
    return *this;
}

void Array::destory()
{
    if (dtInteger == m_Type)
        deleteArray<int*>();
    else if (dtString == m_Type)
        deleteArray<char*>();
    else if (dtInt64 == m_Type)
        deleteArray<Int64*>();
    else if (dtDouble == m_Type)
        deleteArray<double*>();
}

void Array::allocate()
{
    if (dtInteger == m_Type)
    {
	    m_Address = newArray<int> (m_Size);
        m_Length = sizeof(int);
		m_InternalLength = m_Length;
    }
    else if (dtString == m_Type)
    {
		//确保'\0'结尾
		m_InternalLength = m_Length + 1;
		//内存字节对齐
		m_InternalLength += sizeof(void*) - m_InternalLength % sizeof(void*);
        m_Address = newArray<char>( m_InternalLength * m_Size);
    }
    else if (dtInt64 == m_Type)
    {
        m_Address = newArray<Int64>(m_Size);
        m_Length = sizeof(Int64);
		m_InternalLength = m_Length;
    }
    else if (dtDouble == m_Type)
    {
        m_Address = newArray<double>(m_Size);
        m_Length = sizeof(double);
		m_InternalLength = m_Length;
    }
	else
	{
		//确保'\0'结尾
		m_InternalLength = m_Length + 1; 
		//内存字节对齐
		m_InternalLength += sizeof(void*) - m_InternalLength % sizeof(void*);
		m_Address = newArray<char>( m_InternalLength * m_Size);
	}
    memset(m_Address,0x00,m_InternalLength * m_Size);
}

void Array::pushBack(int value)
{
	assert(m_Type != dtString);
	if (m_UsedSize >= m_Size)
	{
		if (dtInteger == m_Type)
			extend<int>(2);
		else if (dtDouble == m_Type)
			extend<double>(2);
		else if (dtInt64 == m_Type)
			extend<Int64>(2);
	}
	
	if (dtInteger == m_Type)
		*pointerCast<int*>(m_UsedSize)    = value;
	else if (dtDouble == m_Type)
		*pointerCast<double*>(m_UsedSize) = (double)value;
	else if (dtInt64 == m_Type)
		*pointerCast<Int64*>(m_UsedSize)  = (Int64)value;

    ++m_UsedSize;
}

void Array::setValue(int value,UInt32 index)
{
	assert(m_Type != dtString);
	while (index > m_UsedSize)
	{
		pushBack((int)-1);
	}

	if (index == m_UsedSize)
		pushBack(value);
	else
	{
		if (dtInteger == m_Type)
			*pointerCast<int*>(index)    = value;
		else if (dtDouble == m_Type)
			*pointerCast<double*>(index) = (double)value;
		else if (dtInt64 == m_Type)
			*pointerCast<Int64*>(index)  = (Int64)value;
	}
}


void Array::pushDouble(double value)
{
    assert(m_Type != dtString);
    if (m_UsedSize >= m_Size)
    {
	   if (dtDouble == m_Type)
		   extend<double>(2);
	   else if (dtInteger == m_Type)
		   extend<int>(2);
	   else if (dtInt64 == m_Type)
		   extend<Int64>(2);
    }    

	if (dtDouble == m_Type)
		*pointerCast<double*>(m_UsedSize) = value;
	else if (dtInteger == m_Type)
		*pointerCast<int*>(m_UsedSize)    = (int)value;
	else if (dtInt64 == m_Type)
		*pointerCast<Int64*>(m_UsedSize)  = (Int64)value;

    ++m_UsedSize;
}

void Array::setDouble(double value,UInt32 index)
{
    assert(m_Type != dtString);
	while (index > m_UsedSize)
	{
		pushDouble(0.00);
	}

	if (index == m_UsedSize)
		pushDouble(value);
	else
	{
		if (dtDouble == m_Type)
			*pointerCast<double*>(index) = value;
		else if (dtInteger == m_Type)
			*pointerCast<int*>(index)    = (int)value;
		else if (dtInt64 == m_Type)
			*pointerCast<Int64*>(index)  = (Int64)value;
	}

}


void Array::pushInt64(Int64 value)
{
    assert(m_Type == dtInt64);
    if (m_UsedSize >= m_Size)
    {
		extend<Int64>(2);
    }    

    *pointerCast<Int64*>(m_UsedSize) = value;
    ++m_UsedSize;
}

void Array::setInt64(Int64 value,UInt32 index)
{
	assert(m_Type == dtInt64);
	while (index > m_UsedSize)
	{
		pushInt64((Int64)-1);
	}

	if (index == m_UsedSize)
		pushInt64(value);
	else
		*pointerCast<Int64*>(index) = value;
}

void Array::pushBack(const char * value)
{
    assert(m_Type == dtString);
    if (m_UsedSize >= m_Size)
    {
		extendString(2);
    }   

    strncpy(pointerCast<char*>(m_UsedSize * m_InternalLength),value,m_Length);
    ++m_UsedSize;
}

void Array::pushBack(const string & value)
{
	pushBack(value.c_str());
}

void Array::setValue(const char *  value,UInt32 index)
{
	assert(m_Type == dtString);
	while (index > m_UsedSize)
	{
		pushBack("");
	}

	if (index == m_UsedSize)
		pushBack(value);
	else
		strncpy(pointerCast<char*>(index * m_InternalLength),value,m_Length);
}

void Array::setValue(const string & value,UInt32 index)
{
	setValue(value.c_str(),index);
}


void Array::popBack()
{
    if (m_UsedSize > 0)
    {
        if (m_Type == dtString)
        {
            memset(pointerCast<char*>(m_InternalLength * (m_UsedSize -1)),0x00,m_Length);
        }
        --m_UsedSize;
    }
}
  
void Array::sort()
{
  g_Array = this;
  qsort(m_Address,m_UsedSize,m_InternalLength,sort_cmp);

}

///////////////////////////////////////////////////
DataBuffer::DataBuffer()
{

}

DataBuffer::DataBuffer(const DataBuffer & inBuf)
{
	m_Arrays.clear();
	m_Arrays = inBuf.m_Arrays;
}

DataBuffer & DataBuffer::operator = (const DataBuffer & inBuf)
{
	if (this == &inBuf)
		return *this;

	m_Arrays.clear();
	m_Arrays = inBuf.m_Arrays;

	return *this;
}

DataBuffer::~DataBuffer()
{
    m_Arrays.clear();
}

void DataBuffer::newField(const char * name,DataType type,UInt32 length,UInt32 reserve)
{
    if (existField(name))
        THROW_EXP(CException,string(name) + "域已经存在!");
               
    m_Arrays.insert(make_pair(name,Array(type,reserve,length) ) );
}

void DataBuffer::newField(const string & name,DataType type,UInt32 length,UInt32 reserve)
{
	newField(name.c_str(),type,length,reserve);
}

void DataBuffer::deleteField(const char * name)
{
    DataBuffer::iterator it = findFieldEx(name);
    m_Arrays.erase(it);
}

void DataBuffer::deleteAllField()
{
	 m_Arrays.clear();
}

void DataBuffer::deleteField(const string & name)
{
	deleteField(name.c_str());
}


bool DataBuffer::existField(const char * name)
{
    return (findField(name) != m_Arrays.end());
}

bool DataBuffer::existField(const string & name)
{
	return existField(name.c_str());
}

bool DataBuffer::existValue(const char *name,UInt32 index)
{
	DataBuffer::iterator it =  m_Arrays.find(name);
	if (it == m_Arrays.end())
		return false;
	
	return (it->second.size() > index);
}

bool DataBuffer::existValue(const  string & name,UInt32 index)
{
	return existValue(name.c_str(),index);
}


void DataBuffer::pushBack(const char * name,int value)
{
    DataBuffer::iterator it = findFieldEx(name);
	
	if (it->second.getDataType() != dtString)
		it->second.pushBack(value);
	else
		THROW_EXP(CException,string(name) + "类型不匹配!");
}

void DataBuffer::pushBack(const string & name,int value)
{
	pushBack(name.c_str(),value);
}


void DataBuffer::pushInt64(const char * name,Int64 value)
{
	DataBuffer::iterator it = findFieldEx(name);
	if (it->second.getDataType() != dtInt64)
		THROW_EXP(CException,string(name) + "类型不匹配!");

	it->second.pushInt64(value);
}

void DataBuffer::pushInt64(const string & name,Int64 value)
{
	pushInt64(name.c_str(),value);
}

void DataBuffer::setValue(const char * name,int value,UInt32 index)
{
	DataBuffer::iterator it = findFieldEx(name);
	if (it->second.getDataType() != dtString)
		it->second.setValue(value,index);
	else
		THROW_EXP(CException,string(name) + "类型不匹配!");
}

void DataBuffer::setValue(const string & name,int value,UInt32 index)
{
	setValue(name.c_str(),value,index);
}


void DataBuffer::setInt64(const char * name,Int64 value,UInt32 index)
{
	DataBuffer::iterator it = findFieldEx(name);

	if (it->second.getDataType() != dtInt64)
		THROW_EXP(CException,string(name) + "类型不匹配!");

	it->second.setInt64(value,index);
}


void DataBuffer::setInt64(const string & name,Int64 value,UInt32 index)
{
	setInt64(name.c_str(),value,index);
}


void DataBuffer::pushDouble(const char * name,double value)
{
    DataBuffer::iterator it = findFieldEx(name);
	if (it->second.getDataType() != dtString)
		it->second.pushDouble(value);
	else
		THROW_EXP(CException,string(name) + "类型不匹配!");
}
void DataBuffer::pushDouble(const string & name,double value)
{
	pushDouble(name.c_str(),value);
}

void DataBuffer::setDouble(const char * name,double value,UInt32 index)
{
	DataBuffer::iterator it = findFieldEx(name);

	if (it->second.getDataType() != dtString)
		it->second.setDouble(value,index);
	else
		THROW_EXP(CException,string(name) + "类型不匹配!");

}
void DataBuffer::setDouble(const string & name,double value,UInt32 index)
{
	setDouble(name.c_str(),value,index);
}

void DataBuffer::pushBack(const char * name,const char * value)
{
   DataBuffer::iterator it = findFieldEx(name);
   if (it->second.getDataType() != dtString)
	   THROW_EXP(CException,string(name) + "类型不匹配!");

   it->second.pushBack(value);
}

void DataBuffer::pushBack(const string & name,const string & value)
{
	pushBack(name.c_str(),value.c_str());
}

void DataBuffer::setValue(const char * name,const char * value,UInt32 index)
{
	DataBuffer::iterator it = findFieldEx(name);
	if (it->second.getDataType() != dtString)
		THROW_EXP(CException,string(name) + "类型不匹配!");

	it->second.setValue(value,index);
}

void DataBuffer::setValue(const string & name,const string & value,UInt32 index)
{
	setValue(name.c_str(),value.c_str(),index);
}

void DataBuffer::popBack(const char * name)
{
    DataBuffer::iterator it = findFieldEx(name);
    it->second.popBack();
}
void DataBuffer::popBack(const string & name)
{
	popBack(name.c_str());
}

int DataBuffer::getInt(const char * name,UInt32 index)
{
    DataBuffer::iterator it = findFieldEx(name);
	if (index >= it->second.size())
		THROW_EXP(CException,string(name) + "数组越界!");

    return it->second.getInt(index);
}

int DataBuffer::getInt(const string & name,UInt32 index)
{
	return getInt(name.c_str(),index);
}

Int64 DataBuffer::getInt64(const char * name,UInt32 index)
{
	DataBuffer::iterator it = findFieldEx(name);
	if (index >= it->second.size())
		THROW_EXP(CException,string(name) + "数组越界!");

	return it->second.getInt64(index);
}

Int64 DataBuffer::getInt64(const string & name,UInt32 index)
{
	return getInt64(name.c_str(),index);
}


double DataBuffer::getDouble(const char * name,UInt32 index)
{
    DataBuffer::iterator it = findFieldEx(name);
	if (index >= it->second.size())
		THROW_EXP(CException,string(name) + "数组越界!");

    return it->second.getDouble(index);
}

double DataBuffer::getDouble(const string & name,UInt32 index)
{
	return getDouble(name.c_str(),index);
}

const char * DataBuffer::getString(const char * name,UInt32 index)
{
    DataBuffer::iterator it = findFieldEx(name);
	if (index >= it->second.size())
		THROW_EXP(CException,string(name) + "数组越界!");

    return it->second.getString(index);
}

const char * DataBuffer::getString(const string & name,UInt32 index)
{
	return getString(name.c_str(),index);
}

UInt32 DataBuffer::count(const char * name)
{
	DataBuffer::iterator it = findFieldEx(name);
	return it->second.size();
}

UInt32 DataBuffer::count(const string & name)
{
	return count(name.c_str());
}

UInt32 DataBuffer::fieldCount()
{
	return m_Arrays.size();
}

void DataBuffer::sort(const char * name)
{
    DataBuffer::iterator it = findFieldEx(name);
    it->second.sort();
}
void DataBuffer::sort(const string & name)
{
	this->sort(name.c_str());
}

void DataBuffer::clear()
{
	ARRAYS::iterator it = m_Arrays.begin();
	for (; it != m_Arrays.end(); ++it)
	{
		it->second.clear();
	}
}
void DataBuffer::clear(const char * name)
{
	DataBuffer::iterator it = findFieldEx(name);
	it->second.clear();
}

void DataBuffer::clear(const string & name)
{
	clear(name.c_str());
}

ostream & operator<<(ostream& os, DataBuffer& buf)
{
	for (DataBuffer::iterator it = buf.begin(); it != buf.end(); ++it)
	{
		os  << "[name=" << it->first <<",dataType=";
		if (it->second.getDataType() == dtInteger)
			os <<"dtInteger]:";
		else if (it->second.getDataType() == dtDouble)
			os <<"dtDouble]:";
		else if (it->second.getDataType() == dtString)
			os <<"dtString]:";
		else if (it->second.getDataType() == dtInt64)
			os <<"dtInt64]:";
		
		for (size_t i = 0; i < it->second.size(); ++i)
		{
			if (it->second.getDataType() == dtInteger)
				os << it->second.getInt(i) <<",";
			else if (it->second.getDataType() == dtDouble)
				os << it->second.getDouble(i) <<",";
			else if (it->second.getDataType() == dtString)
				os << it->second.getString(i) <<",";
			else if (it->second.getDataType() == dtInt64)
				os << it->second.getInt64(i) <<",";
		}
		os << endl;
	}
	return os;
}

void CopyBufferField(DataBuffer &desc,DataBuffer & src,const char * name)
{
	DataBuffer::iterator itSrc  = src.findFieldEx(name);
	DataBuffer::iterator itDesc = desc.findField(name);
	if (itDesc != desc.end())
	{//存在对应字段

		if (itDesc->second.getDataType() != itSrc->second.getDataType())
			THROW_EXP(CException,string(name) + " 域类型不一致!");
	}
	else
	{
		desc.newField(name,itSrc->second.getDataType(),itSrc->second.length());
	}
	
	desc.clear(name);
	for (int i = 0; i < src.count(name); ++i)
	{
		if (itSrc->second.getDataType() == dtString)
			desc.pushBack(name,src.getString(name,i));
		else if (itSrc->second.getDataType() == dtInteger)
			desc.pushBack(name,src.getInt(name,i));
		else if (itSrc->second.getDataType() == dtInt64) 
			desc.pushInt64(name,src.getInt64(name,i));
		else if (itSrc->second.getDataType() == dtDouble) 
			desc.pushBack(name,src.getDouble(name,i));
	}
}

void CopyBufferField(DataBuffer &desc,DataBuffer & src,const string & name)
{
	CopyBufferField(desc,src,name.c_str());
}


//
