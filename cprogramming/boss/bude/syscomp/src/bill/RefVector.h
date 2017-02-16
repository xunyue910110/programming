// Version              : 0.5.0
// Create               : 2001.8.15
// Update               : 2001.9.7
// Authors:     ZhongLiang Lu <luzl@lianchuang.com>
// Copyright (C) 2001 by Linkage Corporation. All Rights Reserved.
// Comments:
// History:


#ifndef VECTORREF_H_INCLUDED
#define VECTORREF_H_INCLUDED

#include "base/config-all.h"

#include "bill/PointerWrapper.h"

#include <vector>
#include <algorithm>
#include <functional>


USING_NAMESPACE(std)


// define object reference vector, RefVector only for reference object,
//  not consider object lifetime.
//
//     ______          
//    | T *  |         
//    |______| ---->   
//    | T *  |         
//    |______| ---->   
//    | T *  |         
//    |______| ---->   
//    | T *  |         
//    |______| ---->   
//    | T *  |         
//    |______| ---->   
//
template < class T >
class RefVector
{
 public:
  typedef typename vector < PointerWrapper < T > >::iterator piterator;

 public:

  class const_iterator
    {
    public:

      const T * operator->() const
        {
          return (* m_data).operator->();
        }

      const T & operator * () const
        {
          return (* m_data).operator * ();
        }

      const_iterator & operator = (const const_iterator & i)
        {
          m_data = i.m_data;
          return * this;
        }

      const_iterator & operator ++()
        {
          m_data++;
          return * this;
        }

      const_iterator operator ++(int)
        {
          const_iterator temp(* this);
          m_data++;
          return temp;
        }

      const_iterator()
        {
        }

      const_iterator(const const_iterator & i)
        {
          m_data = i.m_data;
        }

      const_iterator(const piterator & i)
        {
          m_data = i;
        }

      operator piterator() const
        {
          return m_data;
        }

      const_iterator & operator --()
        {
          m_data--;
          return * this;
        }

      const_iterator operator --(int)
        {
          const_iterator temp(* this);
          m_data--;
          return temp;
        }

      const_iterator & operator += (size_t i)
        {
          m_data += i;
          return * this;
        }

      const_iterator & operator -= (size_t i)
        {
          m_data -= i;
          return * this;
        }

      const_iterator operator + (size_t i) const
        {
          const_iterator tmp = * this;
          return tmp += i;
        }

      const_iterator operator - (size_t i) const
        {
          const_iterator tmp = * this;
          return tmp -= i;
        }

      size_t operator - (const_iterator x) const
        {
          return m_data - x.m_data;
        }

      const T & operator[] (size_t i) const
        {
          return * (* this + i);
        }

      bool operator == (const const_iterator & x) const
        {
          return m_data == x.m_data;
        }

      bool operator < (const const_iterator & x) const
        {
          return m_data < x.m_data;
        }

      bool operator != (const const_iterator & x) const
        {
          return !(* this == x);
        }

      bool operator > (const const_iterator & x) const
        {
          return x < * this;
        }

      bool operator >= (const const_iterator & x) const
        {
          return !(* this < x);
        }

      bool operator <= (const const_iterator & x) const
        {
          return !(* this > x);
        }

    protected:
      piterator m_data;
    };


  // wrap the iterator,
  // if using vector<T*>::iterator directly, need as (*iterator)->,
  // so, wrap it with "iterator->" interface
  class iterator : public const_iterator
    {
    public:

      T * operator->()
        {
          return (* m_data).operator->();
        }

      T & operator * ()
        {
          return (* m_data).operator * ();
        }

      iterator & operator = (const iterator & i)
        {
          m_data = i.m_data;
          return * this;
        }

      iterator & operator ++()
        {
          m_data++;
          return * this;
        }

      iterator operator ++(int)
        {
          iterator temp(* this);
          m_data++;
          return temp;
        }

      iterator()
        {
        }

      iterator(const iterator & i)
        {
          m_data = i.m_data;
        }

      iterator(const piterator & i)
        {
          m_data = i;
        }

      iterator & operator --()
        {
          m_data--;
          return * this;
        }

      iterator operator --(int)
        {
          iterator temp(* this);
          m_data--;
          return temp;
        }

      iterator & operator += (size_t i)
        {
          m_data += i;
          return * this;
        }

      iterator & operator -= (size_t i)
        {
          m_data -= i;
          return * this;
        }

      iterator operator + (size_t i) const
        {
          iterator tmp = * this;
          return tmp += i;
        }

      iterator operator - (size_t i) const
        {
          iterator tmp = * this;
          return tmp -= i;
        }

      size_t operator - (iterator x) const
        {
          return m_data - x.m_data;
        }

      T & operator[] (size_t i)
        {
          return * (* this + i);
        }

      bool operator == (const iterator & x) const
        {
          return m_data == x.m_data;
        }

      bool operator < (const iterator & x) const
        {
          return m_data < x.m_data;
        }

      bool operator != (const iterator & x) const
        {
          return !(* this == x);
        }

      bool operator > (const iterator & x) const
        {
          return x < * this;
        }

      bool operator >= (const iterator & x) const
        {
          return !(* this < x);
        }

      bool operator <= (const iterator & x) const
        {
          return !(* this > x);
        }

    private:
      piterator m_data;
    };

  RefVector()
    {
    }

  // constructor, init by data specialed by first and last
  // example:
  // int a[10] = {...};
  // index<const int *, int> idx1(a, a+10);
  // other example:
  // vector<int> b(a, a+10);
  // RefVector<vector<int>::iterator, int> idx2(b.begin(), b.end());
  template < class InputIterator >
    RefVector(InputIterator first, InputIterator last)
    {
      push_back(first, last);
    }

  template < class InputIterator >
    RefVector(InputIterator first, size_t size)
    {
      push_back(first, size);
    }

  void push_back(T & value)
    {
      m_ref.push_back(PointerWrapper < T > (& value));
    }

  template < class InputIterator >
    void push_back(InputIterator first, InputIterator last)
    {
      for ( ; first != last; ++first)
      {
        PointerWrapper < T > pw(& (* first));
        m_ref.push_back(pw);
      }
    }

  template < class InputIterator >
    void push_back(InputIterator first, size_t size)
    {
      m_ref.reserve(m_ref.size() + size);

      for (size_t i = 0; i < size; i++)
      {
        PointerWrapper < T > pw(& (* first++));
        m_ref.push_back(pw);
      }
    }

  iterator begin()
    {
      return m_ref.begin();
    }

  const_iterator begin() const
    {
      return m_ref.begin();
    }

  iterator end()
    {
      return m_ref.end();
    }

  const_iterator end() const
    {
      return m_ref.end();
    }

  T & operator[] (size_t index)
    {
      return * m_ref[index];
    }

  const T & operator[] (size_t index) const
    {
      return * m_ref[index];
    }

  size_t size() const
    {
      return m_ref.size();
    }

  void resize(size_t newSize)
    {
      m_ref.resize(newSize);
    }

  void clear()
    {
      m_ref.clear();
    }

  void reserve(size_t n)
    {
      m_ref.reserve(n);
    }

  size_t capacity()
    {
      return m_ref.capacity();
    }

 protected:
  vector < PointerWrapper < T > > m_ref;
};

#endif // INDEX_H_INCLUDED
