
/***************************************************************** lm-cpr beg
  *
  * AI Loom - A cross platform artificial intelligence framework
  * AI Loom is (c) Copyright 2003 Josh Brown
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public
  * License along with this library; if not, write to the
  * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  * Boston, MA 02111-1307, USA.
  *
  * -----------------------------------------------------------------
  * File:          $RCSfile: UUID.h,v $
  * Date modified: $Date: 2011/04/15 07:35:52 $
  * Version:       $Revision: 1.1 $
  * -----------------------------------------------------------------
  *
  ************************************************************** lm-cpr-end */
#ifndef LM_UUID_H
#define LM_UUID_H
 
#include <iostream>
#include <string>
/**
   UUID uid = UUID::generate();
   cout << uid << endl;
   string s1 = uid.ascii(); // 字母和数字表示, 24位长
   string s2 = uid.str();   //16进制表示, 36位长
 */
class UUID { 
       static const UUID NullUUID;
       static const char* ch;

   public:

   // typedefs for a bunch of the major sizes in c++
   #ifndef _MSC_VER // assume i386
      typedef   signed char      i8;
      typedef unsigned int       u8;
      typedef          short     i16;
      typedef unsigned short     u16;
      typedef          int       i32;
      typedef unsigned int       u32;
      typedef          long long i64;
      typedef unsigned long long u64;
   #else // we're in windoze
      typedef          __int8  i8;
      typedef unsigned __int8  u8;
      typedef          __int16 i16;
      typedef unsigned __int16 u16;
      typedef          __int32 i32;
      typedef unsigned __int32 u32;
      typedef          __int64 i64;
      typedef unsigned __int64 u64;
   #endif

      
       UUID();
 
       UUID(const std::string& uuid);
 
       UUID(const UUID& uuid);
 
       UUID& operator=(const UUID& uuid);
 
       static UUID generate();
 
    public:
       bool isNull() const;
 
       std::string str() const;
       //添加方法
       std::string ascii() const;
       void testascii();
       
    public:
       bool operator==(const UUID& uuid) const;
 
       bool operator!=(const UUID& uuid) const;
 
       bool operator<(const UUID& uuid) const;
 
    private:
       void makeFromString(const std::string& uuid);
       unsigned long getUIP();
       long getPID();      
       long getCurrentTime();
       long getCount();
    private :
       union germ_uuid_t
       {
          struct standard_
          {
             u32 m0;
             u16 m1;
             u16 m2;
             u8  m3;
             u8  m4;
             u8  m5[6];
          } standard;
 
          struct packed_
          {
             u32 p0;
             u32 p1;
             u32 p2;
             u32 p3;
          } packed;
       
         struct ascii_
         {   
         	u64 a0;
            u64 a1;
         } ascii;
       };
 
       germ_uuid_t mUuid;
       
    };
 
    std::ostream& operator<<(std::ostream& out, const UUID& uuid);

 
 #endif

