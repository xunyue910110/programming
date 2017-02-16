// ##########################################
// Source file : PpLog.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef PPLOG_H_HEADER_INCLUDED_B7A635D1
#define PPLOG_H_HEADER_INCLUDED_B7A635D1

#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include "FieldInfo.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
#include <map>
#include <vector>

USING_NAMESPACE(std);

class MyString;
class OutputFile;

//##ModelId=4855C61E0196
//##Documentation
//## 预处理系统日志类
class PpLog
{
  public:
    //##ModelId=4855C6C60167
    ~PpLog();

    //##ModelId=4855C6D8031C
    //inline static PpLog *getInstance();

    //##ModelId=4855C7D90186
    //##Documentation
    //## 日志属性初始化方法,即全部属性清零
    void clear();

    //##ModelId=4855C80F0242
    bool write();

    //##ModelId=4855C81F03D8
    void ShowMessage(
        //##Documentation
        //## 要显示的信息内容
        string theMessage,
        //##Documentation
        //## 写日志文件标志（=0:不写文件;=1:带时间前缀写文件;=2:不带时间前缀写文件）
        int ShowFlag);

    //##ModelId=4855C84B0196
    void setFileName(const string &filename);

    //##ModelId=4855C86601E4
    //##Documentation
    //## 用于日志信息的显示，可以输出到屏幕或者道日志文件
    //inline void setShowFlag(bool aShowFlag);

    //##ModelId=4855C88102AF
    //inline bool isShowFlag();

    //##ModelId=4855C8940167
    void setEndDateTime(string &left);

    //##ModelId=4855C8A80186
    void addErrorSum();

    //##ModelId=4855C8B90280
    void addForwSum();

    //##ModelId=4855CA360232
    void addMocSum();

    //##ModelId=4855CA3F0167
    void addMtcSum();

    //##ModelId=4855CA460290
    void addNoUseSum();

    //##ModelId=4855CA4F0213
    void addPocSum();

    //##ModelId=4855CAAF000F
    void addPtcSum();

    //##ModelId=4855CAB8004E
    void addRoamSum();

    //##ModelId=4855CAC20138
    void addSmocSum();

    //##ModelId=4855CACA0167
    void addSmtcSum();

    //##ModelId=4855CAD2004E
    void addOtherSum();

    //##ModelId=4855CADC02FD
    void addTotalCdrs();
    
    //统计总话费
    void addTotalFee(int CdrFee);              //add By lizhao 2008-10-07

    //##ModelId=4855CAE602FD
    void setSrcFileName(string &left);

    //##ModelId=4855CB22001F
    string getEndDateTime() const;

    //##ModelId=4855CBC803A9
    int getErrorSum() const;

    //##ModelId=4855CC02003E
    int getForwSum() const;

    //##ModelId=4855CC9103A9
    bool getisShow() const;

    //##ModelId=4855CCA20399
    int getMocSum() const;

    //##ModelId=4855CCB000FA
    int getMtcSum() const;

    //##ModelId=4855CCBF0157
    int getNoUseSum() const;

    //##ModelId=4855CCDE0261
    int getOtherSum() const;

    //##ModelId=4855CCEC0196
    int getPocSum() const;

    //##ModelId=4855CCF80399
    int getPtcSum() const;

    //##ModelId=4855CD0C0280
    int getRoamSum() const;

    //##ModelId=4855CD19031C
    int getSmocSum() const;

    //##ModelId=4855CD28008C
    int getSmtcSum() const;

    //##ModelId=4855CD370251
    string getSrcFileName() const;

    //##ModelId=4855CD53007D
    string getStartDateTime() const;

    //##ModelId=4855CD6903A9
    time_t getStartDateTime_tm() const;

    //##ModelId=4855CD7E001F
    int getTotalCdrs() const;
    
    int getTotalFee() const; // 获取税后总话费，add by lizhao 2008-10-07

    //以下方法用于时效统计
    //##ModelId=4855CD9503C8
    void addDelaySum1();

    //##ModelId=4855CDB4034B
    void addDelaySum2();

    //##ModelId=4855CDB7009C
    void addDelaySum3();

    //##ModelId=4855CDB8009C
    void addDelaySum4();

    //##ModelId=4855CDB9031C
    void addDelaySum5();

    //##ModelId=4855CDBA0290
    void addDelaySum6();

    //##ModelId=4855CDBB0280
    void addDelaySum7();

    //##ModelId=4855CDBC01F4
    void addDelaySum8();

    //##ModelId=4855CDBD00EA
    void addDelaySum9();
    

    //以下方法用于得到统计数据
    //##ModelId=4855CDBD038A
    int getDelaySum1();

    //##ModelId=4855CDFC01C5
    int getDelaySum2();

    //##ModelId=4855CDFD032C
    int getDelaySum3();

    //##ModelId=4855CDFE037A
    int getDelaySum4();

    //##ModelId=4855CE000119
    int getDelaySum5();

    //##ModelId=4855CE060251
    int getDelaySum6();

    //##ModelId=4855CE0701C5
    int getDelaySum7();

    //##ModelId=4855CE08006D
    int getDelaySum8();

    //##ModelId=4855CE0802EE
    int getDelaySum9();

    //##ModelId=4855CE6600CB
    void setFileLenth(const int fileLength);

    //##ModelId=4855CE7E0196
    void setDesFileLenth(const int fileLength);

    //##ModelId=4855CE90008C
    int getFileLenth();

    //##ModelId=4855CE9A02DE
    int getDesFileLenth();

    //##ModelId=4855CEA503B9
    void setSourceType(const string &strSourceType);

    //##ModelId=4855CECC029F
    const string & getSourceType();

    // 记录当前原始文件中cdr的最早开始时间和最晚开始时间
    //##ModelId=4855CF1A030D
    void setMinMaxStartDate(const char* pStartDate, const char * pStartTime);

    //##ModelId=4855CF4D0196
    const string &getMaxStartDateTime() const;

    //##ModelId=4855CF5F030D
    const string &getMinStartDateTime() const;

    //##ModelId=4855D021000F
    int getBeginCdrNo() const;

    //##ModelId=4855D05800EA
    void setBeginCdrNo(int left);

    //##ModelId=4855D06D009C
    int getEndCdrNo() const;

    //##ModelId=4855D080038A
    void setEndCdrNo(int left);

    // 江苏移动质量日志
    //##ModelId=4855E27E038A
    int getLogField();


    //##ModelId=4855E2A50280
    void getBeginDealTime();

    //##ModelId=4855E2B00157
    void getEndDealTime();

    //##ModelId=4855D09A0119
    int m_fileNo;

    //##ModelId=4855E16A005D
    int m_isDoLog;

    //##ModelId=4855E189006D
    string m_qualityLog;

    //##ModelId=4855E193001F
    string m_minEndTime;

    //##ModelId=4855E19C01E4
    string m_maxEndTime;

    //##ModelId=4855E1A5031C
    string m_beginDealTime;

    //##ModelId=4855E1AE02EE
    string m_endDealTime;

    //##ModelId=4855E1BE0222
    vector<string> m_LogFields;

  private:
    //##ModelId=4855D0E7036B
    //inline string getEndTime() const;

    //##ModelId=4855D123005D
    //inline string getStartTime() const;

    //##ModelId=4855D137031C
    PpLog();

    //##ModelId=4855D1D90177
    void setStartDateTime(void);

    //##ModelId=4855D209029F
    //##Documentation
    // 是否显示调试信息
    bool m_isShow;

    //##ModelId=4855D21202BF
    //##Documentation
    //## 原始话单文件名称
    string m_SrcFileName;

    //##ModelId=4855D21C0222
    //##Documentation
    //## 总话单数量
    int m_TotalCdrs;

    //##ModelId=4855D2240271
    //##Documentation
    //## 主叫话单数
    int m_MocSum;

    //##ModelId=4855D22E0251
    //##Documentation
    //## 被叫话单数
    int m_MtcSum;

    //##ModelId=4855D235033C
    //##Documentation
    //## 呼叫转移话单数
    int m_ForwSum;

    //##ModelId=4855D24C0203
    //##Documentation
	//## 漫游话单数
    int m_RoamSum;

    //##ModelId=4855D2540157
    int m_PocSum;

    //##ModelId=4855D25B01A5
    int m_PtcSum;

    //##ModelId=4855D26202CE
    //##Documentation
	//## 短信息主叫(MOC)话单数
    int m_SmocSum;

    //##ModelId=4855D26A01E4
    //##Documentation
	//## 短信息被叫(MTC)话单数
    int m_SmtcSum;

    //##ModelId=4855D2710177
    //##Documentation
	//## 其他类型话单数
    int m_OtherSum;

    //##ModelId=4855D2C0032C
    //##Documentation
	//## 无效话单数
    int m_NoUseSum;

    //##ModelId=4855D2CB02FD
    //##Documentation
	//## 错误话单数
    int m_ErrorSum;

    //##ModelId=4855D2D3029F
    //##Documentation
	//## 开始处理时间
    string m_StartDateTime;

    //##ModelId=4855D2DD034B
    //##Documentation
	//## 结束处理时间
    string m_EndDateTime;

    //##ModelId=4855D2EC00BB
    static PpLog *m_thePpLog;

    //##ModelId=4855D2FF000F
    int m_beginCdrNo;

    //##ModelId=4855D3060251
    int m_endCdrNo;

    //以下成员变量用于时效统计
    //##ModelId=4855D30E0203
    time_t m_tNowTime;

    //##ModelId=4855D31C00EA
    int m_delaySum1;

    //##ModelId=4855D326004E
    int m_delaySum2;

    //##ModelId=4855D32700AB
    int m_delaySum3;

    //##ModelId=4855D340003E
    int m_delaySum4;

    //##ModelId=4855D345036B
    int m_delaySum5;

    //##ModelId=4855D34B033C
    int m_delaySum6;

    //##ModelId=4855D353034B
    int m_delaySum7;

    //##ModelId=4855D3590148
    int m_delaySum8;

    //##ModelId=4855D35E01F4
    int m_delaySum9;

    //##ModelId=4855D36A01A5
    //##Documentation
    // 输入文件的长度
    int m_fileLenth;

    //##ModelId=4855D3730119
    //##Documentation
    // 输出文件的长度
    int m_desFileLenth;

    //##ModelId=4855D37B02CE
    //##Documentation
    // 数据源类型
    string m_sourceType;

    //##ModelId=4855D386006D
    //##Documentation
    // 最晚开始通话时间
    string m_MaxStartDateTime;

    //##ModelId=4855D38F0232
    //##Documentation
    // 最早开始通话时间
    string m_MinStartDateTime;

    //##ModelId=4855D38F0233
    //##Documentation
    // 上传总费用
	int m_UpTotalFee;
	
	
	//税后总话费 add By lizhao 2008-10-07
	int m_TotalFee;

public:
	OutputFile *m_OutPutFile;

//##ModelId=4855C6D8031C
static PpLog *getInstance()
{
	if (m_thePpLog == 0)
	{
		m_thePpLog = new PpLog;
	}

	return m_thePpLog;
}

//##ModelId=4855C86601E4
void setShowFlag(bool aShowFlag)
{
	m_isShow = aShowFlag;
}

//##ModelId=4855D0E7036B
string getEndTime() const
{
    return m_EndDateTime;
}

//##ModelId=4855C88102AF
bool isShowFlag()
{
	return m_isShow;
}

//##ModelId=4855D123005D
string getStartTime() const
{
    return m_StartDateTime;
}

//void setOutPutFile(OutputFile *pOutPutFile)
//{
//	m_OutPutFile = pOutPutFile;
//}
//
//OutputFile *getOutPutFile()
//{
//    return m_OutPutFile;
//}
void setOutPutFile(OutputFile *pOutPutFile);
OutputFile *getOutPutFile();

void WriteCdrtofile();

};

#endif /* PPLOG_H_HEADER_INCLUDED_B7A635D1 */
