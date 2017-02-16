/*功能： 为实时扣费增加。定义记录中间套餐批价结果结构
 */
#ifndef TP_RESULT_INFO_200407
#define TP_RESULT_INFO_200407
#include "FeeInfo.h"

typedef struct TpRentValue
{
    int   m_tpId;   //套餐代码，30000000+营帐套餐代码
    long  m_value;  //套餐月租实扣费用，单位：厘
}TpRentValue; //TpRentValue  为OCS而增加

typedef struct TpRentItem
{
    char  m_tpId[4+1]; //套餐代码xxxx，不足4位左补0
    int   m_renFee;    //扣取的套餐包月费
}TpRentItem;

typedef struct CurrentTPItem
{
    T_TP_ID     m_tpId;
    int         m_tpType; //0 原始政策 1 一次批价 2二次批价
    char        m_chargeType; //Y 实时扣费，N 非实时扣费
    char        m_method;//作用类型，0 初始值 1 共同作用，2 强制作用 2004-11-23 
}CurrentTPItem;
    
typedef struct TpResultInfoFee
{
    T_TP_ID     m_tpId;
    int         m_tpType; //0 原始政策 1 一次批价 2二次批价
    char        m_method; //作用类型，0 初始值 1 共同作用，2 强制作用
    char        m_chargeType; //Y 实时扣费，N 非实时扣费
    int         m_rentFee;//作用结果差额
    FeeInfoGsm  m_feeInfo;//政策作用结果信息
    TpResultInfoFee()
    {
      m_tpId   = 0;
      m_tpType = 0;
      m_rentFee= 0;
      m_feeInfo.clear();
    }
}TpResultInfoFee;
class TpResultInfo  
{
    public:
        bool                    m_baseFlag;      //是否有一次批价结果
        TpResultInfoFee         m_baseFeeInfo;   //一次批价结果或原始费用
        vector<TpResultInfoFee> m_tpFeeInfoList; //二次批价结果
        
    public:
        TpResultInfo()
        {
            clear();
        }
        virtual ~TpResultInfo() {m_tpFeeInfoList.clear();}
        void clear()
        {
            m_baseFlag = false;
            m_baseFeeInfo.m_method   = '0';
            m_tpFeeInfoList.clear();
        }
        friend ostream& operator<<(ostream& os,TpResultInfo& feeinfoList)
        {
            if (feeinfoList.m_baseFlag == false)
                return os;
            os<<"=========TpId:"<<feeinfoList.m_baseFeeInfo.m_tpId<<
                      " tpType="<<feeinfoList.m_baseFeeInfo.m_tpType<<
                     " m_method"<<feeinfoList.m_baseFeeInfo.m_method<<
                  "============"<< endl;
            
            os<<feeinfoList.m_baseFeeInfo.m_feeInfo<<endl;
            
            for (vector<TpResultInfoFee>::iterator  itr = feeinfoList.m_tpFeeInfoList.begin();
                 itr != feeinfoList.m_tpFeeInfoList.end();itr++)
            {
                os<<"==TpId:"       <<itr->m_tpId
                  <<" tpType="      <<itr->m_tpType
                  <<" m_method="    <<itr->m_method
                  << " 实扣标识 "   <<itr->m_chargeType
                  <<" m_rentFee=  " <<itr->m_rentFee 
                  <<"========"      << endl;
                
                os<<itr->m_feeInfo<<endl;
            }
            return os;
        }
        void generateTpRemark(const int &feeField,char *tpRemark)
        {
            char tmpStr[30];int i;
            if (m_baseFlag == false)    return; //没有对应政策作用
            if (m_baseFeeInfo.m_tpType >= 2 )
            {
                for (i=0;i<m_baseFeeInfo.m_feeInfo.m_seg;i++)
                {
                    if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_tpID == int(m_baseFeeInfo.m_tpId))
                    {
                        //设置作用结果
                        sprintf(tmpStr,"%08d|%d|%d|%c,",int(m_baseFeeInfo.m_tpId),
                                feeField,m_baseFeeInfo.m_feeInfo.m_feesum,m_baseFeeInfo.m_method);
                        if (strlen(tpRemark)+strlen(tmpStr) < 200)  //为防止情况越界
                            sprintf(tpRemark+strlen(tpRemark),"%s",tmpStr+4);
                        break;
                    }
                }
            }    
            for (vector<TpResultInfoFee>::iterator itr = m_tpFeeInfoList.begin();
                 itr != m_tpFeeInfoList.end();itr++)
            {
                for (i=0;i<itr->m_feeInfo.m_seg;i++)
                {
                    if (itr->m_feeInfo.m_segInfo[i].m_tpID == int(itr->m_tpId))
                    {
                        sprintf(tmpStr,"%08d|%d|%d|%c,",int(itr->m_tpId),feeField,
                                itr->m_feeInfo.m_feesum,itr->m_method);
                        if (strlen(tpRemark)+strlen(tmpStr) < 200)  //为防止情况越界
                            sprintf(tpRemark+strlen(tpRemark),"%s",tmpStr+4);
                        break;
                    }
                }
            }
            return ;
        }
        void push_back(const T_TP_ID &tpId,const int &tpType,const char &chargeType,const FeeInfoGsm &feeInfo,const char &tpMethod)
        {
            TpResultInfoFee  tmpInfoFee;
            tmpInfoFee.m_tpId   = tpId;
            tmpInfoFee.m_tpType = tpType;
            tmpInfoFee.m_chargeType = chargeType;
            tmpInfoFee.m_feeInfo= feeInfo;
            tmpInfoFee.m_method = tpMethod; //2004-11-23 
            
            if ((tpType <= 1)||(m_baseFlag == false))//一次批价结果或原始费用
            {
                m_baseFlag    = true;
                m_baseFeeInfo = tmpInfoFee;
                m_tpFeeInfoList.clear();
            }
            else //二次批价结果
            {
                m_tpFeeInfoList.push_back(tmpInfoFee);
            }
        }
        
        //计算各seg段的实扣月租
        void adjustFeeInfoResult_new(list<TP> &r_thetpList)
        {
            if (m_tpFeeInfoList.size()<=0)
                return;


            vector<TpResultInfoFee>::iterator t_itrTpResult = m_tpFeeInfoList.end()-1;
            int t_durationAll = 0;
            int i = 0;
            
            
            list<TP>::iterator t_itrTP;
            
            for (i = 0; i < t_itrTpResult->m_feeInfo.m_seg; i++)
            {
                //根据r_thetpList置该seg中tpID的是否实扣标志
                for (t_itrTP = r_thetpList.begin();t_itrTP!=r_thetpList.end();t_itrTP++)
                {
                    if (t_itrTP->m_tpId == t_itrTpResult->m_feeInfo.m_segInfo[i].m_tpID)
                    {
                        t_itrTpResult->m_feeInfo.m_segInfo[i].m_chargeType = t_itrTP->m_chargeType;
                        break;
                    }
                }
                
                //记录各seg的总时长
                t_durationAll  += t_itrTpResult->m_feeInfo.m_segInfo[i].m_duration;
                
                //利用该seg自身的资费信息初置实扣月租费用
                if (t_itrTpResult->m_feeInfo.m_segInfo[i].m_chargeType == YES_FLAG)
                {
                    t_itrTpResult->m_feeInfo.m_segInfo[i].m_rentFee = t_itrTpResult->m_feeInfo.m_segInfo[i].m_unitFee * t_itrTpResult->m_feeInfo.m_segInfo[i].m_unitCount;
                }
            }
            
            //对于初置实扣月租费用为0的,按该seg在总seg中的时长比例摊分基本资费,做为该seg应实扣月租
            for (i = 0; i < t_itrTpResult->m_feeInfo.m_seg; i++)
            {
                if ((t_itrTpResult->m_feeInfo.m_segInfo[i].m_chargeType == YES_FLAG) && (t_itrTpResult->m_feeInfo.m_segInfo[i].m_rentFee == 0))
                {                        
                    t_itrTpResult->m_feeInfo.m_segInfo[i].m_rentFee = m_baseFeeInfo.m_feeInfo.m_feesum * t_itrTpResult->m_feeInfo.m_segInfo[i].m_duration/t_durationAll;
                }
            }
        }

        
        void adjustFeeInfoResult()
        {
            vector<TpResultInfoFee>::iterator itr;
            SegFeeInfoGsm tmpSeg1,tmpSeg2;
            int duration,k;
            
            if (m_tpFeeInfoList.size()<=0)
                return;
            
            for (itr = m_tpFeeInfoList.end();;itr--)
            {   
                
                if (itr == m_tpFeeInfoList.begin())
                {
                    cout <<__FILE__<<__LINE__<<endl;
                    cout << "itr == m_tpFeeInfoList.begin()" << endl;
                    itr->m_feeInfo.updateFeeInfo();
                    break;
                }
                
                if (itr == m_tpFeeInfoList.end())   continue;
                

                //第一个二次批价结果不需对前面的做调整
                for (int i = 0; i < itr->m_feeInfo.m_seg; i++)
                {
                   
                    //有该政策作用,则需判断是该段是作用在前一个套餐基础上，还是和前面套餐无关
                    if ( itr->m_feeInfo.m_segInfo[i].m_tpID == (int)(itr->m_tpId))
                    {
                        //如果是在前面套餐的基础上作用的，则无需修改前面套餐。
                        //和前面套餐无关，需将前面套餐该段的作用用该政策来替换
                        if (!(itr->m_feeInfo.m_segInfo[i].m_exeMethod == TTYPE_DISCOUNT_FEE   ||        //('1');       //费用打折                        
                              itr->m_feeInfo.m_segInfo[i].m_exeMethod == TTYPE_ADD_NEW_FEE    ||        //('4');       //增加费用 如：整条话单增加0.2元  
                              itr->m_feeInfo.m_segInfo[i].m_exeMethod == TTYPE_SEND_FEE       ||        //('6');       //赠送费用 如：赠送30元前         
                              itr->m_feeInfo.m_segInfo[i].m_exeMethod == TTYPE_ADD_NEW_FEERATE))        //('5');       //增加费率 如：每分钟增加0.2元    
                        {
                            for (vector<TpResultInfoFee>::iterator itr1 = m_tpFeeInfoList.begin();
                                 itr1 != itr;itr1++)
                            {
                                
                                 
                                //设置tmpSeg1；用itr->m_feeInfo.m_segInfo[i]对应的一次批价结果m_baseFeeInfo
                                //相应的费用段来替换
                                tmpSeg1 = itr->m_feeInfo.m_segInfo[i];
                                tmpSeg1.m_tpID      = m_baseFeeInfo.m_tpId;
                                tmpSeg1.m_unitCount = 1;
                                tmpSeg1.m_unitTime  = tmpSeg1.m_duration;
                                tmpSeg1.m_exeMethod = '0';
                                tmpSeg1.m_type = FEE_SEG;
                                tmpSeg1.m_value = tmpSeg1.m_unitFee = getFeeSumFromBaseFeeInfo(tmpSeg1.m_startDateTime,tmpSeg1.m_endDateTime);
                                tmpSeg1.m_freetimeId = tmpSeg1.m_freeValue = 0;
                                
                                //用tmpSeg1政策来替换
                                for(int j = 0; j < itr1->m_feeInfo.m_seg; j++)
                                {
                                    
                                    if ((itr1->m_feeInfo.m_segInfo[j].m_startDateTime >= tmpSeg1.m_endDateTime)||
                                        (itr1->m_feeInfo.m_segInfo[j].m_endDateTime   <= tmpSeg1.m_startDateTime))
                                        continue;
                                        
                                    if ((itr1->m_feeInfo.m_segInfo[j].m_startDateTime == tmpSeg1.m_startDateTime)&&
                                        (itr1->m_feeInfo.m_segInfo[j].m_endDateTime   == tmpSeg1.m_endDateTime))
                                    {   //该段刚好全部替换
                                        itr1->m_feeInfo.m_segInfo[j] = tmpSeg1;
                                        break;
                                    }
                                    
                                    //以下应该是交叉的情况
                                    if (itr1->m_feeInfo.m_segInfo[j].m_startDateTime < tmpSeg1.m_startDateTime)
                                    {
                                        duration = itr1->m_feeInfo.m_segInfo[j].m_duration;
                                        //则需要保留从itr1->m_feeInfo.m_segInfo[j].m_startDateTime到tmpSeg1.m_startDateTime的段信息
                                        tmpSeg2 = itr1->m_feeInfo.m_segInfo[j];
                                        tmpSeg2.m_endDateTime = tmpSeg1.m_startDateTime;
                                        tmpSeg2.m_duration    = tmpSeg2.m_endDateTime - 
                                                                tmpSeg2.m_startDateTime;
                                        if (tmpSeg2.m_unitTime > 0 && duration > 0)
                                        {
                                            tmpSeg2.m_unitCount = (tmpSeg2.m_duration+tmpSeg2.m_unitTime-1)/tmpSeg2.m_unitTime;
                                            tmpSeg2.m_freeValue = tmpSeg2.m_freeValue*tmpSeg2.m_duration/duration;
                                            tmpSeg2.m_value     = tmpSeg2.m_value*tmpSeg2.m_duration/duration;
                                        }
                                        itr1->m_feeInfo.m_segInfo[j].m_startDateTime = tmpSeg1.m_startDateTime;
                                        itr1->m_feeInfo.m_segInfo[j].m_duration      = 
                                                itr1->m_feeInfo.m_segInfo[j].m_endDateTime - itr1->m_feeInfo.m_segInfo[j].m_startDateTime;
                                        if (itr1->m_feeInfo.m_segInfo[j].m_unitTime > 0 && duration>0)
                                        {
                                            itr1->m_feeInfo.m_segInfo[j].m_unitCount = (itr1->m_feeInfo.m_segInfo[j].m_duration+
                                                    itr1->m_feeInfo.m_segInfo[j].m_unitTime-1)/itr1->m_feeInfo.m_segInfo[j].m_unitTime;
                                            itr1->m_feeInfo.m_segInfo[j].m_freeValue = itr1->m_feeInfo.m_segInfo[j].m_freeValue*
                                                                                       itr1->m_feeInfo.m_segInfo[j].m_duration/duration;
                                            itr1->m_feeInfo.m_segInfo[j].m_value     = itr1->m_feeInfo.m_segInfo[j].m_value*
                                                                                       itr1->m_feeInfo.m_segInfo[j].m_duration/duration;
                                        }
                                        for ( k = itr1->m_feeInfo.m_seg - 1; k >= j;k--)
                                        {
                                            itr1->m_feeInfo.m_segInfo[k+1] = itr1->m_feeInfo.m_segInfo[k];
                                        }
                                        itr1->m_feeInfo.m_segInfo[j] = tmpSeg2;
                                        itr1->m_feeInfo.m_seg++;
                                    }
                                    else if (itr1->m_feeInfo.m_segInfo[j].m_endDateTime < tmpSeg1.m_endDateTime)
                                    {
                                        //则itr1->m_feeInfo.m_segInfo[j]全部替换，但是后面还有需要替换的段
                                        duration = tmpSeg1.m_duration;
                                        tmpSeg2 = tmpSeg1;
                                        tmpSeg2.m_endDateTime = itr1->m_feeInfo.m_segInfo[j].m_endDateTime;
                                        tmpSeg2.m_duration    = tmpSeg2.m_endDateTime - 
                                                                tmpSeg2.m_startDateTime;
                                        if (tmpSeg2.m_unitTime > 0 && duration > 0)
                                        {
                                            tmpSeg2.m_unitCount = (tmpSeg2.m_duration+tmpSeg2.m_unitTime-1)/tmpSeg2.m_unitTime;
                                            tmpSeg2.m_freeValue = tmpSeg2.m_freeValue*tmpSeg2.m_duration/duration;
                                            tmpSeg2.m_value     = tmpSeg2.m_value*tmpSeg2.m_duration/duration;
                                        }
                                        tmpSeg1.m_startDateTime = itr1->m_feeInfo.m_segInfo[j].m_endDateTime;
                                        tmpSeg1.m_duration    = tmpSeg2.m_endDateTime - 
                                                                tmpSeg2.m_startDateTime;
                                        if (tmpSeg1.m_unitTime > 0 && duration > 0)
                                        {
                                            tmpSeg1.m_unitCount = (tmpSeg1.m_duration+tmpSeg1.m_unitTime-1)/tmpSeg1.m_unitTime;
                                            tmpSeg1.m_freeValue = tmpSeg1.m_freeValue*tmpSeg1.m_duration/duration;
                                            tmpSeg1.m_value     = tmpSeg1.m_value*tmpSeg1.m_duration/duration;
                                        }
                                        
                                        itr1->m_feeInfo.m_segInfo[j] =  tmpSeg2;
                                    }
                                    else if (itr1->m_feeInfo.m_segInfo[j].m_endDateTime > tmpSeg1.m_endDateTime)
                                    {
                                        //则itr1->m_feeInfo.m_segInfo[j]段需要部分替换，至此该政策已经处理完毕
                                        duration = itr1->m_feeInfo.m_segInfo[j].m_duration;
                                        itr1->m_feeInfo.m_segInfo[j].m_startDateTime = tmpSeg1.m_endDateTime;
                                        itr1->m_feeInfo.m_segInfo[j].m_duration      = 
                                                itr1->m_feeInfo.m_segInfo[j].m_endDateTime - itr1->m_feeInfo.m_segInfo[j].m_startDateTime;
                                        if (itr1->m_feeInfo.m_segInfo[j].m_unitTime > 0 && duration>0)
                                        {
                                            itr1->m_feeInfo.m_segInfo[j].m_unitCount = (itr1->m_feeInfo.m_segInfo[j].m_duration+
                                                    itr1->m_feeInfo.m_segInfo[j].m_unitTime-1)/itr1->m_feeInfo.m_segInfo[j].m_unitTime;
                                            itr1->m_feeInfo.m_segInfo[j].m_freeValue = itr1->m_feeInfo.m_segInfo[j].m_freeValue*
                                                                                       itr1->m_feeInfo.m_segInfo[j].m_duration/duration;
                                            itr1->m_feeInfo.m_segInfo[j].m_value     = itr1->m_feeInfo.m_segInfo[j].m_value*
                                                                                       itr1->m_feeInfo.m_segInfo[j].m_duration/duration;
                                        }
                                        for ( k = itr1->m_feeInfo.m_seg - 1; k >= j;k--)
                                        {
                                            itr1->m_feeInfo.m_segInfo[k+1] = itr1->m_feeInfo.m_segInfo[k];
                                        }
                                        itr1->m_feeInfo.m_segInfo[j] = tmpSeg1;
                                        itr1->m_feeInfo.m_seg++;
                                        break;
                                    }
                                }   //end for (itr1->m_feeInfo.m_seg)
                            }   //end for (itr1 = m_tpFeeInfoList.begin())
                        }
                    }
                }   //end for (itr->m_feeInfo.m_seg)
                
                //重新计算itr所指的套餐的作用结果的总费用
                itr->m_feeInfo.updateFeeInfo();
            
            }   //end for (itr = m_tpFeeInfoList.end())

            int preTotalFee = m_baseFeeInfo.m_feeInfo.m_feesum;
            for (itr = m_tpFeeInfoList.begin();itr != m_tpFeeInfoList.end();itr++)
            {
                if (preTotalFee > itr->m_feeInfo.m_feesum)
                {
                    itr->m_rentFee = preTotalFee - itr->m_feeInfo.m_feesum;
                }
                else
                {
                    itr->m_rentFee = 0;
                }
                preTotalFee = itr->m_feeInfo.m_feesum;
            }
        }
    private:
        int getFeeSumFromBaseFeeInfo(DateTime startDateTime,DateTime endDateTime)
        {
            int      feeSum = 0,unitTime = 0;
            DateTime tmpStartTime = startDateTime;
            for (int i = 0;i < m_baseFeeInfo.m_feeInfo.m_seg; i++)
            {
                unitTime = m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_unitTime;
                if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_startDateTime >= endDateTime)
                    return feeSum;
                if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_endDateTime   <= tmpStartTime )
                    continue;
                if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_startDateTime > tmpStartTime)
                    tmpStartTime = m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_startDateTime;
                
                if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_endDateTime >= endDateTime)
                {
                    
                    if (unitTime > 0 )
                        feeSum += m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_unitFee*
                                  ((endDateTime - tmpStartTime+unitTime -1)/unitTime);
                }
                else if (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_endDateTime < endDateTime)
                {
                    if (unitTime > 0 )
                    feeSum += m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_unitFee*
                             (m_baseFeeInfo.m_feeInfo.m_segInfo[i].m_endDateTime - tmpStartTime+unitTime-1)/unitTime;
                }
            }
            return  feeSum;
        }
};

class TpSendFee
{
    public:
        int m_tpID;     // 政策号
        int m_field;    // 费用作用域
        int m_rentFee;  // 可扣取月租费
    
    public:
        
        TpSendFee(){ m_tpID = m_field = m_rentFee = 0;}
        virtual ~TpSendFee() {}

        TpSendFee& operator=(const TpSendFee & tsf)
        {
            m_tpID      = tsf.m_tpID;
            m_field     = tsf.m_field;
            m_rentFee   = tsf.m_rentFee;
            return (*this);
        }
        
        friend bool operator < (TpSendFee &tsf1,TpSendFee &tsf2)
        { return (tsf1.m_tpID < tsf2.m_tpID);}
        friend bool operator == (TpSendFee &tsf1,TpSendFee &tsf2)
        { return (tsf1.m_tpID == tsf2.m_tpID);}
        
               
};

#endif //TP_RESULT_INFO_200407


