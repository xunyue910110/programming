// ##########################################
// Source file : GprsCdrProcessor.h
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020423
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################


#ifndef GPRSCDRPROCESSOR_H_INCLUDED
#define GPRSCDRPROCESSOR_H_INCLUDED

#include "base/supdebug.h"
#include "BaseCdrProcessor.h"
#include "time.h"

class GprsCdrProcessor:public BaseCdrProcessor
{
	public:
		GprsCdrProcessor(){;}

		virtual string getFileType() 
		{ 
			string strType = string("GPRS");
			return strType;
		}
		
		virtual void doMergeCdr(vector<string> *midFieldVect,vector<string> *desFieldVect);
		virtual void isOverStored();  //判断缓存中的话单是否溢出
		virtual void convertCdr2Fields();
		virtual void getMergedCdr();
		virtual bool setMergeFieldPos(const string &str); //用来获得配置中需要合并的字段，在记录中的位置
		virtual bool setIndexFieldPosi(const string &posi);
		
	
	private:	
		int m_startDatePosi;       //开始日期
		int m_startTimePosi;       //开始时间
		int m_call_duration;       //通话时长
		int m_dataUp1      ;       //上行流量1
		int m_dataDn1      ;       //下行流量1
		int m_dataUp2      ;       //上行流量2
		int m_dataDn2      ;       //下行流量2
		
};

#endif
