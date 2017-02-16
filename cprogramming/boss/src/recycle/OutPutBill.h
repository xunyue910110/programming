#ifndef __OUTPUTBILL__
#define __OUTPUTBILL__

#include "RecConstDef.h"
#include "RecConfig.h"
#include "BillManager.h"

#include "base/StringUtil.h"

class OutPutBill {
    public:
        //vector<OutputRule>* m_pOutputRule;  //文件输出规则列表指针
        string              m_dealTime;     //mmddhhmi
        //vector<Channe_Def>  m_channelList;  //通道列表
        vector<long>        m_cdrNum;       //话单数
        int                 m_maxCdrNum;    //一个文件的最大数
        int                 m_billNum;
    	RecConfig		  * m_config;    
    private:
	
        int                          m_lable;
        //vector<OutputRule>::iterator m_ruleItr;
        //vector<Channe_Def>::iterator m_channelItr;
        	
        //Added by Sunam 2006-3-14 10:38 当前排重规则
        //vector<DividFormat>::iterator m_rrRuleItr;
        ofstream                     m_uidFileOfstream;
        string                       m_uidFileName;
        
        ofstream                     m_billFileOfstream;
        	
        vector<string>               m_billStringVec;
        string                       m_cdrString_raw;
        string                       m_minusBillFile;
        //string                       m_fullName;
        string                       m_minusBillName;
        vector<string>               m_tmpFileList;
        string                       m_fieldValue;   //存放临时输出字段
        char                         m_format[20];
        char                         m_fieldBuff[MAXFIELDLEN_REC+1];
        string                       m_errMsg;
        int                          m_fileNo;
        //void setFullName(char* t_fullName){m_fullName = t_fullName;}
                 
    public:
        bool initialize(/*vector<OutputRule>* outputRuleList,const int maxCdrNum,*/
                const char *dealTime,RecConfig * config);
        //bool setDealTime(const char *dealTime);
        bool outputUserBill(const int &r_billCycle,vector<RollBackBill> &r_userBill);
        //bool putCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC]);
        //bool writeCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC]);
        bool commit();
        bool rollBack();
        bool end();
};


#endif //
