template<class T>
vector<PO_AttrFactors*> * FactorParamMgr<T>::m_pFactors=NULL;
  
template<class T>
bool FactorParamMgr<T>::isMathFactors(const T &r_element) const
{
  int  t_p1,t_p2,t_p3,t_cmp;
	bool t_flag;
	for(int i=0;i<m_factorSize;++i)
	{
	  t_flag = false;
		t_p1 = 0;t_p3=m_factors[i]->m_record.m_fvalues.size()-1;
		while(t_p1<=t_p3)
		{
			t_p2 = (t_p3+t_p1)/2;
			t_cmp = strcmp(r_element.m_factors[m_factors[i]->m_record.m_factn].c_str(),
			   					   m_factors[i]->m_record.m_fvalues[t_p2].c_str());
			if(t_cmp == 0) 
			{ t_flag = true;break;}
			else if (t_cmp <0)
			{
				t_p3=t_p2-1;
			}else
			{
				t_p1=t_p2+1;
			}
		}
		if(t_flag == false) return false;
	}
	return true;
}

//比较元素的大小，相当于< 用于排序 考虑优先级
template<class T>
bool FactorParamMgr<T>::cmpElement_sort(const T &r_left,const T &r_right)
{
  if(!(r_left == r_right)) return (r_left<r_right);
  int t_cmp;
 	for(vector<PO_AttrFactors*>::const_iterator 
 	    t_itr =FactorParamMgr<T>::m_pFactors->begin();
 		  t_itr!=FactorParamMgr<T>::m_pFactors->end();++t_itr)
	{
	  t_cmp = strcmp(r_left .m_factors[(*t_itr)->m_record.m_factn].c_str(),
	                 r_right.m_factors[(*t_itr)->m_record.m_factn].c_str());
	  if(t_cmp==0) continue;
	  return (t_cmp<0);
	}
 	//按优先级排倒序
 	return (r_left.m_priority > r_right.m_priority);
} 
//比较元素的大小，相当于< 用于查找 不考虑优先级
template<class T>
bool FactorParamMgr<T>::cmpElement_search(const T &r_left,const T &r_right)
{
  if(!(r_left == r_right)) return (r_left<r_right);
  int t_cmp;
 	for(vector<PO_AttrFactors*>::const_iterator 
 	    t_itr =FactorParamMgr<T>::m_pFactors->begin();
 		  t_itr!=FactorParamMgr<T>::m_pFactors->end();++t_itr)
	{
	  t_cmp = strcmp(r_left .m_factors[(*t_itr)->m_record.m_factn].c_str(),
	                 r_right.m_factors[(*t_itr)->m_record.m_factn].c_str());
	  if(t_cmp==0) continue;
	  return (t_cmp<0);
	}
 	return false;
}
//比较元素*的大小，相当于< 用于排序 考虑优先级
template<class T>
bool FactorParamMgr<T>::cmpPElement_sort(const T* r_left,const T * r_right)
{
	return FactorParamMgr<T>::cmpElement_sort(*r_left,*r_right);
}
//比较FeebaseTP*的大小，相当于< 用于查询 不考虑优先级
template<class T>
bool FactorParamMgr<T>::cmpPElement_search(const T* r_left,const T * r_right)
{
	return FactorParamMgr<T>::cmpElement_search(*r_left,*r_right);
}

template<class T>
bool FactorParamMgr<T>::getElement(const T &r_element,const int &r_fSize,const PCDR &r_pcdr,T * (&r_pElement)) 
{
	FactorParamMgr<T>::m_pFactors = &m_factors;
 #ifdef FP_DEBUG
    FP_BEGIN(equal_range)
  #endif
	m_equalItr = equal_range(m_pElementList.begin(),m_pElementList.end(),
	                         &r_element,FactorParamMgr<T>::cmpPElement_search);
	m_itr = m_equalItr.first;m_upperItr = m_equalItr.second;
 #ifdef FP_DEBUG
    FP_END(equal_range)
  #endif
	while(m_itr != m_upperItr)
	{	// !(*m_itr <  r_element) 
		m_result = true;
		for(int i=0,fPos=0;i<=r_fSize;++i)
		{//如果和cmpPElement_search中比较的是同一个字段，则不需再比较
			if(fPos < m_factorSize)
			{	
				if(i==m_factors[fPos]->m_record.m_factn)
				{fPos++;continue; }
			}
			if((*m_itr)->m_factors[i]=="*" ||
			   r_element.m_factors[i]==(*m_itr)->m_factors[i])
			{
				continue;
			}else
			{m_result=false;break;}
		}
		if(m_result==true) 
		{
      if((*m_itr)->m_ruleId==DEFAULT_RULE_ID )
      {
  			r_pElement = *m_itr;
  			return true;
      }else if((*m_itr)->m_condRule != NULL)
      {
        try
        {
          if((*m_itr)->m_condRule->execute()==true)
          {
      			r_pElement = *m_itr;
      			return true;
      		}
      	}catch(...)
        {
          return false;
        }
      }else continue;
		}
		++m_itr;
	}
	return false;
}

template<class T>
bool FactorParamMgr<T>::getElement(const T &r_element,const int &r_fSize,const PCDR &r_pcdr,vector<T *> &r_pElementList)
{
  r_pElementList.clear();
	FactorParamMgr<T>::m_pFactors = &m_factors;
	m_equalItr = equal_range(m_pElementList.begin(),m_pElementList.end(),
	                         &r_element,FactorParamMgr<T>::cmpPElement_search);
	m_itr = m_equalItr.first;m_upperItr = m_equalItr.second;
	while(m_itr != m_upperItr)
	{	// !(*m_itr <  r_element) 
		m_result = true;
		for(int i=0,fPos=0;i<r_fSize;++i)
		{//如果和cmpPElement_search中比较的是同一个字段，则不需再比较
			if(fPos < m_factorSize)
			{	
				if(i==m_factors[fPos]->m_record.m_factn)
				{fPos++;continue; }
			}
			if(r_element.m_factors[i]=="*" ||
			   (*m_itr)->m_factors[i]=="*" ||
			   r_element.m_factors[i]==(*m_itr)->m_factors[i])
			{
				continue;
			}else 
			{m_result=false;break;}
		}
		if(m_result==true) 
		{
      if((*m_itr)->m_ruleId==DEFAULT_RULE_ID )
      {
  			r_pElementList.push_back(*m_itr);
      }else if((*m_itr)->m_condRule != NULL)
      {
        try
        {
          if((*m_itr)->m_condRule->execute()==true)
          {
            r_pElementList.push_back(*m_itr);
      		}
      	}catch(...)
        {
          return false;
        }
      }else continue;
		}
		++m_itr;
	}
	return (!r_pElementList.empty());
}

template<class T>
void FactorParamMgr<T>::clear()
{
	m_pElementList.clear();
	m_elementList.clear();
	m_factors.clear();
	m_tmpTpList[0].clear();
	m_tmpTpList[1].clear();
}
template<class T>
void FactorParamMgr<T>::sort()
{
	m_pElementList.clear();
	for(vector<T>::iterator t_itr=m_elementList.begin();
			t_itr!=m_elementList.end();t_itr++)
	{
		m_pElementList.push_back(&(*t_itr));
	}
	FactorParamMgr<T>::m_pFactors = &m_factors;
	::sort(m_pElementList.begin(),m_pElementList.end(),FactorParamMgr<T>::cmpPElement_sort);
}
template<class T>
void FactorParamMgr<T>::push_back(const T &r_element)
{
	int t_first=0,t_next=1;
	m_tmpTpList[t_first].clear();
	m_tmpTpList[t_first].push_back(r_element);
	for(vector<PO_AttrFactors*>::const_iterator t_itr=m_factors.begin();
		t_itr != m_factors.end();++t_itr)
	{
		m_tmpTpList[t_next].clear(); 
		for(vector<T>::iterator t_eleItr = m_tmpTpList[t_first].begin();
				t_eleItr!=m_tmpTpList[t_first].end();++t_eleItr)
		{
			if(t_eleItr->m_factors[(*t_itr)->m_record.m_factn]==(char *)"*")
			{
				for(vector<string>::const_iterator t_strItr=
				  (*t_itr)->m_record.m_fvalues.begin();
					t_strItr!=(*t_itr)->m_record.m_fvalues.end();++t_strItr)
				{
					t_eleItr->m_factors[(*t_itr)->m_record.m_factn]=*t_strItr;
					m_tmpTpList[t_next].push_back(*t_eleItr);
				}
			}else if(isMatchFactor(t_eleItr->m_factors[(*t_itr)->m_record.m_factn],
														 (*t_itr)->m_record.m_fvalues)==true)
			{	//如果tpItr->m_factors[itr->m_record.m_factn]不在
				//    itr->m_record.m_fvalues中,则不加入
				m_tmpTpList[t_next].push_back(*t_eleItr);
			}
		}
		t_first = (t_first+1)%2;t_next = (t_next+1)%2;
	}
	for(vector<T>::iterator t_eleItr2=m_tmpTpList[t_first].begin();
		  t_eleItr2!=m_tmpTpList[t_first].end();++t_eleItr2)
	{
		m_elementList.push_back(*t_eleItr2);
	}
}
template<class T>
bool FactorParamMgr<T>::isMatchFactor(const T_ATTRIBUTE_VALUE &r_attribute,
                                      const vector<string>    &r_fAttrList) 
{
	for(int i=0;i<r_fAttrList.size();i++)
	{
		if(r_attribute == r_fAttrList[i]) 
		  return true;
	}
	return false;
}
template<class T>
void FactorParamMgr<T>::setFactors(const vector<PO_AttrFactors*> &r_factorList)
{
	m_factors   =r_factorList;
	m_factorSize=m_factors.size();
	//将m_factors 按照ID排序
	PO_AttrFactors* t_tmpFactor;
	for(int i=0,tmpPos=0;i<m_factorSize;i++)
	{
		tmpPos = i;
		for(int j=i+1;j<m_factorSize;j++)
		{
			if(m_factors[j]->m_record.m_factn<m_factors[tmpPos]->m_record.m_factn )
			{
				tmpPos  = j;
			}
		}	
		if(tmpPos != i)
		{	//交换
			t_tmpFactor   =m_factors[i] ;
			m_factors[i]  =m_factors[tmpPos];
			m_factors[tmpPos]=t_tmpFactor;
		}
	}
}

template<class T>
void FactorParamMgr<T>::dump(ostream & os)
{
	os<<"___________FactorParamMgr m_bizType="<<m_bizType<<" _______________"<<endl;
	os<<"\t______________PO_AttrFactors_________________"<<endl;
	for(vector<PO_AttrFactors*>::const_iterator t_fItr=m_factors.begin();
			t_fItr!=m_factors.end();++t_fItr)
	{
		os<<"\t"<<**t_fItr<<endl;
	}
	os<<"\t______________Element List size()="<<m_pElementList.size()<<" __________________"<<endl;
	
	for(vector<T*>::const_iterator t_eleItr=m_pElementList.begin();
			t_eleItr!=m_pElementList.end();++t_eleItr)
	{
			os<<"\t"<<**t_eleItr<<endl;
	}
	
	os<<"_______________________________________________________________"<<endl;
}
