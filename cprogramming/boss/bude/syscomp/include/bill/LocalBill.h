// Language:    C++
// OS Platform: UNIX
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//    SERVID = 0 is reserved as a invalid SERVID
//

#ifndef LOCALBILL_H_INCLUDED
#define LOCALBILL_H_INCLUDED

#include "base/config-all.h"

#include "bill/MatrixVector.h"
#include "bill/ThriftyList.h"
#include "bill/Index.h"

#include "base/Types.h"

#include <ctime>
#include <cstdio>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <utility>

#include <algorithm>
#include <iostream>
#include <fstream>

#ifndef TIME_TRACE
#define TIME_TRACE
#endif //TIME_TRACE
#undef TIME_TRACE

#if defined(TIME_TRACE)
void TRACETIME(const char * msg);
#else
#define TRACETIME(x)
#endif

USING_NAMESPACE(std)


//--------------------------------------------------------------------------//
const int BILL_OK      = 0;                   //  0 - OK
const int BILL_REVOK   = 1;                   //  1 - recovery ok
const int BILL_LOGERR  = -1;                  // -1 - no bill log file
const int BILL_FILEERR = -2;                  // -2 - bill file error

//--------------------------------------------------------------------------//
#define SERVER_PER_VECTSIZE        20000    //     1X 12 Bytes
#define HEADER_PER_VECTSIZE        20000    //   2-3X 12 Bytes
#define CHARGEITEM_PER_VECTSIZE    50000    // 10-15X  8 Bytes

//Modified By Sunam 2007-1-30 10:25 change 2000 to 3000
//#define MAX_ITEMID                 2000             // max bill fee item
#define MAX_ITEMID                 3000             // max bill fee item
#define MIN_ITEMID                 400              // min bill fee item
#define MAX_CHARGEITEM             (MAX_ITEMID + 1) // max bill item


const int DATETIMELEN = 12;                 //YYMMDDHHMMSS
const int DAY_LEN = 6;                      //YYMMDD
const int BILLSEQUENCE_LEN = 8;

//--------------------------------------------------------------------------//
// define AssignPolicy for once CHARGEITEM data extend
struct MyAssignPolicy
{
  size_t operator() (unsigned int step)
    {
      switch (step)
      {
      case 0:
        return 5;
      case 1:
        return 5;
      case 2:
        return 5;
      default:
        return 5;
      }
    }
};

//--------------------------------------------------------------------------//
// define SERVID
typedef Number SERVID;


//--------------------------------------------------------------------------//
// define CHARGEITEM
struct CHARGEITEM
{
  static int count;       // test only
  int   type;
  Integer value;

  CHARGEITEM() {;}

  CHARGEITEM(const CHARGEITEM& rhs)
    {
      count++;
      type = rhs.type;
      value = rhs.value;
    }

  CHARGEITEM& operator=(const CHARGEITEM& rhs)
    {
      if (this != &rhs)
      {
        type  = rhs.type;
        value = rhs.value;
      }
      return *this;
    }
  friend bool operator==(const CHARGEITEM& lhs, const CHARGEITEM& rhs)
    {return ((lhs.type == rhs.type) || (lhs.value == rhs.value)); }
  friend bool operator<(const CHARGEITEM& lhs, const CHARGEITEM& rhs)
    {return (lhs.type < rhs.type);}

  friend ostream& operator << (ostream& out, const CHARGEITEM& charge);
  friend istream& operator >> (istream& in, CHARGEITEM& charge);
};

//--------------------------------------------------------------------------//
// define Bill
typedef ThriftyList <CHARGEITEM, MyAssignPolicy,
  MatrixVector <CHARGEITEM, CHARGEITEM_PER_VECTSIZE>,
  MatrixVector <ThriftyListBlockHead, HEADER_PER_VECTSIZE> > Bill;

typedef ThriftyList < CHARGEITEM, MyAssignPolicy,
  MatrixVector <CHARGEITEM, CHARGEITEM_PER_VECTSIZE>,
  MatrixVector <ThriftyListBlockHead, HEADER_PER_VECTSIZE> >::iterator BillItr;


//--------------------------------------------------------------------------//
// define Serv
class Serv
{
 public:
  SERVID m_servId;
  Bill   m_bill;

  Serv()
    {
    }

  Serv(const Serv& x)
    {
      m_servId = x.m_servId;
      m_bill = x.m_bill;
      count++;
    }

  bool operator < (const Serv&serv) const
    {
      return m_servId < serv.m_servId;
    }
  static int count;     // test only
};


//--------------------------------------------------------------------------//
// define ServVector
typedef MatrixVector < Serv, SERVER_PER_VECTSIZE > ServVector;
typedef MatrixVector < Serv, SERVER_PER_VECTSIZE >::iterator ServVectorItr;

//--------------------------------------------------------------------------//
// define ServIndex
typedef Bill_Index<Serv> ServIndex;


class LocalBill
{
 public:
  LocalBill():INVALID_SERVID(0), m_isFirstOnce(true), m_sequenceNo(0) {;}
  ~LocalBill() {;}

  //--------------------------------------------------------------------------//
  // BILL_OK       0 - OK
  // BILL_REVOK    1 - recovery ok
  // BILL_LOGERR  -1 - no bill log file
  // BILL_FILEERR -2 - bill file error
  int  check(int cycle, int channel = 0);

  bool initialization(int cycle, int channel = 0);
  bool end();

  bool fileInit(const char * dealdate);   //dealdate: YYYYMMDDHHMMSS
  bool fileCommit(const char * filename);

  bool getBill(const SERVID& servid, Integer     *chargeValues);
  bool updateBill(const SERVID& servid, const int groupid,
                  const Integer     *chargeValues);
                  
  bool writeTotalFileBill(const char * filename);
  bool initTotalBillFromFile(const char * filename);
  bool initFromIncreaseFile(const char *filename);
  bool initTotalBillFromParamFile(const char * filename);

  bool decreseTotalBillFromFile(const char * filename);

  void setServReserve(size_t servsize) { m_servReserve = servsize; }
  void setHeadReserve(size_t headsize) { m_headReserve = headsize; }
  void setDataReserve(size_t datasize) { m_dataReserve = datasize; }

  void setBillLogFilePath(const char *filepath)
    { m_billLogFilePath = filepath; }
  void setTotalBillFilePath(const char *filepath)
    { m_totalBillFilePath = filepath; }
  void setIncreaseBillFilePath(const char *filepath)
    { m_increaseBillFilePath = filepath; }
  //Added by Sunam 2005-11-28 FOR DR
  void setDrBillFilePath(const char *filepath)
    { m_drBillFilePath = filepath; }
  //Added by Sunam For Ocs 2006-6-14 11:42
  void setOcsBillFilePath(const char *filepath)
  { m_ocsBillFilePath = filepath; }

  //添加新的接口
 
  int  checkex(int cycle, int channel = 0);
  
    //取用户全量帐单  vitemcode 有户有效的账单项
  bool getBill(const SERVID& servid, vector<int> &vitemcode, int * chargeVals);
    
    //更新用户帐单
    //vitemcode       需要更新的账单项
    //vchangeitemcode 值发生改变的账单项
  bool updateBill(const SERVID& servid, const vector<int> &vitemcode, int *chargeVals,
                    vector<int> &vchangeitemcode);
    
    //输出内存帐单全部SERVID到文件                  
  bool writeAllServFile(const char * filename);
  
  bool fileCommit();
  
  //Added by Sunam 2005-11-28 FOR DR
  int getSequenceNo(int &sequenceNo);
  
  //Added by Sunam For OCS 2006-6-14 11:28
  bool getBill(const SERVID& servid, Integer *chargeVals ,bool isOcsUser);
  bool updateBill(const SERVID& servid, const int groupid,
                           const Integer *chargeVals, bool isOcsUser);
  void initOcsFeeId(const vector<int> &ocsFeeId);
  //清内存中总量帐单，清之前先把总量帐单写文件！
  //added by zhanglong 2006-7-5 17:54
  bool clearTotalBill();

 private:
  const SERVID INVALID_SERVID;

  ServVector m_servVector;    // Total serv bill
  ServIndex  m_servIndex;     // Total serv bill index

  size_t m_servReserve;
  size_t m_headReserve;
  size_t m_dataReserve;

  bool m_isNewServAdd;        // need to rebuild total serv bill index
  string  m_currentCycle;
  string  m_channelno;

  // total bill file path and name
  string m_totalBillFilePath;
  string m_totalBillFileName;
  // increase bill file path and name
  string m_increaseBillFilePath;
  string m_incrBillFilename;        //INCREASE-BILL-FILE
  string m_incrBillFilenameBak;     //INCREASE-BILL-FILE backup
  //Added by Sunam 2005-11-28 FOR DR
  // dr bill file path and name
  string m_drBillFilePath;
  string m_drBillFileName;
  string m_drBillFileNameBak;
  
  //Added by Sunam For Ocs 2006-6-14 11:42
  string m_ocsBillFilePath;
  string m_ocsBillFileName;
  string m_ocsBillFileNameBak;
  string m_dayBillFileNameOcs;

  string m_dayBillFilename;         //DAY-BILL-FILE
  string m_dayBillFilenameBak;      //DAY-BILL-FILE backup
  //Added by Sunam 2005-11-28 FOR DR
  string m_dayBillFilenameDr;
    
  string m_billLogFilePath;
  string m_billLogfile;
  //Added by Sunam For OCS 2006-6-14 12:59
  string m_ocsBillLogfile;

  fstream m_fstreamIncrease;
  fstream m_fstreamTotal;

  fstream m_fstreamLog;
  //Added by Sunam For OCS 2006-6-14 12:59
  fstream m_ocsFstreamLog;

  // define BILLITEM
  struct BILLITEM
  {
    int                 groupid;
    ServIndex::iterator sit;
    CHARGEITEM          charge;

    BILLITEM() {;}

    BILLITEM(const BILLITEM& rhs)
      {
        groupid = rhs.groupid;
        sit = rhs.sit;
        charge = rhs.charge;
      }

    BILLITEM& operator=(const BILLITEM& rhs)
      {
        if (this != &rhs)
        {
          groupid  = rhs.groupid;
          sit = rhs.sit;
          charge = rhs.charge;
        }
        return *this;
      }
    //friend ostream& operator << (ostream& out,
    //                                   const BILLITEM& billitem);
  };

  typedef multimap<SERVID, BILLITEM> BILLDATAMAP;
  typedef BILLDATAMAP::iterator BILLDATAMIter;
  typedef BILLDATAMAP::const_iterator BILLDATAMCIter;
  BILLDATAMAP m_localdata;
  
  //Added by Sunam For OCS 2006-6-14 11:28
  BILLDATAMAP m_localdataOcs;
  vector<int> m_ocsFeeId;

 private:
  char m_day[DAY_LEN + 1];
  bool m_isFirstOnce;
  int  m_sequenceNo;
  bool updateTotalBill();
  bool writeIncreaseFileBill(const char * filename);
  char m_dealtime[DATETIMELEN + 1]; //YYMMDDHHMMSS
  
  //Added by Sunam For OCS 2006-6-14 11:28
  int m_ocsSequenceNo;
  bool writeOcsIncreaseFileBill(const char * filename);
  bool isOcsFeeId(int feeId);
  
};


/*
  YYYYMM    : bill cycle
  XXX       ：channel no.
  YYMMDD    : year-month-day
  SSSSSSSS  : sequence number

  BILL-LOG-FILE：      XXXBILL.LOG
  TOTAL-BILL-FILE：    CYYYYMMXXXTOTAL.BILL
  old TOTAL-BILL-FILE：CYYYYMMXXXTOTAL.BILL.OLD

1、INCREASE-BILL-FILE name： RYYYYMMXXXYYMMDDSSSSSSSS
2、DAY-BILL-FILE name     ： DYYYYMMXXXYYMMDD
3、MONTH-BILL-FILE name   ： MYYYYMMXXX
4、NEGATIVE-BILL-FILE name： ZYYYYMMXXXYYMMDDSSSSSSSS
*/


#endif // LOCALBILL_H_INCLUDED



