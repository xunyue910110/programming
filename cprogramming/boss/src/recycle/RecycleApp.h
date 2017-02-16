
#ifndef __RECYCLEAPP_H__
#define __RECYCLEAPP_H__

#include "config-all.h"

#include "RecConstDef.h"
#include "RecConfig.h"
#include "OutPutFile.h"
#include "OutPutBill.h"
#include "BillManager.h"
#include "BillUpdate.h"

#include "base/TimeUtil.h"

#include "appfrm/Application.h"
#include "dbi/dbi.h"


#define LOG_COUT ofs_logfile

class RecycleApp : public Application {
    public:
        RecycleApp();
        ~RecycleApp();
        char    m_cdr[MAXFILEDNUM_REC][MAXFIELDLEN_REC];//话单

    protected:
        virtual bool initialization();
        virtual bool recovery();
        virtual bool loopProcess();
        virtual bool end();

    private:
        RecConfig          m_config;          //配置信息
        vector<OutputRule> m_outputRuleList;  //定义输出格式列表
        int                m_maxFieldNum;         
        
        DbInterface        m_db;              //DBI接口
        int                m_connected;       //数据库连接标记：0 不连接

        OutPutFile         m_outputFile;
        
    	time_t             lNowTime;
    	struct tm *        stNowTime;
    	char               chNowTime[30];
    	ofstream           ofs_logfile;
    	
    	vector<RedoFlagConfig> m_redoFlag;

        //BillManager        m_billManager;
        BillManager    m_billManager; //帐单管理对象
        BillUpdate     m_billUpdate;
        OutPutBill     m_outPutBill;
        vector<RollBackBill>   m_rollBackBillVec;

		//日志信息
		long               m_nTotal;
		string  		   m_bizType;
		int                m_billingCycle;
		string             m_status;
		string			   m_tableName;
		
    private:

        bool    getOutPutFileTypes();
        bool    getSysParams();
        bool    setFieldNum(const string &str);  //设置字段数
        //void    cdrFieldProcess();
        bool    recycleRun();
        bool    checkRecycleFlag();
        bool    updateFlag(const int index);
        bool    insertLog(const int r_tag,const char* logText);
        bool    rollBack();
        bool    executeProc(string procName);

		void    writeLog();
        string  m_errMsg;
};

#endif // 

