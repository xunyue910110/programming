// ##########################################
// Source file : GsmCdrProcessor.h
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


#ifndef GSMCDRPROCESSOR_H_INCLUDED
#define GSMCDRPROCESSOR_H_INCLUDED

#include "base/supdebug.h"
#include "BaseCdrProcessor.h"
#include "time.h"

class GsmCdrProcessor:public BaseCdrProcessor
{
	public:
		GsmCdrProcessor(){;}
		
		virtual string getFileType() 
		{ 
			string strType = string("GSM");
			return strType; 
		}
		
		virtual void doMergeCdr(vector<string> *midFieldVect,vector<string> *desFieldVect);
		virtual void isOverStored();  //判断缓存中的话单是否溢出
		virtual void convertCdr2Fields();
		virtual void getMergedCdr();
		virtual bool setMergeFieldPos(const string &str); //用来获得配置中需要合并的字段，在记录中的位置
		virtual bool setIndexFieldPosi(const string &posi);
		
		

	private:
		int m_startDatePosi ;      //开始日期
		int m_startTimePosi ;      //开始时间
		int m_durationPosi  ;      //通话时长
		int m_endDatePosi   ;
		int m_endTimePosi   ;
		
};

#endif
