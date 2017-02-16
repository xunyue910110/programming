#include "HashList.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include "base/supdebug.h" 

//初始化 预分配hash大小
int HashList::construct_table( size_t r_size )
{
  //是否已经分配过
  if (m_size>0)
  {
    return 0;
  }
  if (r_size>0)
	{
	  m_addTimeWindow.clear();
	  m_addSize = 0;
		m_size=r_size;
		m_tableList = (TABLE **)malloc(sizeof(TABLE *) * m_size);
		if ( m_tableList == NULL )
		{
			m_size = 0;
			return -1;
		}
		for (size_t i=0;i<r_size;i++)
		{
      m_tableList[i] = NULL;
    }
    return (int)r_size;
	}else
	{
		return 0;
	}
}

//插入一个节点 需要两个key
//插入hash表 插入时间窗表
int HashList::insert(const HisFidInfo &r_value)
{
	unsigned t_keyVal = hash(r_value.m_fid.c_str()) % m_size;
	Bucket *t_pBucket=NULL;
	TWITER  t_twiter;
	
	t_pBucket = new Bucket(r_value);
	if ( NULL == t_pBucket)
  {
		return -1;
	}
  #ifdef FP_DEBUG
    FP_BEGIN(timeWindowFIND)
  #endif
	//增加时间窗LIST
	t_twiter=::lower_bound(m_addTimeWindow.begin(),m_addTimeWindow.end(),t_pBucket,CmpPBucket_Time());
  #ifdef FP_DEBUG
    FP_END(timeWindowFIND)
  #endif
  //modified by gaojf 2009-6-22 gaojf
  //m_addTimeWindow.insert(t_twiter,t_pBucket);
  t_twiter = m_addTimeWindow.insert(t_twiter,t_pBucket);
  ++m_addSize;
	//--t_twiter;
	
	t_pBucket->m_twiter = t_twiter;
	
	//该Hash值对应的子表为空
	if (NULL == m_tableList[t_keyVal])
	{
	  //先申请子表
	  m_tableList[t_keyVal] = new TABLE();
	  m_tableList[t_keyVal]->push_back(t_pBucket);
	}else
	{
	  t_twiter=::lower_bound(m_tableList[t_keyVal]->begin(),
	                         m_tableList[t_keyVal]->end(),
	                         t_pBucket,CmpPBucket_Fid());
    m_tableList[t_keyVal]->insert(t_twiter,t_pBucket);
	}	  
	++m_number ;
	if(m_addSize >= 5000)  commitAddTimeWindow();
	return 1;	
}

//根据hashkey 删除一组节点
int HashList::erase(const char *r_fid)
{
	unsigned t_keyVal = hash(r_fid) % m_size;
	int i=0;
	
	if (NULL == m_tableList[t_keyVal])
	{
		return 0; //没有对应的节点
	}
  Bucket  t_bucket(r_fid);
	Bucket *t_pBucket=&t_bucket;
	TWITER  t_twiter;
  t_twiter=::lower_bound(m_tableList[t_keyVal]->begin(),
                         m_tableList[t_keyVal]->end(),
                         t_pBucket,CmpPBucket_Fid());
  while(t_twiter!= m_tableList[t_keyVal]->end())
  {
    if(strcmp((*t_twiter)->m_data.m_fid.c_str(),r_fid)!=0)
    {
      break;
    }
    //delete *t_twiter;
    if((*t_twiter)->m_flag == '0')
      m_addTimeWindow.erase((*t_twiter)->m_twiter);
    else
      m_timeWindow.erase((*t_twiter)->m_twiter);
    delete *t_twiter;
    t_twiter = m_tableList[t_keyVal]->erase(t_twiter);
    i++;
    m_number--;
  };
	return i;
}

//根据list节点删除 
//r_itr 是timewind中的节点迭代
int HashList::erase(TWITER &r_itr)
{
  unsigned t_keyVal = hash((*r_itr)->m_data.m_fid.c_str()) % m_size;
  if(NULL == m_tableList[t_keyVal])
  {
    return 0;
  }

  Bucket  t_bucket((*r_itr)->m_data.m_fid.c_str());
	Bucket *t_pBucket=&t_bucket;
	TWITER  t_twiter;
  t_twiter=::lower_bound(m_tableList[t_keyVal]->begin(),
                         m_tableList[t_keyVal]->end(),
                         t_pBucket,CmpPBucket_Fid());
  while(t_twiter!= m_tableList[t_keyVal]->end())
  {
    if(strcmp((*t_twiter)->m_data.m_fid.c_str(),
              (*r_itr)->m_data.m_fid.c_str())!=0)
    {
      break;
    }
    if((*t_twiter)->m_twiter == r_itr)
    {
      //delete *t_twiter; //modified by gaojf 2009-9-18 19:33:06
      //m_timeWindow.erase(r_itr);	
      if((*r_itr)->m_flag == '0')
        m_addTimeWindow.erase(r_itr);
      else
        m_timeWindow.erase(r_itr);
      delete *t_twiter;//modified by gaojf 2009-9-18 19:33:06
      m_tableList[t_keyVal]->erase(t_twiter);
      m_number--;
      return 1;
    }else
    {
      ++t_twiter;
    }
  };
  return 0;
}	

//根据hash节点删除 
int HashList::erase(Bucket *r_node)
{
	TWITER t_itr;
	t_itr = r_node->m_twiter;
	return erase(t_itr);
}
//删除指定时间前的节点
size_t HashList::erase_time(const long &r_time)
{
  TWITER t_itr;
  size_t t_i=0;
  for(t_itr=m_timeWindow.begin();t_itr!=m_timeWindow.end();)
  {
    t_itr=m_timeWindow.begin();
    if((*t_itr)->m_data.m_beginTime <= r_time)
    {
      t_i += erase(t_itr);
    }else break;
  }
  return t_i;
}
//按时间顺序清楚指定数量的节点
size_t HashList::erase_nodeNum(const long &r_nodeNum,long &r_firstTime)	
{
  TWITER t_itr;
  size_t t_i=0;
  if(r_nodeNum<=0) return t_i;
  for(t_itr=m_timeWindow.begin();t_itr!=m_timeWindow.end();)
  {
    t_itr=m_timeWindow.begin();
    if(t_i < r_nodeNum)
    {
      r_firstTime = (*t_itr)->m_data.m_beginTime;
      t_i += erase(t_itr);
    }else if((*t_itr)->m_data.m_beginTime == r_firstTime)
    {
      t_i += erase(t_itr);
    }else
    {
      break;
    }  
  }
  return t_i;
}
//智能删除：至少删除r_time1前的节点，不能删除r_time2后的节点
//        : r_time1 > r_time2
//          在r_time1,r_time2之间，删除总数达到r_nodeNum
//    返回：删除的节点的最大时间
size_t HashList::erase_AI(const long &r_time1   ,const long &r_time2,
                          const long &r_nodeNum ,long &r_firstTime)
{
  commitAddTimeWindow();
  TWITER t_itr;
  size_t t_i=0;
  r_firstTime = 0;
  //for(t_itr=m_timeWindow.begin();t_itr!=m_timeWindow.end();)
  while(1)
  {
    if(m_timeWindow.empty()) break;
    t_itr=m_timeWindow.begin();
    if((*t_itr)->m_data.m_beginTime < r_time1)
    {
      r_firstTime = (*t_itr)->m_data.m_beginTime;
      t_i += erase(t_itr);
    }else if((*t_itr)->m_data.m_beginTime == r_firstTime)
    {
      t_i += erase(t_itr);
    }else if((*t_itr)->m_data.m_beginTime < r_time2)
    {
      if(t_i < r_nodeNum)
      {
        r_firstTime = (*t_itr)->m_data.m_beginTime;
        t_i += erase(t_itr);
      }else break;  
    }else break;
  }
  return t_i;
}

int HashList::lookup(const HisFidInfo &r_fidInfo,vector<Bucket*> &r_result)
{
	unsigned t_keyVal = hash(r_fidInfo.m_fid.c_str()) % m_size;
	Bucket  t_bucket(r_fidInfo.m_fid.c_str());
	Bucket *t_pBucket=&t_bucket;
	int i=0;
	
	r_result.clear();
	if (NULL == m_tableList[t_keyVal])
  {
		return 0;	
	}
	
	TWITER  t_twiter;
  t_twiter=::lower_bound(m_tableList[t_keyVal]->begin(),
                         m_tableList[t_keyVal]->end(),
                         t_pBucket,CmpPBucket_Fid());
  while(t_twiter!= m_tableList[t_keyVal]->end())
  {
    if(strcmp((*t_twiter)->m_data.m_fid.c_str(),
              r_fidInfo.m_fid.c_str())!=0)
    {
      break;
    }else
    {
      r_result.push_back(*t_twiter);
      ++t_twiter;
      ++i;
    }
  };

	return i;	
}	

void HashList::free_map()
{
	TWITER t_itr;
	for (int i=0;i<m_size;i++)
	{
		if (NULL != m_tableList[i])
		{
		  for(t_itr=m_tableList[i]->begin();
		      t_itr!=m_tableList[i]->end();++t_itr)
		  {
		    delete *t_itr;
		    --m_number;
		  }
		  m_tableList[i]->clear();
		  delete m_tableList[i];
		}		
	}
	m_timeWindow.clear();
	m_addTimeWindow.clear();
	if (m_size>0)
	{
		delete[] m_tableList;
	}
	m_size=0;
	m_addSize=0;
	assert(m_number==0);
}
size_t HashList::getNodeNumber()
{
  return m_number;
}

void HashList::getTimeInfo(long &r_firstTime,long &r_lastTime)
{
  if(m_timeWindow.empty())
  {
    r_firstTime=r_lastTime=0;
    return;
  }
  r_firstTime=m_timeWindow.front()->m_data.m_beginTime;
  r_lastTime =m_timeWindow.back()->m_data.m_endTime;
}

void HashList::commitAddTimeWindow()
{
  if(m_addSize==0) return ;
  TWITER  t_itr1,t_itr2;
  
  t_itr2=m_timeWindow.begin();
  for(t_itr1=m_addTimeWindow.begin();
      t_itr1!=m_addTimeWindow.end();
     )
  {
    if(t_itr2 == m_timeWindow.end())
    {
      for(;t_itr1!=m_addTimeWindow.end();++t_itr1)
      {
        (*t_itr1)->m_twiter = m_timeWindow.insert(t_itr2,*t_itr1);
        t_itr2 = m_timeWindow.end();
        //m_timeWindow.push_back(*t_itr1);
        (*t_itr1)->m_flag='1';
        //(*t_itr1)->m_twiter = m_timeWindow.end();
      }
      break;
    }else
    {
      if(CmpPBucket_Time()(*t_itr1,*t_itr2)==true)
      {
        (*t_itr1)->m_twiter = m_timeWindow.insert(t_itr2,*t_itr1);
        (*t_itr1)->m_flag='1';
        ++t_itr1;
      }else
      {
        ++t_itr2;
      }
    }
  }
  m_addSize = 0;
  m_addTimeWindow.clear();
}

//打印内容
void HashList::print_map()
{
	TWITER  t_iter;
	//print map
	cout<<"---------------print_map begin-------------"<<endl;
	cout << "SIZE:" << m_size << ",NUMBER:" << m_number << endl;
	cout << "PRINT HASH MAP:"<<endl;
	for (int i=0;i<m_size;i++)
	{
		if (NULL != m_tableList[i])
		{
			cout << "NO ： "<< i << endl;
			for (t_iter =m_tableList[i]->begin();
			     t_iter!=m_tableList[i]->end();++t_iter)
			{
			  cout << "-->" <<(*t_iter)->m_data<<"||"<<(*((*t_iter)->m_twiter))->m_data<<endl;	
			}		
		}		
	}	
	//print list
	cout << "PRINT total LIST:" << endl;
	for (t_iter=m_timeWindow.begin();t_iter!=m_timeWindow.end();t_iter++)
	{
		cout << (*t_iter)->m_data << endl;
	}	
	cout << "PRINT add LIST:" << endl;
	for (t_iter=m_addTimeWindow.begin();t_iter!=m_addTimeWindow.end();t_iter++)
	{
		cout << (*t_iter)->m_data << endl;
	}	
	cout<<"---------------print_map end-------------"<<endl;
}

