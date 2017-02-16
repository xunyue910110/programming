#ifndef __DATA_BUFFER_H
#define __DATA_BUFFER_H

#include <map>
#include <assert.h>
#include "Types.h"
#include <string>
#include <iostream>
#include "CException.h"
#include <vector>
using namespace std;

inline Int64 StrToInt64(const char * str)
{
	char temp;
	Int64 value;
#if defined(_MSC_VER)
	if (std::sscanf(str, "%I64d%c", &value, &temp) != 1)
		value = -1; 
#else
	if (std::sscanf(str, "%lld%c", &value, &temp) != 1)
		value = -1; 
#endif
	return value;
}


enum DataType 
{
    dtInteger = 0,dtDouble,dtInt64,dtString
};

class Array
{
public:
	Array(DataType type,UInt32 size,UInt32 length):
		m_Type(type),m_Size(size),m_Length(length),m_InternalLength(length),m_UsedSize(0)
	{
		m_Address = NULL;
        allocate();
	}
	
	~Array()
	{
        destory();
	}
    
    int getInt(UInt32 index)
    {
		assert(index < m_UsedSize);
		if (dtInteger == m_Type)
			return *pointerCast<int*>(index);
		else if (dtDouble == m_Type)
			return (int)*pointerCast<double*>(index);
		else if (dtInt64 == m_Type)
			return (int)*pointerCast<Int64*>(index);
		else if (dtString == m_Type)
			return atoi(pointerCast<char*>(index * m_InternalLength));
		else
			return 0;
    }
    
    double getDouble(UInt32 index)
    {
		assert(index < m_UsedSize);

		if (dtDouble == m_Type)
			return *pointerCast<double*>(index);
		else if (dtInteger == m_Type)
			return (double)*pointerCast<int*>(index);
		else if (dtInt64 == m_Type)
			return (double)*pointerCast<Int64*>(index);
		else if (dtString == m_Type)
			return atof(pointerCast<char*>(index * m_InternalLength)); 
		else
			return 0.00;
    }
    
    const char * getString(UInt32 index)
    {
		assert(index < m_UsedSize);

        return pointerCast<char*>(index * m_InternalLength);
    }

    Int64 getInt64(UInt32 index)
    {
        assert(index < m_UsedSize);

		if (dtInt64 == m_Type)
			return *pointerCast<Int64*>(index);
		else if (dtDouble == m_Type)
			return (Int64)*pointerCast<double*>(index);
		else if (dtInteger == m_Type)
			return (Int64)*pointerCast<int*>(index);
		else if (dtString == m_Type)
			return StrToInt64(pointerCast<char*>(index * m_InternalLength));
		else
			return 0;
	}

	void pushBack(int value);
	void setValue(int value,UInt32 index);

    void pushDouble(double value);
	void setDouble(double value,UInt32 index);

    void pushInt64(Int64 value);
	void setInt64(Int64 value,UInt32 index);
	
    void pushBack(const char * value);
	void setValue(const char * value,UInt32 index);

	void pushBack(const string & value);
	void setValue(const string & value,UInt32 index);
    
    void popBack();
    
    UInt32 capacity() const
	{
		return m_Size;
	}

	UInt32 size() const
	{
		return m_UsedSize;
	}
	
	UInt32 length() const
	{
		return m_Length;
	}
	
    void * getAddress()
    {
        return m_Address;
    }
    
    DataType getDataType() const 
    {
        return m_Type;
    }    
    
    void clear()
    {
       memset(m_Address,0x00,m_InternalLength * m_Size);
       m_UsedSize = 0;
    }

	Array(const Array & array);

    Array& operator = (const Array& array);

    void sort();

    
private:


    void allocate();
	
    void destory();

    template <class T> T pointerCast(UInt32 offset ) const
    {
        return reinterpret_cast< T >(m_Address) + offset;
    }

    template <class T> T * newArray(UInt32 size) const
    {
        return new T [size];
    }

    template <class T> void deleteArray()
    {
        delete [] reinterpret_cast< T >(m_Address);
        m_Address = NULL;
    }

	template<class T> void extend(UInt32 times)
	{
		void * tmp = newArray< T >(m_Size * times);
		memcpy(tmp,m_Address,m_InternalLength * m_UsedSize);
		deleteArray< T* >();
		m_Address = tmp;
		m_Size *= times;
	}

	void extendString(UInt32 times)
	{
	    void * tmp = newArray<char>(m_Size * m_InternalLength * times);
		memset(tmp,0x00,m_Size * m_InternalLength * times);
		memcpy(tmp,m_Address,m_InternalLength * m_UsedSize);

		deleteArray<char*>();
		m_Address = tmp;
		m_Size *= times;
	}

    
    DataType m_Type;

	UInt32 m_Size;
    
    UInt32 m_UsedSize;
    
    UInt32 m_Length;
    
    UInt32 m_InternalLength; //内部长度

	void* m_Address;
};
//
class DataBuffer;
class DataPumpInterface
{
public:
	DataPumpInterface(){}
	
	virtual ~DataPumpInterface(){}
	
	virtual bool getValue(DataBuffer &,const char * key) = 0;
};

class DataBuffer
{
    typedef map<string,Array> ARRAYS;

public:
	
	friend ostream & operator<<(ostream& os, DataBuffer& buf);

	typedef ARRAYS::iterator iterator;
	typedef ARRAYS::const_iterator const_iterator;


    DataBuffer();
    DataBuffer (const DataBuffer & inBuf);
	DataBuffer & operator = (const DataBuffer & inBuf);


	void registeCallbackObj(DataPumpInterface *  plugin)
	{
		int i = 0; 
		for (; i < m_dataPumps.size(); ++i)
		{
			if (plugin == m_dataPumps[i])
				break;
		}
		
		if (i == m_dataPumps.size())
			m_dataPumps.push_back(plugin);
	}

	void callback(const char * key)
	{
		for (int i = 0; i < m_dataPumps.size(); ++i)
		{
			if (m_dataPumps[i]->getValue(*this,key))
				break;
		}
	}

    ~DataBuffer();

    void newField(const char * name,DataType type,UInt32 length,UInt32 reserve = 1);
	void newField(const string & name,DataType type,UInt32 length,UInt32 reserve = 1);


    bool existField(const char * name);
	bool existField(const string & name);

    void deleteField(const char * name);
	void deleteField(const string & name);

	void deleteAllField();

	//判断某个位置是否存在值
	bool existValue(const char *name,UInt32 index = 0);
	bool existValue(const string & name,UInt32 index = 0);
    
	void pushBack(const char * name,int value);
    void pushInt64(const char * name,Int64 value);
    void pushDouble(const char * name,double value);
    void pushBack(const char * name,const char * value);

	void pushBack(const string & name,int value);
	void pushInt64(const string & name,Int64 value);
	void pushDouble(const string & name,double value);
	void pushBack(const string & name,const string & value);

    void popBack(const char * name);
	void popBack(const string & name);

	void setValue(const char * name,int value,UInt32 index = 0);
	void setInt64(const char * name,Int64 value,UInt32 index = 0);
	void setDouble(const char * name,double value,UInt32 index = 0);
	void setValue(const char * name,const char * value,UInt32 index = 0);

	void setValue(const string & name,int value,UInt32 index = 0);
	void setInt64(const string & name,Int64 value,UInt32 index = 0);
	void setDouble(const string & name,double value,UInt32 index = 0);
	void setValue(const string & name,const string & value,UInt32 index = 0);
	
	UInt32 count(const char * name);
	UInt32 count(const string & name);
	UInt32 fieldCount();
	
    int getInt(const char * name,UInt32 index = 0);
    double getDouble(const char * name,UInt32 index = 0);
    const char * getString(const char * name,UInt32 index = 0);

	int getInt(const string & name,UInt32 index = 0);
	double getDouble(const string & name,UInt32 index = 0);
	const char * getString(const string & name,UInt32 index = 0);


	Int64 getInt64(const char * name,UInt32 index = 0);
	Int64 getInt64(const string & name,UInt32 index = 0);

    void sort(const char * name);
	void sort(const string & name);

    void clear();

	void clear(const char * name);
	void clear(const string & name);

	DataBuffer::iterator  begin()
	{
		return m_Arrays.begin();
	}

	DataBuffer::iterator  end()
	{
		return m_Arrays.end();
	}

	DataBuffer::iterator findField(const char * name)
    {
    	 return m_Arrays.find(name);
    }

	DataBuffer::iterator findField(const string & name)
	{
		return findField(name.c_str());
	}

	DataBuffer::iterator findFieldEx(const char * name)
	{
		DataBuffer::iterator it = m_Arrays.find(name);
		if (it == m_Arrays.end())
			THROW_EXP(CException,string(name) + " 域不存在!");

		return it;
	}

	DataBuffer::iterator findFieldEx(const string & name)
	{
		return findFieldEx(name.c_str());
	}


	Array & getArray(const char * name)
	{
		DataBuffer::iterator it  =  findFieldEx(name);
		return it->second;
	}

private:

    ARRAYS m_Arrays;

	vector<DataPumpInterface *> m_dataPumps;

};

inline string GetString(DataBuffer & buf,const char * name, UInt32 index = 0)
{
	if (buf.existValue(name,index))
		return buf.getString(name,index);
	
	return "";
}

inline int GetInt(DataBuffer & buf,const char * name, UInt32 index = 0,long defaultValue=-1)
{
	if (buf.existValue(name,index))
		return buf.getInt(name,index);

	return defaultValue;
}

inline string GetString(DataBuffer & buf,const string & name, UInt32 index = 0)
{
	if (buf.existValue(name.c_str(),index))
		return buf.getString(name.c_str(),index);

	return "";
}

inline int GetInt(DataBuffer & buf,const string & name, UInt32 index = 0,long defaultValue=-1)
{
	if (buf.existValue(name.c_str(),index))
		return buf.getInt(name.c_str(),index);

	return defaultValue;
}


void CopyBufferField(DataBuffer &desc,DataBuffer & src,const char * name);
void CopyBufferField(DataBuffer &desc,DataBuffer & src,const string & name);

#endif //__DATA_BUFFER_H

