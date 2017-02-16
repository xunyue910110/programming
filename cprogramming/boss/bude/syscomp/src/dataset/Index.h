#ifndef __INDEX_H
#define __INDEX_H
/*********************************************************************************
 * 描述： Index.h
 * 说明： 索引
 * 环境： 基于标准C++,STL
 * 作者： ZHIJUN SONG(E-mail:songzj@lianchuang.com; MSN:ninyjun@hotmail.com)
 * 日期： 创建 于 2004-03-1
 * 备注： 
 **********************************************************************************/

#include <vector>
#include <assert.h>
#include <algorithm>
#include <iostream>
namespace dataset {
template <class T>
class Index
{
public:

       typedef typename std::vector< T >::iterator iterator;

       iterator  find(T &x)
       {
                iterator it = std::lower_bound(IndexData_.begin(),IndexData_.end(),x);

                if (*it != x)
                {
                       it =  IndexData_.end();
                       return  it;
                }
                else
                        return it;
       }
       
       iterator upperBound(const T &x)
       {
                iterator it = std::upper_bound(IndexData_.begin(),IndexData_.end(),x);
                return it;
       }
       
       
       

       bool locate(T & v, iterator & begin, iterator & end)
       {
                iterator it = std::lower_bound(IndexData_.begin(),IndexData_.end(),v);
                
                begin = IndexData_.end();
                end   = IndexData_.end();
                
                if (it == IndexData_.end() || !(*it == v))
                {
                      return false; 
                }
                else
                {
                        begin = it;
                        end   = it;

                        iterator it2 = upper_bound(it,IndexData_.end(),v);

                        if (it2 == IndexData_.end())
                        {
                           end = begin +1;

                        }
                        else
                        {
                            end = it2;
                        }
                }
                return true;
       }


       void push_back(T &x)
       {
                IndexData_.push_back(x);
       }

	   iterator insert(iterator & it,T &x)
	   {
	   		return IndexData_.insert(it,x);
	   }
       iterator  begin()
       {
                return IndexData_.begin();
       }

	   size_t size()
	   {
	   	 return IndexData_.size();
	   }

       iterator end()
       {
                return IndexData_.end();
       }

       void sort()
       {
         std::sort(IndexData_.begin(),IndexData_.end());
       }

       void clear()
       {
                IndexData_.clear();
       }

       void erase(iterator it)
       {
            IndexData_.erase(it);
       }


       Index()
       {
            clear();
       }
       
       
       ~Index()
       {
            clear();
       }

       void  reserve(unsigned int i)
       {
         IndexData_.reserve(i);
       }
private:
        std::vector< T > IndexData_;
};
}
#endif
