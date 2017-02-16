// ##########################################
// Source file : SendLog.h
// System      : Mobile Billing System
// Version     : 0.1.14
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020325
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef SendLog_H_INCLUDED_C484A413
#define SendLog_H_INCLUDED_C484A413

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
#include "appfrm/ErrorHandle.h"

USING_NAMESPACE(std)

//##ModelId=3B57CA38030E
//##Documentation
//## 预处理系统日志类
class SendLog
{
  public:
	
	//##ModelId=3B621680017D
	~SendLog();

	//##ModelId=3B6EA0580207
	inline static SendLog *getInstance();

	//##ModelId=3B57CCAB01BB
	//##Documentation
	//## 日志属性初始化方法,即全部属性清零。
	void clear();

	//##ModelId=3B70F1FB00F4
	bool write();

	//##ModelId=3B7CB8B002EB
	void ShowMessage(
		//##Documentation
		//## 要显示的信息内容
		string theMessage,
		//##Documentation
		//## 写日志文件标志（=0:不写文件;=1:带时间前缀写文件;=2:不带时间前缀写文件）
		int ShowFlag);

	//##ModelId=3B70EFB50350
	void setFileName(string filename);
	void setDesFileName(string filename);

	//##ModelId=3B57CCF203E4
	//##Documentation
	//## 用于日志信息的显示，可以输出到屏幕或者道日志文件。
	inline void setShowFlag(bool aShowFlag);

	//##ModelId=3B936D8B02F4
	inline bool isShowFlag();

	//##ModelId=3BD8CCED02AC
	void setEndDateTime(string &left);

	//##ModelId=3BD8CCED039D
	void addErrorSum();

	//##ModelId=3BD8CCEE0087
	void addForwSum();

	//##ModelId=3BD8CCEE022C
	void addMocSum();

	//##ModelId=3BD8CCEE0308
	void addMtcSum();

	//##ModelId=3BD8CCEE03DA
	void addNoUseSum();

	//##ModelId=3BD8CCEF01AB
	void addPocSum();

	//##ModelId=3C9F3E130309
    void addPtcSum();

	//##ModelId=3BD8CCEF0287
	void addRoamSum();

	//##ModelId=3BD8CCEF036E
	void addSmocSum();

	//##ModelId=3BD8CCF00062
	void addSmtcSum();

	//##ModelId=3BD8E9AC0196
	void addOtherSum();

	//##ModelId=3BD8CCF0031F
	void addTotalCdrs();

  void addTotalFee(int CdrFee);
  
  void addErrorFee(int ErrFee);

	//##ModelId=3BD8CCF00148
	void setSrcFileName(string &left);
	
	//##ModelId=3BD8EB6400EE
	string getEndDateTime() const;

	//##ModelId=3BD8EB640152
	int getErrorSum() const;

	//##ModelId=3BD8EB6401B7
	int getForwSum() const;

	//##ModelId=3BD8EB640225
	bool getisShow() const;

	//##ModelId=3BD8EB6402A7
	int getMocSum() const;

	//##ModelId=3BD8EB640315
	int getMtcSum() const;

	//##ModelId=3BD8EB64038D
	int getNoUseSum() const;

	//##ModelId=3BD8EB650013
	int getOtherSum() const;

	//##ModelId=3BD8EB65008C
	int getPocSum() const;

	//##ModelId=3C9F3E130331
    int getPtcSum() const;

	//##ModelId=3BD8EB650104
	int getRoamSum() const;

	//##ModelId=3BD8EB650186
	int getSmocSum() const;

	//##ModelId=3BD8EB6501FE
	int getSmtcSum() const;

	//##ModelId=3BD8EB650280
	string getSrcFileName() const;
	string getDesFileName() const;

	//##ModelId=3BD8EB650302
	string getStartDateTime() const;
	time_t getStartDateTime_tm() const;

	//##ModelId=3BD8EB650385
	int getTotalCdrs() const;
	int getTotalFee() const;
	int getErrorFee() const;

	//以下方法用于时效统计	
	void addDelaySum1();
	void addDelaySum2();
	void addDelaySum3();
	void addDelaySum4();
	void addDelaySum5();
	void addDelaySum6();
	void addDelaySum7();
	void addDelaySum8();
	void addDelaySum9();
	
	//以下方法用于得到统计数据	
	int getDelaySum1();
	int getDelaySum2();
	int getDelaySum3();
	int getDelaySum4();
	int getDelaySum5();
	int getDelaySum6();
	int getDelaySum7();
	int getDelaySum8();
	int getDelaySum9();
	
	void setFileLenth(const int fileLength);
	void setDesFileLenth(const int fileLength);
	int getFileLenth();
	int getDesFileLenth();
	void setSourceType(const string &strSourceType);
	const string & getSourceType();
	
	// add by chenm 2006-1-4 记录当前原始文件中cdr的最早开始时间和最晚开始时间
	void setMinMaxStartDate(const string & left);
	string getMaxStartDateTime() const;
	string getMinStartDateTime() const;
	
  public:
	//##ModelId=3BFDB1D4026A
	int getBeginCdrNo() const;

	//##ModelId=3BFDB1D40294
	void setBeginCdrNo(int left);

	//##ModelId=3BFDB1D40329
	int getEndCdrNo() const;

	//##ModelId=3BFDB1D40352
	void setEndCdrNo(int left);

	//##ModelId=3BFCAAE00055
	int m_fileNo;

  private:

	//##ModelId=3B6955FE0159
	inline string getEndTime() const;

	//##ModelId=3B6E542900F6
	inline string getStartTime() const;

	//##ModelId=3B62167F038E
	SendLog();
	
	//##ModelId=3BD8CCF0022F
	void setStartDateTime(void);

  private:
	//##ModelId=3B7CB8B000A6
	bool m_isShow;

	//##ModelId=3B57CACE0039
	//##Documentation
	//## 原始话单文件名称
	string m_SrcFileName;
	string m_DesFileName;

	//##ModelId=3B57CAE00201
	//##Documentation
	//## 总话单数量
	int m_TotalCdrs;
  
  //总话费
  int m_feeSum;
  
  //总错误话费
  int m_errFeeSum;
  
	//##ModelId=3B57CBF2007F
	//##Documentation
	//## 主叫话单数
	int m_MocSum;

	//##ModelId=3B57CC0703B4
	//##Documentation
	//## 被叫话单数
	int m_MtcSum;

	//##ModelId=3B57CC1303E4
	//##Documentation
	//## 呼叫转移话单数
	int m_ForwSum;

	//##ModelId=3B57CC23031E
	int m_RoamSum;

	//##ModelId=3B57CC4A0393
	int m_PocSum;

	//##ModelId=3C9F3E1301DC
    int m_PtcSum;

	//##ModelId=3B57CC5502EE
	//##Documentation
	//## 短信息主叫(MOC)话单数
	int m_SmocSum;

	//##ModelId=3B57CC6F0151
	//##Documentation
	//## 短信息被叫(MTC)话单数
	int m_SmtcSum;

	//##ModelId=3BD82AB7025A
	//##Documentation
	//## 其他类型话单数
	int m_OtherSum;

	//##ModelId=3B57CC7B0176
	//##Documentation
	//## 无效话单数
	int m_NoUseSum;

	//##ModelId=3B57CC8D014A
	//##Documentation
	//## 错误话单数
	int m_ErrorSum;

	//##ModelId=3B57CE47029A
	//##Documentation
	//## 开始处理时间
	string m_StartDateTime;

	//##ModelId=3B57CE580118
	//##Documentation
	//## 结束处理时间
	string m_EndDateTime;

	//##ModelId=3B6EA0D60082
	static SendLog *m_theSendLog;

	//##ModelId=3BFDB199039C
	int m_beginCdrNo;

	//##ModelId=3BFDB1A801DB
	int m_endCdrNo;
	
	time_t m_tNowTime;
		
	//以下成员变量用于时效统计
	int m_delaySum1;
	int m_delaySum2;
	int m_delaySum3;
	int m_delaySum4;
	int m_delaySum5;
	int m_delaySum6;
	int m_delaySum7;
	int m_delaySum8;
	int m_delaySum9;
	
	// 输入文件的长度
	int m_fileLenth;
	// 输出文件的长度
	int m_desFileLenth;
	string m_sourceType;
	// 最晚开始通话时间 add by chenm 2006-1-4
	string m_MaxStartDateTime;
	// 最早开始通话时间
	string m_MinStartDateTime;
};

//##ModelId=3B6EA0580207
SendLog *SendLog::getInstance()
{
	if ( m_theSendLog == 0 )
		m_theSendLog= new SendLog;

	return m_theSendLog;
}

//##ModelId=3B57CCF203E4
void SendLog::setShowFlag(bool aShowFlag)
{
	m_isShow=aShowFlag;
}

//##ModelId=3B6955FE0159
string SendLog::getEndTime() const
{
    return m_EndDateTime;
}

//##ModelId=3B936D8B02F4
bool SendLog::isShowFlag()
{
	return m_isShow;
}

//##ModelId=3B6E542900F6
string SendLog::getStartTime() const
{
    return m_StartDateTime;
}

#endif /* SendLog_H_INCLUDED_C484A413 */
