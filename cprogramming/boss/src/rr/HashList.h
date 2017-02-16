/**
*   内存过滤信息存放结构HashList.
*   @author 高将飞 <gaojf@lianchuang.com>
*           
*   @version 1.0
*   @date 2008-08-13
*/

#ifndef _RR_LIST_H_INCLUDED_20080813_
#define _RR_LIST_H_INCLUDED_20080813_

#include "FilterInfo.h"
#include <list>
#include <vector>

//定义拥有两个key的map，且两个key可以不是唯一
//一个key采用hash，用于定位随机查找 ，一个key采用list的排序顺序查找
class HashList
{
	public:
	  class Bucket;
		typedef list<Bucket*>                 TIMEWINDOW;
		typedef list<Bucket*>::iterator       TWITER;
		typedef list<Bucket*>::const_iterator CONST_TWITER;
		typedef list<Bucket*>                 TABLE;
		//定义hash List 的一个node 结构
		class  Bucket
		{
		  public:
	    	TWITER 	     m_twiter;
	    	char         m_flag;  //0-add 1-total
	    	HisFidInfo 	 m_data;
      public:
        Bucket();
        Bucket(const Bucket &r_right)
        {
          m_twiter= r_right.m_twiter;
          m_data  = r_right.m_data;
          m_flag  = r_right.m_flag;
        }
        Bucket(const char * r_fid)
        {
          m_twiter      = NULL;
          m_data.m_fid  = r_fid;
          m_flag        = '0';
        }
        Bucket(const HisFidInfo &r_right)
        {
          m_twiter= NULL;
          m_data  = r_right;
          m_flag  = '0';
        }
		}; //hash表的一个节点	
		
    class CmpPBucket_Time
    {
    	public:
    		virtual bool operator() (const Bucket *r_left, const Bucket *r_right) const
    		{
    		  return (r_left->m_data.m_beginTime < r_right->m_data.m_beginTime);
    		}
    };
    class CmpPBucket_Fid
    {
    	public:
    		virtual bool operator() (const Bucket *r_left, const Bucket *r_right) const
    		{
    		  return (r_left->m_data.m_fid < r_right->m_data.m_fid);
    		}
    };
	public:
		HashList()
		{
		  m_size=0;
		  m_number=0;
		}
		virtual ~HashList()
		{
		  free_map();
		}
		//建构
		int construct_table(size_t size);
		//插入一个节点
		int insert(const HisFidInfo &r_value);
		//根据hash kay 进行删除
		int erase(const char *r_fid);
		//根据时间窗结点进行删除
		int erase(TWITER &r_itr);
		//根据hash节点进行删除
		int erase(Bucket *r_node);
		//删除指定时间前的节点
		size_t erase_time(const long &r_time);
		//按时间顺序清楚指定数量的节点,返回：删除的节点的最大时间
		size_t erase_nodeNum(const long &r_nodeNum,long &r_firstTime);
		//智能删除：至少删除r_time1前的节点，不能删除r_time2后的节点
		//          在r_time1,r_time2之间，删除总数达到r_nodeNum
		//    返回：删除的节点的最大时间
		size_t erase_AI(const long &r_time1   ,const long &r_time2,
		                const long &r_nodeNum ,long &r_firstTime);
		
		//释放所有资源
		void free_map();
		//查找 多个放到vector 里
		int lookup(const HisFidInfo &r_fidInfo,vector<Bucket*> &r_result);
		
		size_t getNodeNumber();
		void   getTimeInfo(long &r_firstTime,long &r_lastTime);
		void   print_map();
		void   commitAddTimeWindow();
	private:
		TABLE      **m_tableList;     //hash表
		TIMEWINDOW   m_timeWindow;    //时间窗表
		size_t       m_size;          //hash表分配大小
		size_t       m_number;        //实际节点数量

		TIMEWINDOW   m_addTimeWindow; //增量时间窗
		size_t       m_addSize;       //记录数
	public:
	  //取Hash值
		static unsigned hash(const char *r_key)
		{
			unsigned t_ret_val = 0;
			int t_i ;
			
			while (*r_key)
			{
				t_i = (*r_key);
				t_ret_val ^= t_i;
				t_ret_val <<= 1;
				r_key++;
			}
			return t_ret_val;
		};
};



#endif //_RR_LIST_H_INCLUDED_20080813_
