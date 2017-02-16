/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
*
* 文件名称：CMonfeeInternalData.h
* 文件标识：
* 摘    要：完成用户内存数据类的定义
*
* 取代版本：
* 原作者  ：
* 完成日期：
*/
#ifndef CMonfeeInternalData_H
#define CMonfeeInternalData_H


#include "BillingCommon.h"
#include "IdInfo.h"
//#include "Mdb_UserServState.h"
#include "CMonfeeBillMgr.h"

class CMonfeeParamMgr;
class CDiscntMonfee;
class CRecvMonfee;

/**
 * CMonfeeInternalData:用户内部数据类.
 * 含有下面的方法:
 *   initialize：                   用户内部数据类初始化
 *   setFileBill：                  根据输入文件中的用户增量帐单设置内存
 *   syncMemoryBill：               将用户增量帐单同步到内存并获取全量帐单
 *   judgeIfExistUserInfo：         判断该用户标识是否存在用户信息
 *   getUserInfo：                  根据用户标识获取用户具体信息
 *   getUserDiscount：              根据用户标识获取用户优惠信息(优惠＋帐前调整)
 *   getUserAddupInfo：             根据用户标识获取用户累计信息
 *   getDetailBill：                转化用户帐单格式，供优惠类使用
 *   getUserBill：                  转化用户帐单格式，供付费关系类使用
 *   soluteUserDiscount：           对用户帐单进行优惠处理
 *   solutePayRelation：            对用户付费关系进行处理
 *   checkUserBillInfo：            对用户帐单进行核查
 *   addupInfo：                    对用户相关信息进行累计
 *   out：                          输出内部用户各种相关数据
 *   正常用户调用流程：
 *      initUser -> syncMemoryBill -> getUserInfo -> getUserDiscount ->
 *      getUserAddupInfo -> getUserPayInfo -> getDetailBill -> dataPrice ->
 *      soluteUserDiscount -> getUserBill -> checkUserBillInfo -> addupInfo ->
 *      judgeIfExistGroupDiscount -> solutePayRelation -> out
 *                                -> out
 *   无资料用户调用流程：
 *      initUser -> syncMemoryBill -> getUserInfo -> getDetailBill ->
 *      dataPrice -> getUserBill -> checkUserBillInfo -> addupInfo -> out
 */
class CMonfeeInternalData
{
    public:

        CMonfeeInternalData();

        ~CMonfeeInternalData();

        /**
         * initialize：用户内部数据类初始化
         * @param  cycpara         帐期信息.
         * @param  provinceCode     省份编码.
         * @param  operateId        操作标识.
         * @param  operateTypeTag   操作类型标志.
         * @param  monfeeParamMgr 用户出帐资料参数管理
         * @return 无
         */

        bool    initialize(const CycParam& cycPara,
                           const string& provinceCode,
                           const string& performId,
                           const char &  fileType,
                           CMonfeeParamMgr *monfeeParamMgr,
                           const String<14>& dealDate,
                           const vector<IdDiscount>& sysDiscountVector,
                           SpRelaDiscntMap *spRelaDiscntMap,
                           MonfeeConfig *monfeeConfig);//modify by yuxk

        /**
         * initUser：用户内部数据类初始化用户信息
         * @param  userId           用户标识.
         * @return 无
         */
        void    initUser(const USERID& userId,
        				vector<UserSubInfo>& userSubInfoVector);

        /**
         * syncMemoryBill：将用户增量帐单同步到内存并获取全量帐单
         * @param  userBillManager          用户内存帐单管理类.
         * @param  userOriginalBillVector   用户原始增量帐单.
         * @return true:成功 false:失败
         */
        bool    syncMemoryBill(CMonfeeBillManager& userBillManager);

        /**
         * getUserInfo：根据用户标识获取用户具体信息
         * @param  无.
         * @return -1 表示Mdb异常，0 表示查找结果为空 ，1 表示查找出结果
         */
        int    getUserInfo(int& acctTag);

		/**
         * getUserSVCState：根据用户标识获取用户异动信息
         * @param  无.
         * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
         */
        int    getUserSVCState();

        /**
         * getUserDiscount：根据用户标识获取用户优惠信息
         * @param  无.
         * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
         */
        int  getUserDiscount();

        /**
         * getUserBill：转化用户帐单格式，供付费关系类使用
         * @param  无.
         * @return 无
         */
        void    getUserBill();
        void    getUserBillOld();

        /**
         * soluteUserDiscount：对用户帐单进行优惠处理
         * @param  userDiscountSolution：用户优惠类.
         * @return 无
         */
        void    discntMonfee(CDiscntMonfee& cDiscntMonfee);

        /**
         * recvMonfee：收取包月费
         * @param  CRecvMonfee：用户付费关系处理类.
         * @param  fileType：文件类型
         * @return 无
         */
        void    recvMonfee(CRecvMonfee& cRecvMonfee);

        /**
         * checkUserBillInfo：对用户帐单进行核查
         * @param  无.
         * @return 无
         */
        void    checkUserBillInfo();

        /**
         * addupInfo：对用户相关信息进行累计
         * @param  accountAddupInfo：出帐累计信息结构
         * @return 无
         */
        void    addupInfo(AccountAddupInfo& accountAddupInfo);

        /**
         * out：输出内部用户各种相关数据
         * @param  fileType:    文件类型.
         * @param  os:          输出流.
         * @param  recordNum:   记录数.
         * @param  feeSum:      总金额.
         * @return 无
         */
        void    out(const int& fileType, ostream& os, const string& outputFileName,
                    int& recordNum, int& feeSum);

        // Add by Malone 2008-1-25 start
        /**
         * isDumbnessUser：判断是否为沉默/单高用户.
         * @param  userId:用户标识.
         * @return true:是
         *         false:不是
         */
        //bool	isDumbnessUser(const USERID& userId);
        // Add by Malone 2008-1-25 end



    private:
        /**
         * outUserErrorInfo：输出错误用户信息
         * @param  os:          输出流.
         * @param  recordNum:   记录数.
         * @param  feeSum:      总金额.
         * @return 无
         */
        void outUserErrorInfo(ostream& os, int& recordNum, int& feeSum);
        void outCdr(ostream& os,int& recordNum,
        			int& feeSum,const char & fileType);
		void outIBill(ostream& os,int& recordNum,int& feeSum);
		void outPBill(ostream& os,int& recordNum,int& feeSum);
		void outDBill(ostream& os,int& recordNum,int& feeSum);
		void outBBill(ostream& os, const string& outputFileName, int& recordNum, int& feeSum);


    private:

        USERID                      m_userId;
        CycParam                    m_cycPara;
        string                      m_provinceCode;
        string                      m_performId;

        String<14>                      m_dealDate;
        String<14>                      m_currentDate;
        
//        UserInfo                    m_userInfo;
        IdInfo 						m_idInfo;
        char						m_fileType;
        /**
         *  m_ifExistUserInfo：     该用户是否存在用户资料
         *  1:正常用户   0:无资料用户  -1:数据库异常
         */
        int                      m_ifExistUserInfo;

        CMonfeeParamMgr          *m_monfeeParamMgr;

        vector<UserSubInfo> 		m_userSubInfoVector;
        vector<UserSubInfo> 		m_userSubBillVector; //zhaogc add

        vector<UserServState>     m_userSVCStateVector;
        vector<MonfeeTariff>      m_monfeeTariffVector;
        vector<MonfeeDiscount>    m_discountVector;

        vector<IdErrorInfo>         m_userErrorInfoVector;

        vector<CMonfeeBill>        m_memBillVector;
        vector<CMonfeeBill>        m_memBillVectorOld;

        vector<CdrBill>        		m_cdrVector;
				vector<CdrBill>        		m_cdrVectorOld;
				
				vector<OutBill>						m_outBillVector;


        map<int,DetailBill>          m_detailBillMap;
        map<int,DetailBill>          m_detailBillMapOld;
        //SpPunishMap                  m_spPunishMap;//sp处罚规则
        
        vector<IdDiscount>					m_sysDiscountVector;
        
        SpRelaDiscntMap							*m_spRelaDiscntMap;
      //add by yuxk for MT
	MonfeeConfig  *m_monfeeConfig; 
        int m_unitfee_M;
        int m_unitfee_T;
         vector <string>	 m_iphoneVec;   
     //add by yuxk for MT	 

};


#endif
