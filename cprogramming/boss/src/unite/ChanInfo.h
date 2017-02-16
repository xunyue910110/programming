// #################################################################
// Language:    C++
// OS Platform: UNIX
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// Editors:     Yang Zhang  <zhangy@lianchuang.com>
// Update       20011030
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// Comments: 
// #################################################################

#ifndef CHANNELINFO_H_INCLUDED
#define CHANNELINFO_H_INCLUDED

#include "config-all.h"
#include <string>
#include <map>
#include <vector>

USING_NAMESPACE(std)

//typedef map<string,vector<string>*> CDR_POOL;
//typedef CDR_POOL::iterator CDR_POOL_ITR;
//typedef map<string,CDR_POOL *> CDR_POOL_MAPS;
//typedef CDR_POOL_MAPS::iterator CDR_POOL_MAPS_ITR; 
//
    
class CDR_POOL:public multimap<string,vector<string>*>{};
//typedef map<string,vector<string>*> CDR_POOL;
typedef CDR_POOL::iterator CDR_POOL_ITR;
class CDR_POOL_MAPS:public map<string,CDR_POOL *>{};
//typedef map<string,CDR_POOL *> CDR_POOL_MAPS;
typedef CDR_POOL_MAPS::iterator CDR_POOL_MAPS_ITR;     
   
    

USING_NAMESPACE(std)

const int MAXCDRSIZE     = 1024;

class ChanInfo
{
	public:
		ChanInfo()
		{
			m_cdrPoolMaps.clear();
			m_desPath   = ""; 
			m_srcPath   = "";
			m_subChanNo = 0;	
		}
		~ChanInfo()
		{
			CDR_POOL_ITR itr;        //缓冲池迭代器
			
			CDR_POOL_MAPS_ITR itrPoolMaps;    //缓冲池集合迭代器
			// 删除cdr字段vector指针
			itrPoolMaps = m_cdrPoolMaps.begin();
			for(int j=0;j<m_cdrPoolMaps.size();++j,++itrPoolMaps)
			{
				itr=(itrPoolMaps->second)->begin();             //缓冲池
			
				for(int i=0;i<(itrPoolMaps->second)->size();++i,++itr)
				{
					if( itr->second != NULL )
					{
						// 删除 vector<string>*
						delete itr->second;	
					}	
				}
				(itrPoolMaps->second)->clear();
				// 删除 map<string,vector<string>*>
				delete (itrPoolMaps->second);
				
			}
			m_cdrPoolMaps.clear();	
		}
		
		// 用于不同业务话单合并的缓冲池集合
        CDR_POOL_MAPS m_cdrPoolMaps;
        // 输入,输出目录
        string m_desPath;
		string m_srcPath;
		// 用于区分保存缓存信息的文件的文件名
		int m_subChanNo;
};

#endif
