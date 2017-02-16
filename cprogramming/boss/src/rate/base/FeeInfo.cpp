#include <math.h>
#include "FeeInfo.h"
#include "TpInfo.h"

extern "C++" double pow(double,int);

CdrSeg::CdrSeg()
{
  clear();
}
void CdrSeg::clear()
{
  memset(this,0,sizeof(CdrSeg));
}
//根据时长切割话单,false 表示没有满足切割区间的段
bool CdrSeg::split(const long &r_startSecond,
                   const long &r_endSecond,
                   CdrSeg &r_splitSeg) const
{
  if (r_startSecond>=m_endSecond   || 
      r_endSecond  <=m_startSecond || 
      r_startSecond>=r_endSecond )            
    return false;
  //add by gaojf 2009-11-20 16:23:25 为了避免 m_duration=0 时
  if(r_startSecond<=m_startSecond && r_endSecond>=m_endSecond)
  { //全部包含,则整段返回
    r_splitSeg = *this;
    return true;
  }    
	if (r_startSecond>m_startSecond)
		r_splitSeg.m_startSecond = r_startSecond;
	else
		r_splitSeg.m_startSecond = m_startSecond;
		
	if (r_endSecond<m_endSecond)
		r_splitSeg.m_endSecond = r_endSecond;
	else
		r_splitSeg.m_endSecond = m_endSecond;
		
	r_splitSeg.m_duration      = r_splitSeg.m_endSecond - r_splitSeg.m_startSecond ;
	r_splitSeg.m_startDateTime = m_startDateTime + (r_splitSeg.m_startSecond - m_startSecond);		
	r_splitSeg.m_endDateTime   = r_splitSeg.m_startDateTime + r_splitSeg.m_duration;
	if(r_splitSeg.m_duration<=0) return false;
	r_splitSeg.m_startData = m_startData +
                           (r_splitSeg.m_startSecond - m_startSecond)*
                            m_data/m_duration;
  if(r_splitSeg.m_endSecond == m_endSecond)
  { //该段的最后部分要采用差方式
    r_splitSeg.m_data = m_endData-r_splitSeg.m_startData;
  }else
  {
    r_splitSeg.m_data = r_splitSeg.m_duration * m_data/m_duration;
  }
  r_splitSeg.m_endData = r_splitSeg.m_startData+r_splitSeg.m_data;

	return true;        	
}
bool CdrSeg::split(const DateTime &r_startDateTime, 
                   const DateTime &r_endDateTime, 
                   CdrSeg &r_splitSeg) const
{
  if (r_startDateTime>=m_endDateTime || 
      r_endDateTime  <=m_startDateTime || 
      r_startDateTime>=r_endDateTime )            
  	return false;
  //add by gaojf 2009-11-20 16:23:25 为了避免 m_duration=0 时
  if(r_startDateTime <= m_startDateTime && r_endDateTime>=m_endDateTime)
  { //全部包含,则整段返回
    r_splitSeg = *this;
    return true;
  }
  
  if (r_startDateTime>m_startDateTime)
	  r_splitSeg.m_startDateTime = r_startDateTime;
  else
	  r_splitSeg.m_startDateTime = m_startDateTime;
	
  if (r_endDateTime<m_endDateTime)
  	r_splitSeg.m_endDateTime = r_endDateTime;
  else
  	r_splitSeg.m_endDateTime = m_endDateTime;
  	
  r_splitSeg.m_duration = r_splitSeg.m_endDateTime - r_splitSeg.m_startDateTime;
  r_splitSeg.m_startSecond = m_startSecond + (r_splitSeg.m_startDateTime - m_startDateTime);		
  r_splitSeg.m_endSecond   = r_splitSeg.m_startSecond + r_splitSeg.m_duration;

	if(r_splitSeg.m_duration<=0) return false;
	r_splitSeg.m_startData = m_startData +
                           (r_splitSeg.m_startSecond - m_startSecond)*
                            m_data/m_duration;
                           
  if(r_splitSeg.m_endSecond == m_endSecond)
  { //该段的最后部分要采用差方式
    r_splitSeg.m_data = m_endData-r_splitSeg.m_startData;
  }else
  {
    r_splitSeg.m_data = r_splitSeg.m_duration * m_data/m_duration;
  }
  r_splitSeg.m_endData = r_splitSeg.m_startData+r_splitSeg.m_data;
  return true; 
}  
//根据时长切割话单,false 表示没有满足切割区间的段
bool CdrSeg::split_data(const long &r_startData,const long &r_endData,
                            CdrSeg &r_splitSeg) const
{
  if (r_startData>=m_endData   || 
      r_endData  <=m_startData || 
      r_startData>=r_endData )            
    return false;
  //add by gaojf 2009-11-20 16:23:25 为了避免 m_data=0 时
  if(r_startData<=m_startData && r_endData>=m_endData)
  {//全部包含,则整段返回
    r_splitSeg = *this;
    return true;
  }
	if (r_startData>m_startData)
		r_splitSeg.m_startData = r_startData;
	else
		r_splitSeg.m_startData = m_startData;
		
	if (r_endData<m_endData)
		r_splitSeg.m_endData = r_endData;
	else
		r_splitSeg.m_endData = m_endData;
		
  r_splitSeg.m_data = r_splitSeg.m_endData-r_splitSeg.m_startData;
  if(r_splitSeg.m_data<=0) return false;
  
  r_splitSeg.m_startSecond   = m_startSecond+(r_splitSeg.m_startData-m_startData)*m_duration/m_data; 
  r_splitSeg.m_startDateTime = m_startDateTime+(r_splitSeg.m_startSecond-m_startSecond); 
  if(r_splitSeg.m_endData == m_endData)
  { //最后一段
    r_splitSeg.m_endSecond     = m_endSecond;
    r_splitSeg.m_endDateTime   = m_endDateTime;
  }else
  {
    r_splitSeg.m_endSecond  = m_startSecond+(r_splitSeg.m_endData-m_startData)*m_duration/m_data; 
    r_splitSeg.m_endDateTime= m_startDateTime+(r_splitSeg.m_endSecond-m_startSecond); 
  }
  r_splitSeg.m_duration      = r_splitSeg.m_endDateTime - r_splitSeg.m_startDateTime; 

	return true;        	
}
 
//判断两个累计ID主键
int operator==(const RateBillId &r_left,const RateBillId &r_right)
{
  return (r_left.m_userId  == r_right.m_userId  &&
          r_left.m_tpObjId == r_right.m_tpObjId &&
          r_left.m_billId  == r_right.m_billId  &&
          r_left.m_sumBeginTime == r_right.m_sumBeginTime);
}

int operator<(const RateBillId &r_left,const RateBillId &r_right)
{
  if(r_left.m_userId < r_right.m_userId) return true;
  if(r_right.m_userId < r_left.m_userId) return false;  

  if(r_left.m_tpObjId < r_right.m_tpObjId) return true;
  if(r_right.m_tpObjId < r_left.m_tpObjId) return false;  

  if(r_left.m_billId < r_right.m_billId) return true;
  if(r_right.m_billId < r_left.m_billId) return false;  

  if(r_left.m_sumBeginTime < r_right.m_sumBeginTime) return true;
  if(r_right.m_sumBeginTime < r_left.m_sumBeginTime) return false;  

  return false;
}
ostream& operator<<(ostream& r_os,const RateBillId &r_billId)
{
  r_os<<r_billId.m_userId<<" "
      <<r_billId.m_tpObjId<<" "
      <<r_billId.m_billId<<" "
      <<r_billId.m_sumBeginTime<<" "
      <<r_billId.m_sumEndTime<<" "
      <<r_billId.m_billType<<" "
      <<r_billId.m_type<<" "
      <<r_billId.m_groupId;
  return r_os;
}

//added by yinb 2011/2/18 17:07:42
RateBillInfo& RateBillInfo::operator=(const SegAddupInfo &r_right)
{
  m_billId = r_right.m_billId;
  m_objType = r_right.m_objType;
  m_unitValue = r_right.m_unitValue;
  m_unitFee = r_right.m_unitCount;
  m_billValue = r_right.m_value;
  m_maxValue = 0;
  return *this;
}

//判断两个累计ID主键
int operator==(const RateBillInfo &r_left,const RateBillInfo &r_right)
{
  return (r_left.m_billId  == r_right.m_billId);
}

//值相加
RateBillInfo& RateBillInfo::operator+=(const RateBillInfo &r_right)
{
  m_billValue += r_right.m_billValue;
  return *this;
}

void RateBillInfo::clear()
{
  memset(this,0,sizeof(RateBillInfo));
}
ostream& operator<<(ostream& r_os,const RateBillInfo &r_billInfo)
{
  r_os<<r_billInfo.m_billId<<" "
      <<r_billInfo.m_objType<<" "
      <<r_billInfo.m_unitValue<<" "
      <<r_billInfo.m_unitFee<<" "
      <<r_billInfo.m_billValue;
  return r_os;
}
SegFeeInfo::SegFeeInfo()
{
  clear();
}
SegFeeInfo::~SegFeeInfo()
{
  
}
SegFeeInfo::SegFeeInfo(const SegFeeInfo &r_right)
{
  *this = r_right;
}

SegFeeInfo& SegFeeInfo::operator=(const SegFeeInfo &r_right)
{
  m_tpObjId       = r_right.m_tpObjId       ;        
  m_feePolicyId   = r_right.m_feePolicyId   ;
  m_pRateTpInfo   = r_right.m_pRateTpInfo   ;
  m_tpId          = r_right.m_tpId          ;           
  m_startDateTime = r_right.m_startDateTime ;  
  m_endDateTime   = r_right.m_endDateTime   ;    
  m_duration      = r_right.m_duration      ;       
  m_startSecond   = r_right.m_startSecond   ;    
  m_endSecond     = r_right.m_endSecond     ;      
  m_unitTime      = r_right.m_unitTime      ;       
  m_unitFee       = r_right.m_unitFee       ; 
  m_intTag        = r_right.m_intTag    ; 
  m_unitCount     = r_right.m_unitCount     ;      
  m_value         = r_right.m_value         ;          
  m_segType       = r_right.m_segType       ;           
  m_billInfo      = r_right.m_billInfo      ;      
  m_notFinishFlag = r_right.m_notFinishFlag ;
  m_billInfoList  = r_right.m_billInfoList  ;      
  m_actMethod     = r_right.m_actMethod     ;      
  m_data          = r_right.m_data          ;      
  m_startData     = r_right.m_startData     ;      
  m_endData       = r_right.m_endData       ;      
  m_unitData      = r_right.m_unitData      ;      
  m_rateMethod    = r_right.m_rateMethod    ;
  //add for GPRS 0.X厘费率 2009,0904
  m_pricision     = r_right.m_pricision ;
  //Added By Sunam 2009/10/12 10:41:46
  m_totalFlag     = r_right.m_totalFlag;
  m_totalUnitFee  = r_right.m_totalUnitFee;
  return *this;
}

SegFeeInfo& SegFeeInfo::operator=(const CdrSeg &r_right)
{
  clear();
  m_startDateTime = r_right.m_startDateTime ;  
  m_endDateTime   = r_right.m_endDateTime   ;    
  m_duration      = r_right.m_duration      ;       
  m_startSecond   = r_right.m_startSecond   ;    
  m_endSecond     = r_right.m_endSecond     ;
  m_segType       = FEE_SEG                 ; 
  m_actMethod     = ACT_METHOD_COVER        ;
  m_data          = r_right.m_data          ;
  m_startData     = r_right.m_startData     ;
  m_endData       = r_right.m_endData       ;
  m_rateMethod    = RATE_METHOD_TIME        ;
  return *this;
}
void SegFeeInfo::clear()    
{
  m_billInfoList.clear();                           
  m_billInfo.clear();
  m_tpObjId       = 0;        
  m_feePolicyId   = 0;
  m_pRateTpInfo   = NULL;
  m_tpId          = 0;           
  m_unitTime      = 0;       
  m_unitFee       = 0; 
  m_intTag        = CARRY_TAG_NODEAL; 
  m_unitCount     = 0;      
  m_value         = 0;          
  m_segType       = 0;           
  m_notFinishFlag = 0;
  m_actMethod     = ACT_METHOD_COVER;
  
  m_data          = 0;           
  m_startData     = 0;           
  m_endData       = 0;   
  m_unitData      = 0;        
  m_rateMethod    = RATE_METHOD_TIME;
  //add for GPRS 0.X厘费率 2009,0904
  m_pricision       = 0; 
  //Added By Sunam 2009/10/12 10:42:40
  m_totalFlag     = false;
  m_totalUnitFee  = 0;
}
//设置TpId信息
void SegFeeInfo::setTpIdInfo(const T_TPOBJ_ID &r_objId,
                             const T_TP_ID    &r_feePolicyId,
                             const T_TP_ID    &r_priceId,
                             const RateTpInfo *r_rateTpInfo)
{
  m_tpObjId     = r_objId;
  m_feePolicyId = r_feePolicyId;
  m_tpId        = r_priceId;
  m_pRateTpInfo = r_rateTpInfo;
}

long SegFeeInfo::getTimes(const long &r_duration,const long &r_unitTime,const char &r_timeIntTag)const
{
  if(r_unitTime<=0) return 0;
  if(r_timeIntTag == CARRY_TAG_ADD ||
     r_timeIntTag == CARRY_TAG_NODEAL)
  { //进一
    return ((r_duration+r_unitTime-1)/r_unitTime);
  }else if(r_timeIntTag == CARRY_TAG_DEL)
  { //舍去
    return (r_duration/r_unitTime);
  }else if(r_timeIntTag == CARRY_TAG_ROUND)
  { //四舍五入
    long t_remain = r_duration%r_unitTime;
    if(t_remain*2 >=  r_unitTime)
    {
      return (r_duration/r_unitTime+1);
    }else
    {
      return (r_duration/r_unitTime);
    }
  }
  //Added By Sunam 2009/9/8 20:13:34 
  else if(r_timeIntTag == CARRY_TAG_SPECIAL)
  {
    return 2;
  }
  //Added By Sunam 2009/10/8 15:09:27
  else if(r_timeIntTag == CARRY_TAG_DOUBLE)
  {
      return ((r_duration+r_unitTime-1)/r_unitTime) * 2;
  }
  return 0;  
}

long SegFeeInfo::getDataTimes(const long &r_data,const long &r_unitData,const char &r_intTag) const
{
  if(r_unitData<=0) return 0;
  if(r_intTag == CARRY_TAG_ADD ||
     r_intTag == CARRY_TAG_NODEAL)
  { //进一
    return ((r_data+r_unitData-1)/r_unitData);
  }else if(r_intTag == CARRY_TAG_DEL)
  { //舍去
    return (r_data/r_unitData);
  }else if(r_intTag == CARRY_TAG_ROUND)
  { //四舍五入
    long t_remain = r_data%r_unitData;
    if(t_remain*2 >=  r_unitData)
    {
      return (r_data/r_unitData+1);
    }else
    {
      return (r_data/r_unitData);
    }
  }
  return 0;  
}
long SegFeeInfo::getTimes() const
{
  if(m_rateMethod == RATE_METHOD_DATA)
  {
    return getDataTimes(m_data,m_unitData,m_intTag);
  }
  else
  {
    return getTimes(m_duration,m_unitTime,m_intTag);
  }
}
 
void SegFeeInfo::compute()
{
  //Added 2009/10/10 18:35:55
  if (m_totalFlag)
  {
      long t_duration;
      if (m_intTag == '1')
      {
          t_duration = m_duration;
      }
      else if (m_intTag == '2')
      {
          t_duration = (m_duration + 59)/60;
      }
      else if (m_intTag == '3')
      {
          t_duration = (m_duration + 3599)/3600;
      }
      m_value = (t_duration*m_totalUnitFee)/m_unitTime;
      int t_times1 = 0;
      for(vector<RateBillInfo>::iterator t_itr=m_billInfoList.begin();t_itr!=m_billInfoList.end();++t_itr)
      {
          if(t_itr->m_unitValue > 0)
          {
              if(t_itr->m_objType == BILLOBJ_DURATION)
              {
                  t_times1 = getTimes(m_duration,t_itr->m_unitValue,m_intTag);
                  t_itr->m_billValue = t_times1*t_itr->m_unitValue;
                  if(m_intTag == CARRY_TAG_NODEAL && t_itr->m_billValue > m_duration)
                  {
                      t_itr->m_billValue = m_duration;
                  }
              }
              else if(t_itr->m_objType == BILLOBJ_DATA)
              {
                  t_times1 = getDataTimes(m_data,t_itr->m_unitValue,m_intTag);
                  t_itr->m_billValue = t_times1*t_itr->m_unitValue;
              }
              else if(t_itr->m_objType == BILLOBJ_TIMEFEE || t_itr->m_objType == BILLOBJ_FEELIMIT_T )
              {
                  t_times1 = getTimes(m_duration,t_itr->m_unitValue,m_intTag);
                  t_itr->m_billValue = t_times1*t_itr->m_unitFee;
              }
              else if(t_itr->m_objType == BILLOBJ_DATAFEE || t_itr->m_objType == BILLOBJ_FEELIMIT_D )
              {
                  t_times1 = getDataTimes(m_data,t_itr->m_unitValue,m_intTag);
                  t_itr->m_billValue = t_times1*t_itr->m_unitFee;
              }
          }
          else
          {
            t_itr->m_billValue = 0;
          }
      }
      if(m_rateMethod == RATE_METHOD_TIME)
          m_unitCount=getTimes(m_duration,m_unitTime,m_intTag);
      else
          m_unitCount=getDataTimes(m_data,m_unitData,m_intTag);
      return;
  }
  
  //Add by yangb For gd 1/1M At 2009-2-21 17:18:05 下午
  if (m_segType == COM_ADD_SEG)
    return;//在 computeFee_totalData() 中进行计算,这里不在重新计算
    
  if(m_rateMethod == RATE_METHOD_TIME)
    m_unitCount=getTimes(m_duration,m_unitTime,m_intTag);
  else
    m_unitCount=getDataTimes(m_data,m_unitData,m_intTag);
  
  m_value = m_unitFee * m_unitCount;
  //add for GPRS 0.X厘费率 2009,0904
  m_value = (Integer)((m_value+0)/pow((double)10,m_pricision));
  int t_times = 0;
  for(vector<RateBillInfo>::iterator t_itr=m_billInfoList.begin();
      t_itr!=m_billInfoList.end();++t_itr)
  {
    if(t_itr->m_unitValue > 0)
    {
      if(t_itr->m_objType == BILLOBJ_DURATION)
      {
        t_times = getTimes(m_duration,t_itr->m_unitValue,m_intTag);
        t_itr->m_billValue = t_times*t_itr->m_unitValue;
        if(m_intTag == CARRY_TAG_NODEAL &&
           t_itr->m_billValue > m_duration)
        {
          t_itr->m_billValue = m_duration;
        }
      }else if(t_itr->m_objType == BILLOBJ_DATA)
      { 
        t_times = getDataTimes(m_data,t_itr->m_unitValue,m_intTag);
        t_itr->m_billValue = t_times*t_itr->m_unitValue;
      }else if(t_itr->m_objType == BILLOBJ_TIMEFEE ||
               t_itr->m_objType == BILLOBJ_FEELIMIT_T )
      {
        t_times = getTimes(m_duration,t_itr->m_unitValue,m_intTag);
        t_itr->m_billValue = t_times*t_itr->m_unitFee;
      }else if(t_itr->m_objType == BILLOBJ_DATAFEE ||
               t_itr->m_objType == BILLOBJ_FEELIMIT_D )
      {
        t_times = getDataTimes(m_data,t_itr->m_unitValue,m_intTag);
        t_itr->m_billValue = t_times*t_itr->m_unitFee;
      }
    }else
    {
      t_itr->m_billValue = 0;
    }
  }
}

ostream& operator<<(ostream& r_os,const SegFeeInfo &r_segInfo)
{
  r_os<<"m_tpObjId="<<r_segInfo.m_tpObjId<<" "
      <<"m_feePolicyId="<<r_segInfo.m_feePolicyId<<" "
      <<"m_tpId="<<r_segInfo.m_tpId<<" "
      <<"m_startDateTime="<<r_segInfo.m_startDateTime<<" "
      <<"m_endDateTime="<<r_segInfo.m_endDateTime<<" "
      <<"m_duration="<<r_segInfo.m_duration<<" "
      <<"m_startSecond="<<r_segInfo.m_startSecond<<" "
      <<"m_endSecond="<<r_segInfo.m_endSecond<<" "
      <<"m_unitTime="<<r_segInfo.m_unitTime<<" "
      <<"m_data="<<r_segInfo.m_data<<" "
      <<"m_startData="<<r_segInfo.m_startData<<" "
      <<"m_endData="<<r_segInfo.m_endData<<" "
      <<"m_unitData="<<r_segInfo.m_unitData<<" "
      <<"m_rateMethod="<<r_segInfo.m_rateMethod<<" "
      <<"m_unitFee="<<r_segInfo.m_unitFee<<" "
      <<"m_intTag="<<r_segInfo.m_intTag<<" "
      <<"m_unitCount="<<r_segInfo.m_unitCount<<" "
      <<"m_value="<<r_segInfo.m_value<<" "
      <<"m_segType="<<r_segInfo.m_segType<<" "
      <<"m_notFinishFlag="<<r_segInfo.m_notFinishFlag<<" "
      <<"m_actMethod="<<r_segInfo.m_actMethod<<" "
      <<"m_billInfo="<<r_segInfo.m_billInfo<<" "
      <<"m_totalFlag="<<r_segInfo.m_totalFlag<<endl;
  for(vector<RateBillInfo>::const_iterator 
      t_itr=r_segInfo.m_billInfoList.begin();
      t_itr!=r_segInfo.m_billInfoList.end();
      ++t_itr)
  {
    r_os<<*t_itr;
  }
  return r_os;
}

FeeInfo::FeeInfo()          
{  
  m_pRateTpInfo=NULL;
  m_coverFlag = false;    
  m_segCapacity= 0;   
  clear();    
}

FeeInfo::~FeeInfo()
{
  m_pRateTpInfo=NULL;
}

FeeInfo::FeeInfo(const FeeInfo &r_right)
{
  m_coverFlag = false;    
  m_segCapacity= 0;   
  clear();    
  *this = r_right;
}

FeeInfo& FeeInfo::operator=(const FeeInfo &r_right)
{
  if(this==&r_right) return *this;
  m_pRateTpInfo=r_right.m_pRateTpInfo;  
  m_unitCount= r_right.m_unitCount;
  m_feeSum   = r_right.m_feeSum   ;
  m_freeTime = r_right.m_freeTime ; 
  m_freeData = r_right.m_freeData ; 
  m_freeFee  = r_right.m_freeFee  ;
  m_exeMethod= r_right.m_exeMethod;
  m_rateTime = r_right.m_rateTime ;
  m_coverFlag= r_right.m_coverFlag;
  m_seg      = r_right.m_seg      ;
  m_lackvalue= r_right.m_lackvalue;
  
  if(m_segCapacity < r_right.m_seg)
  { //先填满
    m_segInfoList.resize(r_right.m_seg);
    m_segCapacity = r_right.m_seg;
  }
  for(int j=0;j<r_right.m_seg;j++)
  {
    m_segInfoList[j]=r_right.m_segInfoList[j];
  }
  
  return *this;
}
void FeeInfo::clear()
{
  m_pRateTpInfo=NULL;
  m_unitCount= 0;
  m_feeSum   = 0;
  m_freeTime = 0; 
  m_freeData = 0;
  m_freeFee  = 0;
  m_seg      = 0;
  m_lackvalue= 0;
    
  m_coverFlag= false;        
}
void FeeInfo::resizeSegList(const int &r_size)
{
  if(m_segCapacity>=r_size) return;
  m_segInfoList.resize(r_size);
  m_segCapacity = r_size;
}
//进位标志r_carryTag  0:不处理（保持现状）1:舍去  2:进位 3:四舍五入 
void FeeInfo::computeFee(const char &r_carryTag)
{
  m_unitCount=0,m_feeSum=0,m_freeTime=0,m_freeData=0,m_freeFee = 0,m_lackvalue=0;
  m_limitFee =0;
  for (int i=0; i< m_seg; i++)
  {
    m_segInfoList[i].compute();
    m_unitCount+=m_segInfoList[i].m_unitCount;
    m_feeSum   +=m_segInfoList[i].m_value;
    m_lackvalue +=m_segInfoList[i].m_value%10;
    for(vector<RateBillInfo>::const_iterator 
        t_itr=m_segInfoList[i].m_billInfoList.begin();
        t_itr!=m_segInfoList[i].m_billInfoList.end();
        ++t_itr)
    {
      if(t_itr->m_objType == BILLOBJ_DURATION )
      {
        m_freeTime += t_itr->m_billValue;
      }else if(t_itr->m_objType == BILLOBJ_TIMEFEE ||
               t_itr->m_objType == BILLOBJ_DATAFEE  )     
      {  
        m_freeFee += t_itr->m_billValue;
      }else if(t_itr->m_objType == BILLOBJ_DATA )//流量
      { 
        m_freeData += t_itr->m_billValue;
      }else if(t_itr->m_objType == BILLOBJ_FEELIMIT_T  ||
               t_itr->m_objType == BILLOBJ_FEELIMIT_D  )
      {
        m_limitFee += t_itr->m_billValue;
      }
    }
  }
  switch(r_carryTag)
  {
    case CARRY_TAG_NODEAL: //0:不处理（保持现状）
      break;
    case CARRY_TAG_DEL   : //1:舍去 
      m_feeSum -= m_feeSum%10;
      //Added By Sunam 2009/10/26 22:35:01最后一个分段的补偿值
      m_lackvalue -= m_lackvalue%10;
      break;
    case CARRY_TAG_ADD   : //2:进位 
      if(m_feeSum%10 > 0 )
      {
        m_feeSum += 10-m_feeSum%10;
      }
      //Added By Sunam 2009/10/26 22:35:01最后一个分段的补偿值
      if(m_lackvalue%10 > 0 )
      {
        m_lackvalue += 10-m_lackvalue%10;
      }
      break;
    case CARRY_TAG_ROUND : //3:四舍五入
      if(m_feeSum%10 > 4 )
      {
        m_feeSum += 10-m_feeSum%10;
      }else
      {
        m_feeSum -= m_feeSum%10;
      }
      //Added By Sunam 2009/10/26 22:35:01最后一个分段的补偿值
      if(m_lackvalue%10 > 4 )
      {
        m_lackvalue += 10-m_lackvalue%10;
      }else
      {
        m_lackvalue -= m_lackvalue%10;
      }
      break;
    default:
      break;
  }
}
//add by gaojf 2009-2-23 21:34
bool SegFeeInfo::cheaper(const SegFeeInfo &r_right) const
{
  if(r_right.m_unitFee<=0) return false;
  if(m_rateMethod == RATE_METHOD_TIME )
  {
  	//Added By Sunam 2009/12/2 21:47:49 吉林特殊处理
  	#ifdef __PROV090__
		if(r_right.m_rateMethod == RATE_METHOD_TIME)
		{
			//return (m_unitFee*r_right.m_unitTime < r_right.m_unitFee*m_unitTime);
			if(m_unitTime==0 || r_right.m_unitTime==0)
			{
				return (r_right.m_unitTime!=0);
			}
			else
			{
				return ((Integer)((m_unitFee*((m_duration+m_unitTime-1)/m_unitTime)+0)/pow((double)10,m_pricision)) < 
			    	    (Integer)((r_right.m_unitFee*((r_right.m_duration+r_right.m_unitTime-1)/r_right.m_unitTime)+0)/pow((double)10,m_pricision)));
			}
		}
		else
		{
			return true;
		}
	#endif
    
    if(r_right.m_rateMethod == RATE_METHOD_TIME)
    {
      //Modifiy for GPRS 0.X厘费率 2009,0904
      //return (m_unitFee*r_right.m_unitTime < r_right.m_unitFee*m_unitTime);
      return ((Integer)((m_unitFee*((m_duration+m_unitTime-1)/m_unitTime)+0)/pow((double)10,m_pricision)) < 
              (Integer)((r_right.m_unitFee*((r_right.m_duration+r_right.m_unitTime-1)/r_right.m_unitTime)+0)/pow((double)10,m_pricision)));
    }else
    {
      return true;
    }
  }else if(m_rateMethod==RATE_METHOD_DATA)
  {
  	#ifdef __PROV090__
  		if(r_right.m_rateMethod == RATE_METHOD_DATA)
    	{
    	  //return (m_unitFee*r_right.m_unitData < r_right.m_unitFee*m_unitData);
    	  if(m_unitData==0 || r_right.m_unitData==0)
    	  {
			return (r_right.m_unitData!=0); 
    	  }
    	   //Modifiy for GPRS 0.X厘费率 2009,0904
    	  //return (m_unitFee*r_right.m_unitData < r_right.m_unitFee*m_unitData);
    	  return ((Integer)((m_unitFee*((m_data+m_unitData-1)/m_unitData)+0)/pow((double)10,m_pricision)) <
    	          (Integer)((r_right.m_unitFee*((r_right.m_data+r_right.m_unitData-1)/r_right.m_unitData)+0)/pow((double)10,m_pricision)));
    	}else
    	{
    	  return true;
    	}
	#else
    if(r_right.m_rateMethod == RATE_METHOD_DATA)
    {
      //Modifiy for GPRS 0.X厘费率 2009,0904
      //return (m_unitFee*r_right.m_unitData < r_right.m_unitFee*m_unitData);
      return ((Integer)((m_unitFee*((m_data+m_unitData-1)/m_unitData)+0)/pow((double)10,m_pricision)) <
              (Integer)((r_right.m_unitFee*((r_right.m_data+r_right.m_unitData-1)/r_right.m_unitData)+0)/pow((double)10,m_pricision)));
    }else
    {
      return true;
    }
    #endif
  }
  return true;
}
//add by gaojf 2009-3-11 17:17
//r_exeFlag为执行方式 0 普通 1强制 2 部分强制 3整条比较
bool SegFeeInfo::needUpdate(const SegFeeInfo &r_right,const int &r_exeFlag)
{
  if(m_tpObjId == r_right.m_tpObjId &&
     m_feePolicyId == r_right.m_feePolicyId &&
     m_tpId == r_right.m_tpId)
  {
    return cheaper(r_right);
  }else if(r_exeFlag==1) 
  {
    return true;
  }else if(r_exeFlag==2)
  {
    return (r_right.m_unitFee>0); 
  }else if(r_exeFlag==3) 
  { //整条比较
    if(r_right.m_unitFee==0 && r_right.m_segType!=FEE_SEG)
    { //如果前面政策是累计方式赠送的,则不需更新
      return false;
    }else
    {
      return true;
    }
  }else
  {
    return cheaper(r_right);
  }
}

//根据一段的费用信息更新总费用信息
//r_exeFlag为执行方式 0 普通 1强制 2 部分强制
void FeeInfo::updateSegFeeInfo(const SegFeeInfo &r_segFeeInfo,
                               //const bool       &r_exeFlag)
                               const int        &r_exeFlag)
{
  int i;
  if(m_seg==0)
  { //为空，直接追加
    resizeSegList(m_seg+1);
    m_segInfoList[m_seg]=r_segFeeInfo;
    m_seg++;
  }else if(m_segInfoList[m_seg-1].m_endDateTime <  r_segFeeInfo.m_startDateTime ||
           m_segInfoList[m_seg-1].m_endDateTime == r_segFeeInfo.m_startDateTime &&
           m_segInfoList[m_seg-1].m_endData     <= r_segFeeInfo.m_startData)
  { //在最后追加
    resizeSegList(m_seg+1);
    m_segInfoList[m_seg]=r_segFeeInfo;
    m_seg++;
  }else
  {
    vector<SegFeeInfo>  t_midSegFeeInfo;
    SegFeeInfo  t_segFeeInfo = r_segFeeInfo;
    SegFeeInfo  t_spSegFeeInfo;
    for(i=0;i<m_seg;)
    {
      if(t_segFeeInfo.m_duration<=0 && t_segFeeInfo.m_data<=0)
      { //需要更新已经全部更新完毕
        t_midSegFeeInfo.push_back(m_segInfoList[i]);
        ++i;
        continue;
      }else if(m_segInfoList[i].m_endDateTime <  t_segFeeInfo.m_startDateTime ||
               m_segInfoList[i].m_endDateTime == t_segFeeInfo.m_startDateTime &&
               m_segInfoList[i].m_endData     <= t_segFeeInfo.m_startData )
      {   //要更新的段在该段之后
        t_midSegFeeInfo.push_back(m_segInfoList[i]);
        ++i;
        continue;
      }else if(m_segInfoList[i].m_startDateTime >  t_segFeeInfo.m_endDateTime ||
               m_segInfoList[i].m_startDateTime == t_segFeeInfo.m_endDateTime &&
               m_segInfoList[i].m_startData     >= t_segFeeInfo.m_endData)
      { //需要更新的段，正好缺失
        t_midSegFeeInfo.push_back(t_segFeeInfo);
        t_segFeeInfo.m_duration=0;t_segFeeInfo.m_data=0;//作为结束标记
        continue;
      }else
      {  //交叉
        if(m_segInfoList[i].m_startDateTime == t_segFeeInfo.m_startDateTime &&
           m_segInfoList[i].m_startData     == t_segFeeInfo.m_startData)
        {
          if(m_segInfoList[i].m_endDateTime == t_segFeeInfo.m_endDateTime &&
             m_segInfoList[i].m_endData     == t_segFeeInfo.m_endData)
          {   //该段全部更新
            //if(m_segInfoList[i].m_unitFee <= 0&&
            //if(t_segFeeInfo.cheaper(m_segInfoList[i])==false &&
            //    r_exeFlag == false)
            if(t_segFeeInfo.needUpdate(m_segInfoList[i],r_exeFlag)==false)
            { //非强制执行,不覆盖原来的已经免费的段
              t_midSegFeeInfo.push_back(m_segInfoList[i]);
            }else
            { 
              t_midSegFeeInfo.push_back(t_segFeeInfo);
            }
            i++;
            t_segFeeInfo.m_duration=0;t_segFeeInfo.m_data=0;//作为结束标记
            continue;
          }else if(m_segInfoList[i].m_endDateTime > t_segFeeInfo.m_endDateTime ||
                   m_segInfoList[i].m_endDateTime ==t_segFeeInfo.m_endDateTime &&
                   m_segInfoList[i].m_endData     > t_segFeeInfo.m_endData)
          {   //该段需要更新，但是结束时间在该段中间
            t_spSegFeeInfo = m_segInfoList[i];
            t_spSegFeeInfo.m_endDateTime = t_segFeeInfo.m_endDateTime;
            t_spSegFeeInfo.m_endSecond   = t_segFeeInfo.m_endSecond;
            t_spSegFeeInfo.m_duration    = t_spSegFeeInfo.m_endDateTime - 
                                           t_spSegFeeInfo.m_startDateTime;
            t_spSegFeeInfo.m_endData     = t_segFeeInfo.m_endData;
            t_spSegFeeInfo.m_data        = t_spSegFeeInfo.m_endData-t_spSegFeeInfo.m_startData;
            t_spSegFeeInfo.compute();
            //if(t_spSegFeeInfo.m_unitFee <= 0&&
            //   r_exeFlag == false)
            //if(t_spSegFeeInfo.m_unitFee <= t_segFeeInfo.m_unitFee &&
            //if(t_segFeeInfo.cheaper(m_segInfoList[i])==false &&
            //    r_exeFlag == false)
            if(t_segFeeInfo.needUpdate(m_segInfoList[i],r_exeFlag)==false)
            {
              t_midSegFeeInfo.push_back(t_spSegFeeInfo);
            }
            else
            {
              t_midSegFeeInfo.push_back(t_segFeeInfo);
            }
            m_segInfoList[i].m_startDateTime = t_spSegFeeInfo.m_endDateTime;
            m_segInfoList[i].m_startSecond   = t_spSegFeeInfo.m_endSecond;
            m_segInfoList[i].m_duration      = m_segInfoList[i].m_endDateTime - 
                                               m_segInfoList[i].m_startDateTime;
            m_segInfoList[i].m_startData     = t_spSegFeeInfo.m_endData;
            m_segInfoList[i].m_data          = m_segInfoList[i].m_endData-
                                               m_segInfoList[i].m_startData;
            t_segFeeInfo.m_duration=0;t_segFeeInfo.m_data=0;//作为结束标记
            m_segInfoList[i].compute();
            continue;
          }else //if(m_segInfoList[i].m_endDateTime < t_segFeeInfo.m_endDateTime ||
                //   m_segInfoList[i].m_endDateTime ==t_segFeeInfo.m_endDateTime &&
                //   m_segInfoList[i].m_endData     < t_segFeeInfo.m_endData)
          {   //全部更新，但是结束时间在该段之后，后面段也要更新
            t_spSegFeeInfo = t_segFeeInfo;
            t_spSegFeeInfo.m_endDateTime = m_segInfoList[i].m_endDateTime;
            t_spSegFeeInfo.m_endSecond   = m_segInfoList[i].m_endSecond;
            t_spSegFeeInfo.m_duration    = t_spSegFeeInfo.m_endDateTime - 
                                           t_spSegFeeInfo.m_startDateTime;
            t_spSegFeeInfo.m_endData     = m_segInfoList[i].m_endData;
            t_spSegFeeInfo.m_data        = t_spSegFeeInfo.m_endData-t_spSegFeeInfo.m_startData;
            t_spSegFeeInfo.compute();
            //if(m_segInfoList[i].m_unitFee <= 0&&
            //   r_exeFlag == false)
            //if(m_segInfoList[i].m_unitFee <= t_segFeeInfo.m_unitFee &&
            //if(t_segFeeInfo.cheaper(m_segInfoList[i])==false &&
            //    r_exeFlag == false)
            if(t_segFeeInfo.needUpdate(m_segInfoList[i],r_exeFlag)==false)
            {
              t_midSegFeeInfo.push_back(m_segInfoList[i]);
            }
            else
            {
              t_midSegFeeInfo.push_back(t_spSegFeeInfo);
            }
            t_segFeeInfo.m_startDateTime= t_spSegFeeInfo.m_endDateTime;
            t_segFeeInfo.m_startSecond  = t_spSegFeeInfo.m_endSecond;
            t_segFeeInfo.m_duration     = t_segFeeInfo.m_endDateTime - 
                                          t_segFeeInfo.m_startDateTime;
            t_segFeeInfo.m_startData    = t_spSegFeeInfo.m_endData;
            t_segFeeInfo.m_data         = t_segFeeInfo.m_endData-t_segFeeInfo.m_startData;
            t_segFeeInfo.compute();
            ++i;
            continue;
          }
        }else if(m_segInfoList[i].m_startDateTime < t_segFeeInfo.m_startDateTime ||
                 m_segInfoList[i].m_startDateTime ==t_segFeeInfo.m_startDateTime &&
                 m_segInfoList[i].m_startData     < t_segFeeInfo.m_startData)
        {   //该段需要更新，并且要更新的起始时间在该段中间
          t_spSegFeeInfo = m_segInfoList[i];
          t_spSegFeeInfo.m_endDateTime = t_segFeeInfo.m_startDateTime;
          t_spSegFeeInfo.m_endSecond   = t_segFeeInfo.m_startSecond;
          t_spSegFeeInfo.m_duration    = t_spSegFeeInfo.m_endDateTime - 
                                         t_spSegFeeInfo.m_startDateTime;
          t_spSegFeeInfo.m_endData     = t_segFeeInfo.m_startData;
          t_spSegFeeInfo.m_data        = t_spSegFeeInfo.m_endData-t_spSegFeeInfo.m_startData;
          t_spSegFeeInfo.compute();
          t_midSegFeeInfo.push_back(t_spSegFeeInfo);
          m_segInfoList[i].m_startDateTime = t_segFeeInfo.m_startDateTime;
          m_segInfoList[i].m_startSecond   = t_segFeeInfo.m_startSecond;
          m_segInfoList[i].m_duration      = m_segInfoList[i].m_endDateTime - 
                                             m_segInfoList[i].m_startDateTime;
          m_segInfoList[i].m_startData     = t_segFeeInfo.m_startData;
          m_segInfoList[i].m_data          = m_segInfoList[i].m_endData-
                                             m_segInfoList[i].m_startData;
          m_segInfoList[i].compute();                                   
          continue;
        }else //if(m_segInfoList[i].m_startDateTime > t_segFeeInfo.m_startDateTime ||
             //    m_segInfoList[i].m_startDateTime== t_seg