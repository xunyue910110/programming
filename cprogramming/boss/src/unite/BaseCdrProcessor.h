// ##########################################
// Source file : BaseCdrProcessor.h
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


#ifndef BASECDRPROCESSOR_H_INCLUDED
#define BASECDRPROCESSOR_H_INCLUDED

#include "base/supdebug.h"

#include "ChanInfo.h"
#include "InOutPut.h"
#include "base/StringUtil.h"
#include "Log.h"

const int NEED_NOT_MERGE=0;
const int NEED_MERGE    =1;
const int MERGED        =2;

class InOutPut;

class BaseCdrProcessor
{
	public:
		BaseCdrProcessor();
		void setInOutPut(InOutPut *pInOutPut);  //设置输入输出
		virtual  ~BaseCdrProcessor();
		virtual string getFileType()   = 0;     //得到文件类型
		virtual void doMergeCdr(vector<string> *midFieldVect,vector<string> *desFieldVect) = 0; //合并记录
		// 得到用于到缓存里查找以往话单的关键字段组合
		virtual string getSearchKey();
		
		// 得到用户保存到缓存中,提供查找索引的关键字段组合
        string getBeginKey(vector<string> record);
        
        long timeTosec(string tmpDate, string tmpTime);

		virtual void isOverStored()    = 0; //判断缓存中的话单是否超时或溢出
		virtual void convertCdr2Fields() = 0;
		virtual void getMergedCdr()      = 0;  //得到合并记录
	
		// 纯虚函数,用来获得配置中的需要进行合并的字段,在cdr中的位置
		virtual bool setMergeFieldPos(const string &str) = 0;
		virtual bool setIndexFieldPosi(const string &posi)= 0;
		
		
		void setPreFix(const char * preFix);
		void setSearchFieldPosi(const int posi);
		void setCdrToken(const char token);
		void setMaxStoredCdr(const int maxCdrs );
		void setMaxDelay(const string &maxDelay );
		void setMergeFlagPosi(const string &posi);
		void setCdrFieldsCount(const string &count);
		void setMergeCdrFieldNum(const string & cdrfieldNum);
		bool matchPreFix(const char * fileName);
		void process(const char* cpFileName,ChanInfo * pChanInfo);
		///add by dzk
		void process_gprs(const char* cpFileName,ChanInfo * pChanInfo);
		
		string ComputeEndtime(string endDate, string endTime, int duration);
		
		int needMerge();
		int needMergeGprs();
		
	    int m_cdrFieldCount;                // 每条cdr中的总字段数
	    
	    int m_diffSec;

        int m_chanNo;
	protected:
		
		vector<string> m_vectPreFix;        // 需要匹配的文件名前缀
		vector<int>    m_searchFieldPosi;   // 关键search字段顺序  
		string  m_cdrToken;                 // cdr分割符
		int m_maxStoredCdr;                 // 缓存中最大话单条数阀值(单位：条)
		unsigned int m_maxDelaySecond;      // 缓存中话单通话时间到当前处理时间之差阀值(单位：秒) 
		vector<string> * m_pFieldVect;      // 每条cdr转换成字段后的字段值的容器 
		int m_mergeFlagPosi;

		
		
		char m_cdrBuffer[MAXCDRSIZE];		 
		
		CDR_POOL *m_cdrPool;                //class CDR_POOL:public map<string,vector<string>*>{};              
		InOutPut *m_pInOutPut;              
		Log *m_pLog;                        // 日志
		
		int m_cdrNum;          //  add by lizhao 2008-12-5
		int m_outResult;
		   int m_fieldNum;        //  ERROR_CODE 或 CALL_TYPE 的位置 add by lizhao 2008-12-5
		   bool m_flag;
        bool m_isFind;
        bool m_midCdr;
        bool m_isAddtoPool;
        bool m_changed;
        
        int m_beginTime;
        int m_beginDate;
        int m_endTime;
        int m_endDate;
        
        time_t m_first_time;
        time_t m_last_time;
	public:
		multimap<string, vector<string> >  m_needUnite;               // add by lizhao ,用来存储合并完成以前的记录
		multimap<string, vector<string> >  m_alreadyUnite;             //add by lizhao, 用来存储暂时未找到尾记录的需要合并的话单
		//CDR_POOL  m_notUnited;
	
	//话单顺序异常情况的处理	
    public:
        multimap<string, vector<string> > m_lastCdr;   //用来存储早于首话单、中间话单出现的尾话单

	private:
		BaseCdrProcessor(const BaseCdrProcessor & a){;}
};

#endif
