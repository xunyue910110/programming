// Version   : 0.5.0
// Create    : 2001.8.15
// Update    : 2001.9.7
// Authors:  ZhongLiang Lu <luzl@lianchuang.com>
// Copyright (C) 2001 by Linkage Corporation. All Rights Reserved.
// Comments:
// History:

#ifndef Bill_Index_H_INCLUDED
#define Bill_Index_H_INCLUDED


#include "base/config-all.h"

#include "bill/PointerWrapper.h"
#include "bill/RefVector.h"

#include <vector>
#include <algorithm>
#include <functional>

USING_NAMESPACE(std)

// build Bill_Index on a group objects and implement search method
template < class T, class COMP = less < PointerWrapper < T > > >
class Bill_Index : private RefVector < T >
{
  typedef typename RefVector < T >::piterator piterator;

  public:
  typedef typename RefVector < T >::iterator iterator;
  typedef typename RefVector < T >::const_iterator const_iterator;

  Bill_Index()
  {
  }

  // construct and init by data which specialed by first and last
  // example:
  // int a[10] = {...};
  // Bill_Index<const int *, int> idx1(a, a+10);
  // other example:
  // vector<int> b(a, a+10);
  // Bill_Index<vector<int>::iterator, int> idx2(b.begin(), b.end());

  template < class InputIterator >
  Bill_Index(InputIterator first, InputIterator last)
  {
    reset(first, last);
  }

  template < class InputIterator >
  Bill_Index(InputIterator first, size_t size)
  {
    reset(first, size);
  }

  // rebuild Bill_Index, init by data which specialed by first and last
  template < class InputIterator >
  void reset(InputIterator first, InputIterator last)
  {
    RefVector < T >::clear();
    RefVector < T >::push_back(first, last);

    sort(this->m_ref.begin(), this->m_ref.end(), cmp);
  }

  // rebuild Bill_Index
  template < class InputIterator >
  void reset(InputIterator first, size_t size)
  {
    RefVector < T >::clear();
    RefVector < T >::push_back(first, size);

    sort(this->m_ref.begin(), this->m_ref.end(), cmp);
  }

  template < class InputIterator >
  void append(InputIterator first, InputIterator last)
  {
    if (this->size() == 0)
    {
      reset(first, this->size());
      return;
    }

    m_temp.clear();

    for ( ; first != last; ++first) m_temp.push_back(& * first);

    merge();
  }

  template < class InputIterator >
  void append(InputIterator first, size_t size)
  {
    if (this->size() == 0)
    {
      reset(first, size);
      return;
    }

    m_temp.clear();

    for (int i = 0; i < size; ++i) m_temp.push_back(& * first);

    merge();
  }

  iterator begin()
  {
    return RefVector < T >::begin();
  }

  const_iterator begin() const
  {
    return RefVector < T >::begin();
  }

  iterator end()
  {
    return RefVector < T >::end();
  }

  const_iterator end() const
  {
    return RefVector < T >::end();
  }
  
  iterator locate(T & v)
  {
    PointerWrapper < T > value(& v);
    piterator i = ::lower_bound(this->m_ref.begin(), this->m_ref.end(),
                              value, cmp);

    if (i != this->m_ref.end() && !cmp(value, * i)) return i;

    return this->m_ref.end();
  }

  iterator lower_bound(T & v)
  {
    PointerWrapper < T > value(& v);
    piterator i = ::lower_bound(this->m_ref.begin(), this->m_ref.end(),
                              value, cmp);

    if (i != this->m_ref.end()) return i;

    return this->m_ref.end();
  }

  // locate range which equal the object, example
  // find '7' in '1234777890', return: 
  //                  ^  ^
  void locate(T & v, iterator & beginIterator, iterator & endIterator)
  {
    PointerWrapper < T > value(& v);
    piterator i = lower_bound(this->m_ref.begin(), this->m_ref.end(),
                              value, cmp);

    if (i != this->m_ref.end() && !cmp(value, * i))
    {
      beginIterator = i;
      endIterator = upper_bound(this->m_ref.begin(), this->m_ref.end(),
                                value, cmp);

      return;
    }

    beginIterator = end();
    endIterator = end();
  }

  T & operator[] (size_t Bill_Index)
  {
    return RefVector < T >::operator[] (Bill_Index);
  }

  const T & operator[] (size_t Bill_Index) const
  {
    return RefVector < T >::operator[] (Bill_Index);
  }

  size_t size() const
  {
    return RefVector < T >::size();
  }

  // unique Bill_Index, Alternative, choice one when duplicate
  // false: first, true: second 
  // bool alternative(const T&, const T&);

  template < class Alternative >
  void unique(Alternative alternative)
  {
    piterator result = this->m_ref.begin(), first = this->m_ref.begin(),
    last = this->m_ref.end();

    while (++first != last)
    {
      if (* * result < * * first) * ++result = * first;
      else if (alternative(* * result, * * first)) * result = * first;
    }
    this->m_ref.erase(++result, this->m_ref.end());
  }


  // unique Bill_Index
  void unique()
  {
    piterator result = this->m_ref.begin(), first = this->m_ref.begin(),
    last = this->m_ref.end();

    while (++first != last)
    {
      if (* * result < * * first) * ++result = * first;
    }
    this->m_ref.erase(++result, this->m_ref.end());
  }

  static iterator nit()
  {
    return m_nit;
  }

  private:
  static iterator m_nit;

  void merge();

  COMP cmp;
  vector < PointerWrapper < T > > m_temp;
};

template < class T, class COMP >
typename Bill_Index< T, COMP >::iterator Bill_Index< T, COMP >::m_nit;

template < class T, class COMP >
void Bill_Index < T, COMP >::merge()
{
  piterator _begin = this->m_ref.begin();
  piterator _end = this->m_ref.end();

  sort(m_temp.begin(), m_temp.end(), cmp);
  this->m_ref.resize(this->m_ref.size() + m_temp.size());

  for (int i = m_temp.size() - 1; i >= 0; --i)
  {
    piterator pi = lower_bound(_begin, _end, m_temp[i], cmp);

    if (pi == _end)
    {
      * (_end + i) = m_temp[i];
    }
    else if (pi != _begin)
    {
      copy(pi, _end, pi + i + 1);
      _end = pi;
      * (_end + i) = m_temp[i];
    }
    else
    {
      copy(_begin, _end, _begin + i + 1);
      copy(m_temp.begin(), m_temp.begin() + i + 1, _begin);
      return;
    }
  }
}

#endif // Bill_Index_H_INCLUDED
