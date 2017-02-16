#ifndef MONTHENDAPP_H_HEADER_INCLUDED_B8827F98
#define MONTHENDAPP_H_HEADER_INCLUDED_B8827F98

//#include "MonthEndOutputFile.h"
#include "MonthInternalData.h"

#include "appfrm/Application.h"
#include "dbi/dbi.h"
#include "AltibaseIntf.h"

/**
 * MonthEndApp：完成月底开帐总体框架类.
 * 含有下面的方法:
 *    initialization:   初始化
 *    Process:          处理
 *    recovery:         恢复
 *    end:              结束
 */

//##ModelId=477C465402DC
class MonthEndApp :public Application
{
  public:
    //##ModelId=477C4655000C
    MonthEndApp();

    //##ModelId=477C4655000D
    ~MonthEndApp();

  protected:
    //##ModelId=477C4655000E
    virtual bool initialization();

    //##ModelId=477C46550010
    virtual bool beforeLoop();

    //##ModelId=477C46550012
    virtual bool loopProcess();

    //##ModelId=477C46550014
    virtual bool recovery();

    //##ModelId=477C46550016
    virtual bool end();

  private:
    //##ModelId=477C46550018
    bool getSysParams();

    //##ModelId=477C46550019
    bool getSysParamsFromDb();

    //##ModelId=477C4655001D
    void getDealDate();

    //##ModelId=477C46550025
    bool monthEndRun();
    
    
    int writeAlarm(const int ecode,const char *msg);
    
    bool isValidPath(const string& path);
    
    //分割，枚举
    void splitEnumValue(const char* r_pEnumValue, 
	                            		set<string> &r_sValueList);

    //##ModelId=477DF553026D
    MonthEndConfig m_monthEndConfig;

    //##ModelId=477C465403B9
    string m_fileName;

    //##ModelId=477C465403BA
    char m_fileType;

    //##ModelId=477C465403BB
    int m_fileBcycId;

    //##ModelId=477C465403C2
    int m_minRealCycId; //当前需要开帐的帐期

    //##ModelId=477C465403C3
   // int m_currentBcycId; //当前月份

    //##ModelId=477C465403C4
  //  AcycPara m_acycPara;
   
    //##ModelId=477C465403C5
    string m_provinceCode; //当前省份编码

    //##ModelId=477C465403C6
    string m_dealDate;

    //##ModelId=477C465403CC
    string m_performId;

    //##ModelId=477C465403CD
    char m_errorTag;

    //##ModelId=477DE75C010D
    PM_MonthAccount *m_monthAccountParam;


    //##ModelId=477C46550003
 //   vector<AccountCheckInfo> m_accountCheckInfoVector;


    //##ModelId=477C465403D5
    MonthEndOutputFile m_monthEndOutputFile;


    //##ModelId=477C465403E7
    MonthInternalData m_monthInternalData;
    
    //零明细账单是否入综合账单标识
    char m_zeroDetailBillIntoIntDbTag;
    
    //通道划分
    vector<channelSt> m_channelStVec;

    //批次数
    int m_fileBatchNum;
    
    //开账地市
    vector<string> m_actEparchyCode;

};



#endif /* MONTHENDAPP_H_HEADER_INCLUDED_B8827F98 */
