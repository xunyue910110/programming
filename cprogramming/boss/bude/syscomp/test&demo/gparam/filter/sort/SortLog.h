// ##########################################
// Source file : SortCdr.h
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020308
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTLOG_H_INCLUDED_C37B75A3
#define SORTLOG_H_INCLUDED_C37B75A3

#include "config-all.h"

#include <vector>
#include <string>

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

USING_NAMESPACE(std)

//##ModelId=3BE9DD2F033D
class SortLog
{
 public:
   //##ModelId=3BE9DF2F0357
   void clear();

   //##ModelId=3BE9DF360018
   SortLog();

   //##ModelId=3BE9DF450272
   ~SortLog();

   //##ModelId=3BE9E05F0250
   void showLog();

   //##ModelId=3BE9E0DC02D2
   bool isShowFlag();

   //##ModelId=3BE9E14201AC
   int getErrorSum() const;

   //##ModelId=3BE9E142021A
   void addErrorSum();

   //##ModelId=3BE9E142030A
   int getIrSum() const;

   //##ModelId=3BE9E1420383
   void addIrSum();

   //##ModelId=3BE9E1430095
   int getNoinfoSum() const;

   //##ModelId=3BE9E143010D
   void addNoinfoSum();

   //##ModelId=3BE9E1430207
   int getNrSum() const;

   //##ModelId=3BE9E1430280
   void addNrSum();

   //##ModelId=3BE9E144001E
   void setSrcFileName(string left);

   //##ModelId=3BE9E1440137
   int getTotalCdrs() const;

   //##ModelId=3BE9E14401B9
   void addTotalCdrs();

   //##ModelId=3BE9E14402D1
   int getTrashSum() const;

   //##ModelId=3BE9E1440353
   void addTrashSum();

   //##ModelId=3C1EF6FF02E7
   int getSpecialSum() const;

   //##ModelId=3C1EF7000053
   void addSpecialSum();

   //##ModelId=3C1EF7000162
   int getPocSum() const;

   //##ModelId=3C1EF70001A8
   void addPocSum();

   //##ModelId=3C1EF7000270
   int getPocNoinfoSum() const;

   //##ModelId=3C1EF7000307
   void addPocNoinfoSum();

   //##ModelId=3C84DCA202F6
   void addChannelSum(const int channel);

   //##ModelId=3C84DCAE02F3
   int getChannelSum(const int channel) const;

   //##ModelId=3C84E761019D
   void initialize(const int channel);

   //##ModelId=3C8579DD019F
   int getChannelCount();

   //##ModelId=3C85D0C5016A
   string getSrcFileName() const;

   //##ModelId=3C887254015B
   int getUpdataTimeLimen() const;

   //##ModelId=3C8872910181
   void setUpdataCDRsLimen(const int cdrsLimen);

   //##ModelId=3C8872D502AB
   void setUpdataTimeLimen(const int timeLimet);

   //##ModelId=3C8872F0006F
   int getUpdataCDRsLimen() const;

 private:
   //##ModelId=3BE9DD600026
   string m_SrcFileName;

   //##ModelId=3BE9DD6C0272
   int m_TotalCdrs;

   //##ModelId=3BE9DD7700AB
   int m_nrSum;

   //##ModelId=3BE9DDA3005E
   int m_irSum;

   //##ModelId=3BE9DDA9016B
   int m_noinfoSum;

   //##ModelId=3BE9DDBC00DD
   int m_trashSum;

   //##ModelId=3BE9DDC90334
   int m_errorSum;

   //##ModelId=3BE9E0A1031D
   bool m_isShow;

   //##ModelId=3C1EF68B03BC
   int m_pocSum;

   //##ModelId=3C1EF69C0226
   int m_pocNoinfoSum;

   //##ModelId=3C1EF6AF009D
   int m_specialSum;

   //##ModelId=3C84DB280374
   vector <int> m_channelSum;

   //##ModelId=3C8871810202
   int m_updataTimeLimen;

   //##ModelId=3C8871BB01E7
   int m_updataCDRsLimen;

};

#endif /* SORTLOG_H_INCLUDED_C37B75A3 */
