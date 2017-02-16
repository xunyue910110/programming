// Version  : 0.5.0
// Create   : 2001.8.15
// Update   : 2001.9.7
// Authors:     ZhongLiang Lu <luzl@lianchuang.com>
// Copyright (C) 2001 by Linkage Corporation. All Rights Reserved.
// Comments:
// History:


#ifndef SMARTMEMORY_H_INCLUDED
#define SMARTMEMORY_H_INCLUDED

//=============================================================================

struct ThriftyListBlockHead
{
  ThriftyListBlockHead(size_t begin = 0, size_t last = 0,
                       size_t next = (size_t)-1)
      : m_begin(begin), m_last(last), m_next(next) { }
  
  // begin offset to DataVector
  size_t m_begin;

  // end offset to DataVector
  size_t m_last;

  // next Head offset to HeadVector
  size_t m_next;
};


// define single-list, forward access.
//
//  ThriftyList       HeadVector        DataVector
//    _____            _____             _____    
//   |     |          |     |           |     |   
//   |     |          |     |           |     |
//   |begin|--------->|begin|---------->|     |   
//   |_____|          |last |--         |     |   
//                 ---|next |  \        |     |   
//                 |  |     |   \       |     |   
//                 |  |     |    \      |     |   
//                 -->|     |     \     |     |   
//                    |_____|      \    |     |
//                                  --->|     |   
//                                      |     |
//                                      |     |   
//                                      |     |
//                                      |     |   
//                                      |     |
//                                      |_____|   
//                                                
// 
template < class T, class AssignPolicy, class DataVector, class HeadVector >
class ThriftyList
{
 public:

  //==========================================================================
  // memory management
  class SmartMemory
    {
    public:
      SmartMemory()
        {    
        }

      size_t assignNewBuffer(size_t step)
        {
          size_t result = m_headVector.size();
          size_t dataSize = m_dataVector.size();

          m_headVector.push_back(ThriftyListBlockHead(dataSize,
                                                      dataSize, (size_t)-1));
          m_dataVector.resize(dataSize + stepSize(step));

          return result;
        }

      size_t stepSize(size_t step)
        {
          return m_assignPolicy(step);
        }

      DataVector m_dataVector;

      HeadVector m_headVector;

      AssignPolicy m_assignPolicy;
    };

  //==========================================================================
  // read-only forward iterator
  class const_iterator
    {
      friend class ThriftyList < T, AssignPolicy, DataVector, HeadVector >;

    public:
      const T * operator->() const
        {
          return & ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.m_dataVector[m_pointer];
        }

      const T & operator * () const
        {
          return ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.m_dataVector[m_pointer];
        }

      const_iterator& operator ++()
        {
          if (m_pointer != (size_t)-1)
          {
            ++m_pointer;
            if (m_pointer >= head().m_begin + m_blockSize)
            {
              if (head().m_next != (size_t)-1)
              {
                incStep();
                m_headIndex = head().m_next;
                m_pointer = head().m_begin;
              }
            }
          }
          return * this;
        }

      const_iterator operator ++(int)
        {
          const_iterator temp(*this);
          operator ++ ();
          return temp;
        }

      const_iterator()
        {
        }

      const_iterator(const const_iterator& x)
        {
          *this = x;
        }

      const const_iterator& operator = (const const_iterator& x)
        {
          m_blockSize = x.m_blockSize;
          m_headIndex = x.m_headIndex;
          m_pointer   = x.m_pointer;
          m_step      = x.m_step;

          return *this;
        }

      bool operator < (const const_iterator & x) const
        {
          if (m_step != x.m_step)
            return m_step < x.m_step;

          return m_pointer < x.m_pointer;
        }

      bool operator == (const const_iterator & x) const
        {
          if (m_step != x.m_step)
            return false;

          return m_pointer == x.m_pointer;
        }

      bool operator != (const const_iterator & x) const
        {
          return !operator == (x);
        }

      void incStep()
        {
          ++m_step;
          m_blockSize = ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.stepSize(m_step);
        }

      void decStep()
        {
          --m_step;
          m_blockSize = ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.stepSize(m_step);
        }

    protected:

      ThriftyListBlockHead& head()
        {
          return ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.m_headVector[m_headIndex];
        }

      const_iterator(size_t step, size_t headIndex, size_t pointer)
        : m_step(step), m_headIndex(headIndex), m_pointer(pointer),
        m_blockSize(ThriftyList < T, AssignPolicy, DataVector,
                    HeadVector >::m_memory.stepSize(m_step))
        {
        }

      size_t m_step;

      size_t m_blockSize;
    
      size_t m_headIndex;    

      size_t m_pointer;
    };

  // read-write forward iterator
  class iterator : public const_iterator
    {
      friend class ThriftyList < T, AssignPolicy, DataVector, HeadVector >;

    public:
      T * operator->() const
        {
          return & ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.m_dataVector[m_pointer];
        }

      T & operator * () const
        {
          return ThriftyList < T, AssignPolicy, DataVector,
            HeadVector >::m_memory.m_dataVector[m_pointer];
        }

      iterator& operator ++()
        {
          if (m_pointer != (size_t)-1)
          {
            ++m_pointer;
            if (m_pointer >= head().m_begin + m_blockSize)
            {
              if (head().m_next != (size_t)-1)
              {
                incStep();
                m_headIndex = head().m_next;
                m_pointer = head().m_begin;
              }
            }
          }
          return * this;
        }

      iterator operator ++(int)
        {
          iterator temp(*this);
          operator ++ ();
          return temp;
        }

      iterator()
        {
        }

      iterator(const iterator& x)
        {
          *this = x;
        }

      const iterator& operator = (const iterator& x)
        {
          m_blockSize = x.m_blockSize;
          m_headIndex = x.m_headIndex;
          m_pointer   = x.m_pointer;
          m_step      = x.m_step;

          return *this;
        }

      bool operator < (const iterator & x) const
        {
          if (m_step != x.m_step)
            return m_step < x.m_step;

          return m_pointer < x.m_pointer;
        }

      bool operator == (const iterator & x) const
        {
          if (m_step != x.m_step)
            return false;

          return m_pointer == x.m_pointer;
        }

      bool operator != (const iterator & x) const
        {
          return !operator == (x);
        }    

    private:
      iterator(size_t step, size_t headIndex, size_t pointer) :
        const_iterator(step, headIndex, pointer)
        {
        }
    };

  //==========================================================================
  ThriftyList() : m_beginHead((size_t)-1)
    {
    }

  void push_back(const T & value)
    {
      if (m_beginHead == (size_t)-1)
        m_beginHead = m_memory.assignNewBuffer(0);

      iterator e = end();

      if (e.head().m_last >= e.head().m_begin + e.m_blockSize)
      {
        e.head().m_next = m_memory.assignNewBuffer(e.m_step + 1);
        ++e;
      }

      m_memory.m_dataVector[e.head().m_last] = value;
      ++e.head().m_last;
    }

  iterator begin()
    {
      return getBegin();
    }

  const_iterator begin() const
    {
      return getBegin();
    }

  iterator end()
    {
      return getEnd();
    }

  const_iterator end() const
    {
      return getEnd();
    }

  void clear()
    {
      m_beginHead = (size_t)-1;
    }

  static void reset();  
  static void reserve(size_t nHead, size_t nData);  

 private:

  iterator getBegin() const
    {
      if (m_beginHead == (size_t) - 1)
        return iterator(0, (size_t)-1, (size_t)-1);
      else
        return iterator(0, m_beginHead,
                        m_memory.m_headVector[m_beginHead].m_begin);
    }

  iterator getEnd() const
    {
      if (m_beginHead == (size_t) - 1)
        return iterator(0, (size_t)-1, (size_t)-1);

      size_t step;
      size_t p;
      size_t headIndex = (size_t)-1;

      for (step = (size_t)-1, p = m_beginHead;
           p != (size_t)-1;
           p = m_memory.m_headVector[p].m_next, ++step)
        headIndex = p;

      return iterator(step, headIndex,
                      m_memory.m_headVector[headIndex].m_last);
    }

  // begin block head
  size_t m_beginHead;

  // static memory management
  static SmartMemory m_memory;

  friend class const_iterator;
  friend class iterator;
};


template < class T, class AssignPolicy, class DataVector, class HeadVector >
typename ThriftyList < T, AssignPolicy, DataVector,
  HeadVector >::SmartMemory ThriftyList < T, AssignPolicy, DataVector,
  HeadVector >::m_memory;


template < class T, class AssignPolicy, class DataVector, class HeadVector >
void ThriftyList < T, AssignPolicy, DataVector, HeadVector >::reset()
{
  m_memory.m_headVector.resize(0);
  m_memory.m_dataVector.resize(0);
}

template < class T, class AssignPolicy, class DataVector, class HeadVector >  
void ThriftyList < T, AssignPolicy, DataVector, HeadVector >::
reserve(size_t nHead, size_t nData)
{
  m_memory.m_headVector.reserve(nHead);
  m_memory.m_dataVector.reserve(nData);
}

#endif // SMARTMEMORY_H_INCLUDED
