//##ModelId=3B8F7D0F0288
template <class T>
bool ParamContainer<T>::dataIn(StringVector &recordSet)
{
    StringVector record;
	StringVectorIterator recordItr;
	PO_Property property;
	int fieldnum;

	m_param.getProperty(property);
	fieldnum=property.fieldNum;
	record.resize(fieldnum);
	for(recordItr=recordSet.begin();recordItr<recordSet.end();recordItr+=fieldnum)
	{
		for(int i=0;i<fieldnum;i++)
		{
			if(recordItr+i>=recordSet.end())
			{
				return false;
			}
			record[i]=(*(recordItr+i));
		}
		if(!insert(record))return false;
	}
	return true;
}

template <class T>
bool ParamContainer<T>::rewind()
{
	m_point=0;
	return true;
}


template <class T>
bool ParamContainer<T>::prepareSize(long size)
{
	m_paramVector.reserve(size);
	return true;
}


template <class T>
bool ParamContainer<T>::resize(long size)
{
	if(m_paramVector.size()!=size)
	{
		m_paramVector.resize(size);
	}
	return true;
}

//##ModelId=3B8F7D0F0350
template <class T>
bool ParamContainer<T>::clean()
{
	m_paramVector.clear();
	return true;
}

//##ModelId=3B8F7D0F03C8
template <class T>
bool ParamContainer<T>::sort()
{
	//::sort(m_paramVector.begin(),m_paramVector.end());
	//::stable_sort(m_paramVector.begin(),m_paramVector.end());
	::partial_sort(m_paramVector.begin(),m_paramVector.end(),m_paramVector.end());
	//m_paramVector.sort();
	return true;
}

template <class T>
bool ParamContainer<T>::dump(ostream& os)
{
	for(itr=m_paramVector.begin();itr!=m_paramVector.end();itr++)
	{
		os<<*itr<<endl;
	}
	return true;
}


template <class T>
bool ParamContainer<T>::getProperty(PO_Property &property)
{
	return m_param.getProperty(property);
}

//##ModelId=3B6275980309
template <class T>
int ParamContainer<T>::search(const T &record,T &result)
{
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);
	if(itr==m_paramVector.end()||!(*itr==record))return 0;
	result=*itr;
	return 1;
}

template <class T>
int ParamContainer<T>::search(const T &record,T* &result)
{
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);
	if(itr==m_paramVector.end()||!(*itr==record))return 0;
	result=&(*itr);
	return 1;
}

//##ModelId=3B8F55730377
template <class T>
int ParamContainer<T>::searchMulti(const T &record, vector<T> &result)
{
	int i=0;

	result.clear();
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);

	for(;itr!=m_paramVector.end();itr++)
	{
		if(!(*itr==record))break;
		//if(*itr!=record)break;
		result.push_back(*itr);
		i++;
	}
	return i;
}

template <class T>
int ParamContainer<T>::searchMulti(const T &record, vector<T*> &result)
{
	int i=0;

	result.clear();
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);

	for(;itr!=m_paramVector.end();itr++)
	{
		if(!(*itr==record))break;
		//if(*itr!=record)break;
		result.push_back(&(*itr));
		i++;
	}
	return i;
}

//##ModelId=3B8F55730377
template <class T>
int ParamContainer<T>::searchMulti(const T &record, list<T> &result)
{
	int i=0;

	result.clear();
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);

	for(;itr!=m_paramVector.end();itr++)
	{
		if(!(*itr==record))break;
		//if(*itr!=record)break;
		result.push_back(*itr);
		i++;
	}
	return i;
}

template <class T>
int ParamContainer<T>::searchMulti(const T &record, list<T*> &result)
{
	int i=0;

	result.clear();
	itr=lower_bound(m_paramVector.begin(),m_paramVector.end(),record);

	for(;itr!=m_paramVector.end();itr++)
	{
		if(!(*itr==record))break;
		//if(*itr!=record)break;
		result.push_back(&(*itr));
		i++;
	}
	return i;
}

template <class T>
int ParamContainer<T>::scanSearch(const T &record,T &result)
{
	for(itr=m_paramVector.begin();itr!=m_paramVector.end();itr++)
	{
		if(*itr==record)
		{
			result=*itr;
			return 1;
		}
	}
	return 0;
}

template <class T>
int ParamContainer<T>::scanSearch(const T &record,T* &result)
{
	for(itr=m_paramVector.begin();itr!=m_paramVector.end();itr++)
	{
		if(*itr==record)
		{
			result=&(*itr);
			return 1;
		}
	}
	return 0;
}

template <class T>
int ParamContainer<T>::scanSearchMulti(const T &record, vector<T> &result)
{
	int i=0;

	result.clear();
	for(itr=m_paramVector.begin();itr!=m_paramVector.end();itr++)
	{
		if(*itr==record)
		{
			result.push_back(*itr);
			i++;
		}
	}
	return i;
}

template <class T>
int ParamContainer<T>::scanSearchMulti(const T &record, vector<T*> &result)
{
	int i=0;

	result.clear();
	for(itr=m_paramVector.begin();itr!=m_paramVector.end();itr++)
	{
		if(*itr==record)
		{
			result.push_back(&(*itr));
			i++;
		}
	}
	return i;
}

//##ModelId=3B90369B029E
template <class T>
bool ParamContainer<T>::insert(StringVector &record)
{
	if(!m_param.convert(record))return false;
	if(m_paramVector.size()<=m_point)
	{
		m_paramVector.push_back(m_param);
	}
	else m_paramVector[m_point]=m_param;
	m_point++;
	return true;
}

template <class T>
bool ParamContainer<T>::getParamVector(vector<T>  &result)
{
	result=m_paramVector;
	return true;
}

template <class T>
bool ParamContainer<T>::getParamVector(vector<T*>  &result)
{
	result.clear();
	for(vecotr<T>::iterator t_itr=m_paramVector.begin();
	    t_itr!=m_paramVector.end();++t_itr)
  {
    result.push_back(&(*t_itr));
  }
	return true;
}

template <class T>
vector<T> & ParamContainer<T>::getParamVector()
{

	return m_paramVector;
}
template <class T>
void ParamContainer<T>::setLocalProperty(PO_Property* prop)
{
    if(prop==NULL)
    {
        m_param.getProperty(m_localProperty);
        m_localProperty.fileModifyTime = -1;
    }
    else
    {
        m_localProperty.dataSource = prop->dataSource;
        m_localProperty.updateCondition = prop->updateCondition;
        m_localProperty.filePath = prop->filePath;
        m_localProperty.fileName = prop->fileName;
        m_localProperty.userName = prop->userName;
        m_localProperty.passWord = prop->passWord;
        m_localProperty.servName = prop->servName;
        m_localProperty.sqlStatement = prop->sqlStatement;
        m_localProperty.fieldNum = prop->fieldNum;
        m_localProperty.countSql = prop->countSql;
        //m_localProperty.fileModifyTime = prop->fileModifyTime;
    }
}

template <class T>
bool ParamContainer<T>::buildNextRelation()//zhaogc add
{	
	return true;
}

template <class T>
PO_Property* ParamContainer<T>::getLocalProperty()
{
    return &m_localProperty;
}

