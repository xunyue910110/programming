// Version  : 0.5.0
// Create   : 2001.8.15
// Update   : 2001.9.7
// Authors:  ZhongLiang Lu <luzl@lianchuang.com>
// Copyright (C) 2001 by Linkage Corporation. All Rights Reserved.
// Comments:
// History:


#ifndef MatrixVector_H_INCLUDED
#define MatrixVector_H_INCLUDED

#include "base/config-all.h"

#include <vector>

USING_NAMESPACE(std)


// define matrix vector, it's interfaces same as vector.
// when the vector need continue mem segment, the matrix vector
// using many mem segments, using segment+offset locate element.
// the matrix vector which prevent realloc all mem   
//
//      vector<vector<T> *>
// ___________________________
// |_________________________|  <-----vector<T>
// |_________________________|
// |_________________________|
// |_________________________|
// |_________________________|

template < class T, size_t BUFFER_SIZE >
class MatrixVector
{
  // define row iterator
  typedef typename vector < vector < T > * >::iterator rowiterator;

 public:

  // read-only Random Access Iterators
  class const_iterator
    {
      friend class MatrixVector < T, BUFFER_SIZE >;

    public:

      const_iterator()
        {
        }

      const_iterator(const const_iterator & i)
        {
          * this = i;
        }

      const T * operator->() const
        {
          return & (*m_handle->m_data[m_row])[m_offset];
        }

      const T & operator * () const
        {
          return (*m_handle->m_data[m_row])[m_offset];
        }

      const_iterator & operator = (const const_iterator & i)
        {
          m_row = i.m_row;
          m_offset = i.m_offset;
          m_handle = i.m_handle;
          return * this;
        }

      const_iterator & operator ++() //TODO
        {
          if (++m_offset >= BUFFER_SIZE)
          {
            ++m_row;
            m_offset = 0;
          }
          return * this;
        }

      const_iterator operator ++(int)
        {
          const_iterator tmp(* this);

          if (++m_offset >= BUFFER_SIZE)
          {
            ++m_row;
            m_offset = 0;
          }

          return tmp;
        }

      const_iterator & operator --()
        {
          if (m_offset-- == 0)
          {
            --m_row;
            m_offset = BUFFER_SIZE - 1;
          }
          return * this;
        }

      const_iterator operator --(int)
        {
          const_iterator tmp(* this);

          if (m_offset-- == 0)
          {
            --m_row;
            m_offset = BUFFER_SIZE - 1;
          }

          return tmp;
        }

      const_iterator & operator += (size_t i)
        {
          m_offset += i;
          m_row += m_offset / BUFFER_SIZE;
          m_offset %= BUFFER_SIZE;

          return * this;
        }

      const_iterator & operator -= (size_t i)
        {
          m_offset = m_row * BUFFER_SIZE - i;
          m_row = m_offset / BUFFER_SIZE;
          m_offset %= BUFFER_SIZE;

          return * this;
        }

      const_iterator operator + (size_t i) const
        {
          const_iterator tmp(* this);
          return tmp += i;
        }

      const_iterator operator - (size_t i) const
        {
          const_iterator tmp = * this;
          return tmp -= i;
        }

      size_t operator - (const const_iterator & x) const
        {
          return (m_row - x.m_row) * BUFFER_SIZE + m_offset - x.m_offset;
        }

      const T & operator[] (size_t i) const
        {
          return * (* this + i);
        }

      bool operator == (const const_iterator & x) const
        {
          return m_row == x.m_row && m_offset == x.m_offset
            && m_handle == x.m_handle;
        }

      bool operator < (const const_iterator & x) const
        {
          return (m_row == x.m_row) ?(m_offset < x.m_offset):(m_row < x.m_row);
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
      const_iterator(MatrixVector *handle, size_t row, size_t offset)
        : m_row(row), m_handle(handle), m_offset(offset)
        {
        }

      MatrixVector* m_handle;
    
      size_t m_row;

      size_t m_offset;
    };


  // read-write Random Access Iterators
  class iterator : public const_iterator
    {
      friend class MatrixVector < T, BUFFER_SIZE >;

    public:

      iterator()
        {
        }

      iterator(const iterator & i)
        {
          * this = i;
        }

      T * operator->()
        {
          return & (*m_handle->m_data[m_row])[m_offset];
        }

      T & operator * ()
        {
          return (*m_handle->m_data[m_row])[m_offset];
        }

      iterator & operator = (const iterator & i)
        {
          m_row = i.m_row;
          m_offset = i.m_offset;
          m_handle = i.m_handle;
          return * this;
        }

      iterator & operator ++() //TODO
        {
          if (++m_offset >= BUFFER_SIZE)
          {
            ++m_row;
            m_offset = 0;
          }
          return * this;
        }

      iterator operator ++(int)
        {
          iterator tmp(* this);

          if (++m_offset >= BUFFER_SIZE)
          {
            ++m_row;
            m_offset = 0;
          }

          return tmp;
        }

      iterator & operator --()
        {
          if (m_offset-- == 0)
          {
            --m_row;
            m_offset = BUFFER_SIZE - 1;
          }
          return * this;
        }

      iterator operator --(int)
        {
          iterator tmp(* this);

          if (m_offset-- == 0)
          {
            --m_row;
            m_offset = BUFFER_SIZE - 1;
          }

          return tmp;
        }

      iterator & operator += (size_t i)
        {
          m_offset += i;
          m_row += m_offset / BUFFER_SIZE;
          m_offset %= BUFFER_SIZE;

          return * this;
        }

      iterator & operator -= (size_t i)
        {
          m_offset = m_row * BUFFER_SIZE - i;
          m_row = m_offset / BUFFER_SIZE;
          m_offset %= BUFFER_SIZE;

          return * this;
        }

      iterator operator + (size_t i) const
        {
          iterator tmp(* this);
          return tmp += i;
        }

      iterator operator - (size_t i) const
        {
          iterator tmp = * this;
          return tmp -= i;
        }

      size_t operator - (const iterator & x) const
        {
          return (m_row - x.m_row) * BUFFER_SIZE + m_offset - x.m_offset;
        }

      T & operator[] (size_t i)
        {
          return * (* this + i);
        }

      bool operator == (const iterator & x) const
        {
          return m_row == x.m_row && m_offset == x.m_offset
            && m_handle == x.m_handle;
        }

      bool operator < (const iterator & x) const
        {
          return (m_row == x.m_row) ?(m_offset < x.m_offset):(m_row < x.m_row);
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

      iterator(MatrixVector * handle, size_t row, size_t offset) :
        const_iterator(handle, row, offset)
        {
        }
    };

  friend class iterator;
  friend class const_iterator;
  

  MatrixVector() : m_first(this, 0, 0), m_last(this, 0, 0)
    {
    }

  ~MatrixVector()
    {
      for (size_t i = 0; i < m_data.size(); ++i)
      {
        delete m_data[i];
      }
    }
 
  iterator begin()
    {
      return m_first; //iterator(m_data.begin(), 0);
    }

  iterator end()
    {
      return m_last;
    }

  const_iterator begin() const
    {
      return m_first;
    }

  const_iterator end() const
    {
      return m_last;
    }

  T & operator[] (size_t index)
    {
      return (* m_data[index / BUFFER_SIZE]) [index % BUFFER_SIZE];
    }

  void push_back(const T & value)
    {
      if (m_last.m_row == m_data.size())
        growRow();

      m_data[m_last.m_row]->push_back(value);

      ++m_last.m_offset;

      if (m_last.m_offset >= BUFFER_SIZE)
      {
        ++m_last.m_row;
        m_last.m_offset = 0;
      }
    }

  template < class InputIterator >
    void push_back(InputIterator first, InputIterator last)
    {
      for ( ; first != last; ++first)
      {
        if (m_last.m_row == m_data.size())
          growRow();

        m_data[m_last.m_row]->push_back(* first);

        ++m_last.m_offset;

        if (m_last.m_offset >= BUFFER_SIZE)
        {
          m_last.m_offset = 0;
          ++m_last.m_row;
        }
      }
    }

  size_t size() const
    {
      return (m_last.m_row - m_first.m_row) * BUFFER_SIZE + m_last.m_offset;
    }

  void resize(size_t newSize)
    {
      size_t oldSize = size();

      if (capacity() < newSize)
        reserve(newSize);

      size_t grow;

      for ( ; newSize > oldSize; newSize -= grow)
      {
        grow = BUFFER_SIZE - m_last.m_offset;
        if (grow > newSize - oldSize)
          grow = newSize - oldSize;

        m_last.m_offset += grow;
        m_data[m_last.m_row]->resize(m_last.m_offset);

        if (m_last.m_offset >= BUFFER_SIZE)
          ++m_last.m_row, m_last.m_offset = 0;
      }

      for ( ; newSize < oldSize; newSize += grow)
      {
        if (m_last.m_offset == 0)
          --m_last.m_row, m_last.m_offset = BUFFER_SIZE;

        grow = m_last.m_offset;
        if (grow > oldSize - newSize)
          grow = oldSize - newSize;

        m_last.m_offset -= grow;
        m_data[m_last.m_row]->resize(m_last.m_offset);
      }
    }

  size_t capacity() const
    {
      return m_data.size() * BUFFER_SIZE;
    }

  void reserve(size_t n)
    {
      growRow(n / BUFFER_SIZE + 1 - m_data.size());
    }

 private:

  void growRow(int n = 1)
    {
      if (n > 0)
      {
        int oldSize = m_data.size();
        m_data.resize(oldSize + n);
        for (int i = oldSize + n - 1; i >= oldSize; --i)
        {
          m_data[i] = new vector < T > ();
          m_data[i]->reserve(BUFFER_SIZE);
        }
      }
    }

  vector < vector < T > * > m_data;

  iterator m_first;
  iterator m_last;
};

#endif // MatrixVector_H_INCLUDED
