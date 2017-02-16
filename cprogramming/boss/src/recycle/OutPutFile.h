#ifndef __OUTPUTFILE__
#define __OUTPUTFILE__

#include "RecConstDef.h"
#include "RecConfig.h"

#include "base/StringUtil.h"


class RedoFileName
{
	public :
		//给dsp的raw文件名
		string m_stdFileName;
		//分拣输入的文件名两个文件名可能有所不同
		string m_orgFileName;
};

class Channe_Def {
    public:
        string          m_stdFilePath;
        string          m_bakFilePath;
//	 string          m_billFilePath;
        string          m_fileName;
        
        ofstream        m_ofstream;
        ofstream 		m_ofstream_raw;
        int             m_fileNo;
        long            m_cdrNum;
	 bool          m_ifFull;
        //vector<string>  m_tmpFileList;
        vector<RedoFileName> m_tmpFileList;
    public:
        Channe_Def(){m_tmpFileList.clear();}
        Channe_Def(const Channe_Def &right) {
            m_stdFilePath = right.m_stdFilePath;
            m_bakFilePath = right.m_bakFilePath;
            m_fileName    = right.m_fileName;
            m_fileNo      = right.m_fileNo;
            m_cdrNum      = right.m_cdrNum;
            m_tmpFileList = right.m_tmpFileList;
        }
        Channe_Def(Channe_Def &right) {
            m_stdFilePath = right.m_stdFilePath;
            m_bakFilePath = right.m_bakFilePath;
            m_fileName    = right.m_fileName;
            m_fileNo      = right.m_fileNo;
            m_cdrNum      = right.m_cdrNum;
            m_tmpFileList = right.m_tmpFileList;
        }
        void operator=(const Channe_Def& right) {
            m_stdFilePath = right.m_stdFilePath;
            m_bakFilePath = right.m_bakFilePath;
            m_fileName    = right.m_fileName;
            m_fileNo      = right.m_fileNo;
            m_cdrNum      = right.m_cdrNum;
            m_tmpFileList = right.m_tmpFileList;
        }
};

class OutPutFile {
    public:
        vector<OutputRule>* m_pOutputRule;  //文件输出规则列表指针
        string              m_dealTime;     //mmddhhmi
        vector<Channe_Def>  m_channelList;  //通道列表
        vector<long>        m_cdrNum;       //话单数
        int                 m_maxCdrNum;    //一个文件的最大数
    	RecConfig		  * m_config;    
    private:
	
        int                          m_lable;
        vector<OutputRule>::iterator m_ruleItr;
        vector<Channe_Def>::iterator m_channelItr;
        	
        //Added by Sunam 2006-3-14 10:38 当前排重规则
        vector<DividFormat>::iterator m_rrRuleItr;
        ofstream                     m_uidFileOfstream;
        string                       m_uidFileName;
        	
        string                       m_cdrString;
        string                       m_cdrString_raw;
        string                       m_fieldValue;   //存放临时输出字段
        char                         m_format[20];
        char                         m_fieldBuff[MAXFIELDLEN_REC+1];
        string                       m_errMsg;
                 
    public:
        bool initialize(vector<OutputRule>* outputRuleList,
                const char *dealTime,const int maxCdrNum,RecConfig * config);
        bool setDealTime(const char *dealTime);
        bool putCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC]);
        bool writeCdr(char cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC]);
        bool commit();
        bool rollBack();
        bool end();
    private:
        int   copy(const char *r_srcFile,const char *r_desFile) const;
};


#endif //
