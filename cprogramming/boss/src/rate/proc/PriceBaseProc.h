#ifndef _PRICEBASEPROC_H_INCLUDE_20080702_
#define _PRICEBASEPROC_H_INCLUDE_20080702_

#include "Procedure.h"
#include "config-all.h"
#include "TpInfo.h"
#include "RateGlobalInfo.h"
#include "TpRealRentMgr.h"

class PriceBaseProc:public Procedure 
{
  public:
    virtual bool initialize();
    virtual int execute();   
  protected:
    virtual bool cdr_init();     //话单级初始化
    virtual bool getCdrSegments();//获取话单段
    virtual void setOrgFeeInfo();//设置原始费用信息
    virtual void setOrgFeeInfo(const int &r_feeField)=0;//设置原始费用信息
    virtual bool pricing();      //批价
    //设置实收费用
    void setItemFee(const int &r_feeField);
    //设置批价结果
    virtual bool setFeeResult();
    //设置跳次信息
    virtual void setTimes();
    //设置一次批价费用
    virtual void setBaseFee(const int &r_feeField);
    //一些批价后的特殊处理
    virtual bool specialProcess();

  protected:    
    //一个事件类型政策对一条话单的批价  
    //r_rateTp  事件政策政策
    //返回值： true 批价成功  
    //         false (m_errCode==0 没有满足条件的情况)
    //         false (m_errCode!=0 异常出错)
    bool eventPrice(RateTpInfo &r_rateTp);
    //一个批价政策对一个费用域的批价  
    //r_price  批价政策
    //返回值： true 批价成功  
    //         false (m_errCode==0 没有满足条件的情况)
    //         false (m_errCode!=0 异常出错)
    bool rateTpPrice(Price *r_price);
    //按天分割话单，形成话单天分割段列表
    bool getCdrSegments(CdrSeg &r_cdrTotalSeg,vector<CdrSeg> &r_cdrSegList);
    //将政策属性填入CDR
    void setTpInfoToCdr(const RateTpInfo *r_tpInfo);
    //设置计费账单轨迹
    void   setBillInfos();
    //提交赠送时长的结果
    void commitFldSendMin();
    //提交赠送优惠量信息
    void commitSendBillInfo(const RateBillInfo &r_billInfo);
    //合明细账单
    bool setDetailBill();
    //设置优惠轨迹
    void setRateTrack();
    void selectTpResult();  //多个政策间的选取 
    void selectTpResult(const bool &r_samePri,
                              CdrFeeInfo &r_cdrFeeInfo1,
                              CdrFeeInfo &r_cdrFeeInfo2);
    
    //设置错误信息
    void setErrorCode(const int &r_errCode);
    //获取错误信息
    int  getErrorCode();
  protected:
    vector<RateTpInfo>      *m_pTpList;    //资费政策列表
    vector<int>             *m_pRateTpList;//批价资费政策下标列表
    int                      m_errCode;    //错误代码
    TpRealRentMgr            m_tpRentMgr;  //套餐实扣月租管理器
    
    //账单轨迹信息为:{用户ID|账单类型|帐期|资费实例|起始时间|账单项|值}
    // 多个信息间 以 分号 分割
    char                     m_billTrack[TRACK_INFO_MAXLEN+1]; //计费账单轨迹
    int                      m_billTrackLen;        //账单轨迹长度
  protected:
    RateInfo *m_pRateFeeInfo; //批价信息
    vector<RateBillInfo>     m_tpSendMinList;  //一个政策赠送时长累计
    vector<RateBillInfo>     m_fldSendMinList; //一个费用域赠送时长累计
    vector<UserSumBill1>     m_tpRentFeeList;  //套餐实扣费用
    vector<UserBill>         m_tpRentBillList; //套餐实扣账单
  //Added By Sunam 2009/9/2 11:09:34
  private:
    bool m_continueProcessFlag[FEEFIELDS_MAX];
    set<T_BILL_ID> m_rateBillSet;
};

#endif //_PRICEBASEPROC_H_INCLUDE_20080702_
