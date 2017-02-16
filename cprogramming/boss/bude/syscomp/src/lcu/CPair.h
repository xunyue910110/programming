/**
 *
 * <p>Title: CPair 一对数据类</p>
 * <p>Description: 该类按照first比较大小</p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: Linkage</p>
 * @author wangxr
 * @version 1.0
 */
#ifndef CPAIR_HEADER_INCLUDED
#define CPAIR_HEADER_INCLUDED
#include "system.h"

template<class T1, class T2>
class CPair
{
public:
    T1 first;
    T2 second;
    
    /**
     * 函数名称：CPair<T1,T2>(const CPair<T1,T2> & right)
     * 函数功能：拷贝构造函数
     * 使用注意：无
     * @param   right           要拷贝的对象
     * @return  
     * @throws
     */   
    CPair<T1,T2>(const CPair<T1,T2> & right){
        first = right.first;
        second = right.second;
    }
    
    /**
     * 函数名称：CPair<T1, T2> & operator = (const CPair<T1, T2> & right)
     * 函数功能：赋值函数重载
     * 使用注意：无
     * @param   right           右操作数
     * @return  
     * @throws
     */  
    CPair<T1, T2> & operator = (const CPair<T1, T2> & right){
        first = right.first;
        second = right.second;
        return * this;
    }

    /**
     * 函数名称：bool operator < (const CPair<T1, T2> & right)
     * 函数功能：小于比较函数重载，按照first的值比较
     * 使用注意：无
     * @param   right           右操作数
     * @return  
     * @throws  
     */  
    bool operator < (const CPair<T1, T2> & right) const{
        return first < right.first;
    }

    bool operator > (const CPair<T1, T2> & right) const{
        return first > right.first;
    }
    
    bool operator >= (const CPair<T1, T2> & right) const{
        return first >= right.first;
    }
    
    bool operator <= (const CPair<T1, T2> & right) const{
        return first <= right.first;
    }
    
    bool operator != (const CPair<T1, T2> & right) const{
        return first != right.first;
    }
    
    bool operator == (const CPair<T1, T2> & right) const{
        return first == right.first;
    }

    /**
     * 函数名称：CPair()
     * 函数功能：普通构造函数
     * 使用注意：无
     * @param
     * @return  
     * @throws  
     */  
    CPair(){}
    
     /**
     * 函数名称：CPair(const T1 & val1, const T2 & val2)
     * 函数功能：构造函数，first和second 的初值为val1与val2
     * 使用注意：无
     * @param   val1
     * @param   val2
     * @return  
     * @throws  
     */  
    CPair(const T1 & val1, const T2 & val2){
        first = val1;
        second = val2;
    }
    
    /**
     * 函数名称：virtual ~CPair()
     * 函数功能：析构函数
     * 使用注意：无
     * @param
     * @return  
     * @throws  
     */  
    virtual ~CPair(){}; 
};

template<class T1, class T2, class T3>
struct triangle
{
    T1 first;
    T2 second;
    T3 third;
    
    triangle<T1,T2,T3>(){}
    
    triangle<T1,T2,T3>(const T1 & i1, const T2 & i2, const T3 & i3): first(i1),second(i2),third(i3)
    {}
    
    triangle<T1,T2,T3>(const triangle<T1,T2,T3> & right){
        first = right.first;
        second = right.second;
        third = right.third;
    }
    
    triangle<T1,T2,T3> & operator = (const triangle<T1,T2,T3> & right){
        first = right.first;
        second = right.second;
        third = right.third;
        return * this;
    }
};

#endif  /* CPAIR_HEADER_INCLUDED */

