// Version              : 0.5.0
// Create               : 2001.8.15
// Update               : 2001.9.7
// Authors:     ZhongLiang Lu <luzl@lianchuang.com>
// Copyright (C) 2001 by Linkage Corporation. All Rights Reserved.
// Comments:
// History:


#ifndef _POINTERWRAPPER_H_
#define _POINTERWRAPPER_H_

#include <cassert>

#ifndef __NULL__
#define __NULL__
#define NULL 0
#endif

// wrap the pointer, with operator< 
template < class __Val >
class PointerWrapper
{
 public:
  __Val * Item;

  PointerWrapper(__Val * AItem = NULL)
    {
      Item = AItem;
    }

  PointerWrapper(const PointerWrapper < __Val > & x)
    {
      Item = x.Item;
    }

  bool operator < (const PointerWrapper < __Val > & x) const
    {
      assert(Item != NULL && x.Item != NULL);
      return * Item < * (x.Item);
    }

  bool operator == (const PointerWrapper < __Val > & x) const
    {
      assert(Item != NULL && x.Item != NULL);
      return !(*Item < *(x.Item) || *(x.Item) < *Item) ;
    }

  bool operator != (const PointerWrapper < __Val > & x) const
    {
      return !(*this == x );
    }

  __Val * operator->()
    {
      assert(Item != NULL);
      return Item;
    }

  const __Val * operator->() const
    {
      assert(Item != NULL);
      return Item;
    }

  __Val & operator * ()
    {
      assert(Item != NULL);
      return * Item;
    }

  operator const __Val & () const
    {
      return * Item;
    }

  operator __Val & ()
    {
      return * Item;
    }

  const __Val & operator * () const
    {
      assert(Item != NULL);
      return * Item;
    }
};

#endif //_POINTERWRAPPER_H_

