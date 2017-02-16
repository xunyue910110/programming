/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
*
* CMdbInfoMgr.h
* 文件标识：
* 摘    要：用户资料接口类，提供对用户基本资料、用户服务状态等信息的查询功能，访问内存数据库。
*
* 取代版本：
* 原作者  ：
* 完成日期：
*/
#ifndef CMdbInfoMgr_H
#define CMdbInfoMgr_H

#include "MonfeeCommon.h"

typedef vector<UserSubInfo>::iterator UserSubInfoIter;


/**
 * CMonfeeParamMgr：用户出帐涉及资料、参数内存管理管理.
 * 含有下面的方法:
 *   getCycParaFromDate:        根据时间获取帐期信息
 *   getCycIdFromDate:          根据时间获取帐务帐期标识
 *   getCycParaFromBcycId:      根据计费帐期标识获取帐期信息
 *   getSysParamInfo:            根据系统参数编码获取系统信息
 *   getUserInfo:                根据用户标识获取用户资料信息
 *   getUserPayRelation:         根据用户标识、帐期标识获取用户有效付费关系
 *   getUserDiscount:            根据用户信息、帐期信息获取用户有效优惠信息
 *   getUserAddupInfo:           根据用户标识、帐期信息获取用户有效累计信息
 *   judgeIfExistGroupDiscount:  根据用户、帐期信息判断是否存在群优惠
 *   judgeIfExistBindRelation:   根据用户、帐期信息判断是否存在绑定关系
 *   getLocalProvinceCode:       获取本地省份编码
 *   getItemInfo:                获取明细帐目和明细帐目相关的对应信息
 *   getDetailItemInfo:          根据明细帐目编码获取明细帐目具体信息
 *   getDetailItemCode:          根据外部系统类型和外部帐目编码获取对应的明细帐目编码
 *   getIntegrateItemCode:       根据地州编码和明细帐目编码获取对应的综合帐目编码
 *   judgeIfRealOutputDiscount:  根据用户、帐期判断是否实时输出优惠信息
 *   getInvalidStateInfo:        获取导致优惠无效的状态信息集
 *   getAccountParam:            返回通用资料参数管理器对象地址
 *   getStateDiscntByEparchyCode: 根据地州编码获取状态优惠信息       
 *   getStateDiscntByDiscntId:    根据状态编码获取状态优惠信息       
 *   judgeStateEffect:            根据主服务状态导致优惠失效编码判断优惠是否失效
 */

class CMdbInfoMgr
{
    public:
// added by zhoujie 20050708 start
        CMdbInfoMgr();

        ~CMdbInfoMgr();
        
        bool initialization();
        
        bool end();
        
        bool needUpdate();
        
        bool update();

    public:

        virtual bool addParamObject();

        virtual bool getData();

    public:

        /**
         * getCycParaFromDate:根据时间获取帐期信息
         * @param  dateTime 时间，格式为yyyymmddhh24miss.
         * @param  cycPara 帐期信息.
         * @return true:成功 false:失败
         */
        bool getCycParaFromDate(const string& dateTime,CycPara& cycPara);

        /**
         * getCycIdFromDate:根据时间获取帐务帐期标识
         * @param  dateTime 时间，格式为yyyymmddhh24miss.
         * @param  cycId   帐务帐期标识.
         * @return true:成功 false:失败
         */
        bool getCycIdFromDate(const string& dateTime,int& cycId);

        /**
         * getCycParaFromCycId:根据计费帐期标识获取帐期信息
         * @param  cycId   计费帐期标识.
         * @param  cycPara 帐期信息.
         * @return true:成功 false:失败
         */
        bool getCycParaFromCycId(const int& cycId,CycPara& cycPara);

        
        /**
         * getSysParamInfo:根据系统参数编码获取系统信息
         * @param  sysParamCode         系统参数编码.
         * @param  sysParamInfoVector   系统参数信息集.
         * @return true:成功 false:失败
         */
        bool getSysParamInfo(const string& sysParamCode,
                             vector<SysParamInfo>& sysParamInfoVector);

        /**
         * getUserInfo:根据用户标识获取用户资料信息
         * @param  userId     用户标识.
         * @param  userInfo   用户信息.
         * @return true:成功 false:失败
         */
        bool getUserInfo(const USERID& userId,UserInfo& userInfo);

        /**
         * getUserPayRelation:根据用户标识、帐期标识获取用户有效付费关系
         * @param  userId              用户标识.
         * @param  cycId              帐期标识.
         * @param  payRelationVector   付费关系集.
         * @return 无
         */
        
        void getUserDiscount(const char& dataType,
                             const UserInfo& userInfo,                             
                             const CycPara& cycPara,
                             vector<MonthFeeDiscount>& discountVector);

        
        void getUserSVCState(const USERID& userId,
                             vector<UserSVCState>& userSVCStateVector);

        /**
         * getLocalProvinceCode:获取本地省份编码
         * @param  provinceCode     省份编码
         * @return 无    added by zhoujie 20050712
         */
        bool getLocalProvinceCode(string& provinceCode);
                                        
        void getTariffBySubInfo(const char &dataTypeCode,
                                const UserSubInfo& userSubInfo,                                      
                                const CycPara &cycPara,                                             
                                vector<MonthFeeTariff>& monthFeeTariffVector);
                                
        bool judgeNotOpen(    const string& startDate,
                              const string& endDate,
                              const vector<UserSVCState>& userSVCStateVector);
    
             
        
        void getSubInfoByDiscnt(const MonthFeeDiscount& monthFeeDiscount,
                               const CycPara &cycPara,
                               vector<UserSubInfo>& userAllSubInfoVector,
                               vector<UserSubInfoIter>& userDiscountVector);
        
        void getUserNum(const USERID& ecUserId, const String<18>& spBizCode,
                        /*const String<18>& spCode,*/ int &usernum);
        //zhaogc add                        
        bool getUserState(const USERID& ecUserId, const String<18>& spBizCode);
        
        bool judgeDumbnessUser(const int& cycId, const String<20>& serialNumber, const String<18>& spCode,
                            const String<18>& spServCode, const MonfeeTariff& monfeeTariff);
        
        /**
         * getSpPunishRule:获取sp处罚规则
         * @return true:成功 false:失败
         */
        bool getSpPunishRule(const CycPara& cycPara,SpPunishMap& spPunishMap);  
        
        // Add by Yih 2008-08-06 start
        /*
         * getAllRelaDiscount:获取所有SP关联优惠信息
         * @param  spRelaDiscntVector   SP关联优惠信息集.
         * @return true:成功 false:失败
         */
        bool getAllRelaDiscount(vector<SpRelaDiscnt> &spRelaDiscntVector); 
        // Add by Yih 2008-08-06 end
        
    private:          
        bool getEffDiscntByIdDiscnt(const char& dataType,
                                    const IdDiscount& idDiscnt,
                                    const CycPara& cycPara,
                                    const string& openDate,
                                    vector<MonthFeeDiscount> & dVector);
                                    
        void getSysDiscount(const string&  discntObjectType,          
                                const string& cycStartTime,           
                                const string& cycEndTime,             
                                vector<IdDiscount>& acctDiscountVector);

        bool getInfoFromDiscountId(
                                const char& dataType,
                                //Modified by Malone start
                                /*const int& discountId,*/ 
                                const IdDiscount& idDiscnt,
                                //Modified by Malone end
                                const CycPara& cycPara,   
                                vector<MonthFeeDiscount> & dVector);
        
        /**
         * getUserCommonDiscount:根据用户标识、帐期时间获取用户有效普通优惠集
         * @param  userId               用户标识.
         * @param  cycStartTime        帐期起始时间.
         * @param  cycEndTime          帐期终止时间.
         * @param  userDiscountVector   用户有效普通优惠集
         * @return 无
         */
        void getUserCommonDiscount(const USERID& userId,
                                   const string& cycStartTime,
                                   const string& cycEndTime,
                                   vector<IdDiscount>& userDiscountVector); 
                                   
        void getDuration(const string& endDate, const string& startDate,
                        int& dayNum);                                                                                               
        
    private:
        ParamContainer<PO_CycPara>              m_PO_CycPara;
        ParamContainer<PO_SpTariff>              m_PO_SpTariff;
        ParamContainer<PO_SpDiscount>            m_PO_SpDiscount;        

        ParamContainer<PO_UserInfo>              m_PO_UserInfo;
        ParamContainer<PO_UserInfoIncre>         m_PO_UserInfoIncre;
        ParamContainer<PO_UserDiscount>          m_PO_UserDiscount;
        ParamContainer<PO_UserDiscountIncre>     m_PO_UserDiscountIncre;        
        ParamContainer<PO_UserSVCState>          m_PO_UserSVCState;
        ParamContainer<PO_UserSVCStateIncre>     m_PO_UserSVCStateIncre;        
        ParamContainer<PO_AdcECuserNum>          m_PO_AdcECuserNum;
        ParamContainer<PO_AdcECuserState>        m_PO_AdcECuserState;//zhaogc add
        ParamContainer<PO_SysDiscount>           m_PO_SysDiscount;
        
        // Add by Malone 2008-1-26 start
        ParamContainer<PO_SpDumbness>            m_PO_SpDumbness;
        // Add by Malone 2008-1-26 end        
        ParamContainer<PO_SpPunish>              m_PO_SpPunish;
        
        // Add by yih 2008-08-06 start
        ParamContainer<PO_SpRelaDiscnt>          m_PO_SpRelaDiscnt;
        ParamContainer<PO_SpRelaDiscntDetail>    m_PO_SpRelaDiscntDetail;
        // Add by yih 2008-08-06 end
        ParamContainer<PO_SysParamInfo>           m_PO_SysParamInfo;
             
    private:                
        string        m_localProvinceCode;
        bool          m_needUpdated;
                                      
};

#endif // end CMdbInfoMgr_H
