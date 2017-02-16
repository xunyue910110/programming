#ifndef CROSSRR_H_HEADER_INCLUDED_B7642005
#define CROSSRR_H_HEADER_INCLUDED_B7642005

#include <fstream>
#include <sys/time.h>
#include <set>
#include "CrossRr.h"
#include "dbi/dbi.h"
#include "base/PString.h"

const int SEGMENT_NUM = 24 * 31;

const string SELECTSQL = "SELECT MSISDN,A_SECTION_CODE,OTHER_PARTY,START_DATE||START_TIME,END_DATE||END_TIME,CALL_DURATION,TRUNK_GROUPOUT,TRUNK_GROUPIN FROM";
const string INSERTSQL = "INSERT INTO TG_CROSSRR_CDR(MSISDN,A_SECTION_CODE,OTHER_PARTY,START_TIME,END_TIME,CALL_DURATION,TRUNK_GROUPOUT,TRUNK_GROUPIN,SAME_FLAG,PARTITION_ID) VALUES(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10)";

class DBINFO
{
  public :
    string m_userName;
    string m_passWord;
    string m_dbServer;
    void set(const char *userName, const char *passWord, const char *dbServer)
    {
        m_userName = userName;
        m_passWord = passWord;
        m_dbServer = dbServer;
    }
    DBINFO& operator = (const DBINFO &right)
    {
        m_userName = right.m_userName;
        m_passWord = right.m_passWord;
        m_dbServer = right.m_dbServer;
        return *this;
    }
    friend ostream& operator << (ostream& os, const DBINFO &right)
    {
		os<< " UserName = " << right.m_userName << " , " << " PassWord = " << right.m_passWord << " , " << " DbServer = " << right.m_dbServer;
		return os;
    }
};

class CDR
{
  public :
    CDR(){};
    PString m_key;
    PString m_sectionCode;
    PString m_otherParty;
    long    m_beginTime;
    long    m_endTime;
    long    m_callDuration;
    PString m_trunkGroupIn;
    PString m_trunkGroupOut;
    char    m_sameFlag;
    friend int operator == (const CDR &left, const CDR& right);
    friend int operator < (const CDR &left, const CDR& right);
    friend ostream& operator << (ostream& os,const CDR& po);
};

class CrossRr
{
  public :
    CrossRr();
    ~CrossRr();
    bool initialization(const DBINFO &paramDbInfo, const DBINFO &srcDbInfo, const DBINFO &desDbInfo, const string &whereClause);
    int crossRrCheck();
    int crossRrIndb();
    bool setCheckInfo(const int &billCycle, const int &beginPartition, const int &endPartition);
  private :
    bool connect();
    int getData();
    int getFilterUserList();
  private :
    DbInterface m_paramDbInterface;
    DbInterface m_srcDbInterface;
    DbInterface m_desDbInterface;
    
    DBINFO m_paramDbInfo;
    DBINFO m_srcDbInfo;
    DBINFO m_desDbInfo;
    //源数据的过滤条件
    string m_whereClause;
    
    bool m_connected;
    set<PString> m_filterUserList;
    //按小时保存提高查找效率
    vector<CDR> m_data[SEGMENT_NUM];
    //保存本次检查日期前一日的最后两个小时内跨天的数据即beginDate! = endDate
    vector<CDR> m_lastData[2];
    //交叉查重结果
    vector<CDR *> m_result;
    //本次检话单账期
    int m_billCycle;
    //本次检查话单开始分区
    int m_beginPartition;
    //本次检查话单结束分区
    int m_endPartition;
};

#endif
