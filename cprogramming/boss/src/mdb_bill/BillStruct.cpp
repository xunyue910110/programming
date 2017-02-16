#include "BillStruct.h"

ostream& operator<<(ostream& os,const UserBill& po)
{
	os << " userId : " << po.m_userId << " , " << " billId : " << po.m_billId 
	   << " value : "  << po.m_value  <<" , DiscntValue:"  <<po.m_discntValue<<" , AdjustBefore:"  <<po.m_adjustBefore;
	return os;
}
int operator<(const UserBill &left,const UserBill& right)
{
  if(left.m_userId < right.m_userId ) return true;
  if(right.m_userId < left.m_userId ) return false;
  return (left.m_billId < right.m_billId);
}
int operator==(const UserBill &left,const UserBill& right)
{
  return (left.m_userId  == right.m_userId &&
          left.m_billId  == right.m_billId);
}
//Added By Sunam 2009/8/12 21:44:50
bool UserBill::equal(const UserBill &left)
{
	return (m_value == left.m_value) && 
		   (m_discntValue == left.m_discntValue) && 
		   (m_adjustBefore == left.m_adjustBefore);
}
void UserBill::operator=(const RollBackBill& right)
{
  m_userId = right.m_userId;
  m_billId = right.m_billId;
  m_value  = right.m_value;
  m_discntValue  = right.m_discntValue;
  //Added By Sunam 2009/8/12 11:27:46
  m_adjustBefore = right.m_adjustBefore;
}
UserBill& UserBill::operator +=(const UserBill &r_right)
{
  m_value       += r_right.m_value;
  m_discntValue += r_right.m_discntValue;
  //Added By Sunam 2009/8/12 11:27:46
  m_adjustBefore += r_right.m_adjustBefore;
  return *this;
}
UserBill& UserBill::operator -=(const UserBill &r_right)
{
  m_value       -= r_right.m_value;
  m_discntValue -= r_right.m_discntValue;
  //Added By Sunam 2009/8/12 11:27:46
  m_adjustBefore -= r_right.m_adjustBefore;
  return *this;
}

//add by xuf 2009-10-22 16:36:58
UserBill& UserBill::updatePart(const UserBill &r_right)
{ 
  m_discntValue  = r_right.m_discntValue;
  m_adjustBefore = r_right.m_adjustBefore;
  return *this;	
}

ostream& operator<<(ostream& os,const UserSumBill1& po)
{
	os << " userId : " << po.m_userId << " tpObjId : " << po.m_tpObjId 
	   << " , " << " billId : " << po.m_billId << " value : " << po.m_value << " maxValue : " << po.m_maxValue;
	return os;
}
int operator< (const UserSumBill1 &left,const UserSumBill1& right)
{
  if (left.m_userId  < right.m_userId) return true;
  if (right.m_userId  < left.m_userId) return false;

  if (left.m_tpObjId  < right.m_tpObjId) return true;
  if (right.m_tpObjId  < left.m_tpObjId) return false;
   
  return (left.m_billId < right.m_billId);
}

int operator==(const UserSumBill1 &left,const UserSumBill1& right)
{
  return (left.m_userId  == right.m_userId &&
          left.m_tpObjId == right.m_tpObjId &&
          left.m_billId  == right.m_billId);
}
//Added By Sunam 2009/8/12 21:44:50
bool UserSumBill1::equal(const UserSumBill1 &left)
{
	return (m_value == left.m_value);
}

void UserSumBill1::operator=(const RollBackBill& right)
{
  m_userId = right.m_userId;
  m_tpObjId= right.m_tpObjId;
  m_billId = right.m_billId;
  m_value  = right.m_value;
  //Added By Sunam 2009/8/6 16:32:25
  m_feePolicyId = right.m_feePolicyId;
  if (right.m_maxValue != 999999999999)
  {
    m_maxValue    = right.m_maxValue;
  }
}
UserSumBill1& UserSumBill1::operator +=(const UserSumBill1 &r_right)
{
  m_value       += r_right.m_value;
  //Added By Sunam 2009/8/8 11:26:51
  m_feePolicyId = r_right.m_feePolicyId;
  if (r_right.m_maxValue != 999999999999)
  {
    m_maxValue    = r_right.m_maxValue;
  }
  return *this;
}
UserSumBill1& UserSumBill1::operator -=(const UserSumBill1 &r_right)
{
  m_value       -= r_right.m_value;
  //Added By Sunam 2009/8/8 11:26:51
  m_feePolicyId = r_right.m_feePolicyId;
  if (r_right.m_maxValue != 999999999999)
  {
    m_maxValue    = r_right.m_maxValue;
  }
  return *this;
}
ostream& operator<<(ostream& os,const UserSumBill2& po)
{
	os << " userId    : " << po.m_userId 
	   << " tpObjId   : " << po.m_tpObjId 
	   << " beginTime : " << po.m_beginTime 
	   << " endTime   : " << po.m_endTime 
	   << " cycleId   : " << po.m_cycleId 
	   << " value     : " << po.m_value    
	   << " type      : " << po.m_type 
	   << " grpUserId : " << po.m_groupUserId;
	return os;
}

int operator<(const UserSumBill2 &left,const UserSumBill2& right)
{
  if (left.m_userId  < right.m_userId) return true;
  if (right.m_userId  < left.m_userId) return false;

  if (left.m_tpObjId  < right.m_tpObjId) return true;
  if (right.m_tpObjId  < left.m_tpObjId) return false;

  if (left.m_beginTime  < right.m_beginTime) return true;
  if (right.m_beginTime  < left.m_beginTime) return false;

  if (left.m_cycleId  < right.m_cycleId) return true;
  if (right.m_cycleId  < left.m_cycleId) return false;
   
  return (left.m_billId < right.m_billId);
}

int operator==(const UserSumBill2 &left,const UserSumBill2& right)
{
  return (left.m_userId     == right.m_userId &&
          left.m_tpObjId    == right.m_tpObjId &&
          left.m_beginTime  == right.m_beginTime &&
          left.m_cycleId    == right.m_cycleId &&
          left.m_billId     == right.m_billId    );
}

//Added By Sunam 2009/8/12 21:44:50
bool UserSumBill2::equal(const UserSumBill2 &left)
{
	return (m_value == left.m_value);
}

void UserSumBill2::operator=(const RollBackBill& right)
{
  m_userId      = right.m_userId;
  m_tpObjId     = right.m_tpObjId;
  m_beginTime   = right.m_beginTime;
  m_endTime     = right.m_endTime;
  m_cycleId     = right.m_billCycle;
  m_billId      = right.m_billId;
  m_value       = right.m_value;
  m_type        = right.m_type;
  m_groupUserId = right.m_groupUserId;
  //Added By Sunam 2009/8/6 16:32:25
  m_feePolicyId = right.m_feePolicyId;
  if (right.m_maxValue != 999999999999)
  {
    m_maxValue    = right.m_maxValue;
  }
}
UserSumBill2& UserSumBill2::operator +=(const UserSumBill2 &r_right)
{
  m_value       += r_right.m_value;
  //Added By Sunam 2009/8/8 11:26:51
  m_feePolicyId = r_right.m_feePolicyId;
  if (r_right.m_maxValue != 999999999999)
  {
    m_maxValue    = r_right.m_maxValue;
  }
  return *this;
}
UserSumBill2& UserSumBill2::operator -=(const UserSumBill2 &r_right)
{
  m_value       -= r_right.m_value;
  //Added By Sunam 2009/8/8 11:26:51
  m_feePolicyId = r_right.m_feePolicyId;
  if (r_right.m_maxValue != 999999999999)
  {
    m_maxValue    = r_right.m_maxValue;
  }
  return *this;
}

ostream& operator<<(ostream& os,const AccountBill& po)
{
	os << " accountId : " << po.m_accountId << " userId : " << po.m_userId  
	   << " , " << " billId : " << po.m_billId << " value : " << po.m_value
	   << " , " << " DiscntValue : "<<po.m_discntValue
	   << " , " << " A_discnt : "<<po.m_a_discnt
	   << " , " << " B_discnt : "<<po.m_b_discnt
	   << " , " << " A_adjustBefore : "<<po.m_acct_adjustBefore
	   << " , " << " userGrpDiscnt : "<<po.m_userGrpDiscnt
	   << " , " << " userAdjust : "<<po.m_userAdjust
	   << " , " << " m_areaCode : "<<po.m_areaCode;;
	return os;
}


int operator==(const AccountBill &left,const AccountBill& right)
{
  return (left.m_accountId == right.m_accountId &&
          left.m_userId    == right.m_userId &&
          left.m_billId    == right.m_billId);
}

//Added By Sunam 2009/8/12 21:44:50
bool AccountBill::equal(const AccountBill &left)
{
	return (m_value == left.m_value) &&
		   (m_discntValue == left.m_discntValue) &&
		   (m_a_discnt == left.m_a_discnt) &&
		   (m_b_discnt == left.m_b_discnt)&&
		   (m_acct_adjustBefore == left.m_acct_adjustBefore);
}

void AccountBill::operator=(const RollBackBill& r_right)
{
  m_accountId   = r_right.m_accountId;
  m_userId      = r_right.m_userId;
  m_billId      = r_right.m_billId;
  m_value       = r_right.m_value;
  m_discntValue = r_right.m_discntValue;
  m_a_discnt    = r_right.m_a_discnt;
  m_b_discnt    = r_right.m_b_discnt;
  m_acct_adjustBefore = r_right.m_adjustBefore;
  m_areaCode    = r_right.m_areaCode;
  m_userGrpDiscnt = r_right.m_userGrpDiscnt;
  m_userAdjust   = r_right.m_userAdjust;
}
AccountBill& AccountBill::operator +=(const AccountBill &r_right)
{
  m_value            += r_right.m_value;
  m_discntValue      += r_right.m_discntValue;
  m_a_discnt         += r_right.m_a_discnt;
  m_b_discnt         += r_right.m_b_discnt;
 	m_acct_adjustBefore += r_right.m_acct_adjustBefore; 
  m_userGrpDiscnt    += r_right.m_userGrpDiscnt;
  m_userAdjust       += r_right.m_userAdjust;
  return *this;
}
AccountBill& AccountBill::operator -=(const AccountBill &r_right)
{
  m_value            -= r_right.m_value;
  m_discntValue      -= r_right.m_discntValue;
  m_a_discnt         -= r_right.m_a_discnt;
  m_b_discnt         -= r_right.m_b_discnt;
  m_acct_adjustBefore-= r_right.m_acct_adjustBefore; 
  m_userGrpDiscnt    -= r_right.m_userGrpDiscnt;
  m_userAdjust       -= r_right.m_userAdjust;
  return *this;
}
ostream& operator<<(ostream& os,const RateDiscntBill& po)
{
	os << " userId : " << po.m_userId<< " accountId : " << po.m_accountId   
	   << " , " << " billId : " << po.m_billId << " value : " << po.m_value;
	return os;
}


int operator==(const RateDiscntBill &left,const RateDiscntBill& right)
{
  return (left.m_accountId == right.m_accountId &&
          left.m_userId    == right.m_userId &&
          left.m_billId    == right.m_billId);
}

//Added By Sunam 2009/8/12 21:44:50
bool RateDiscntBill::equal(const RateDiscntBill &left)
{
	return (m_value == left.m_value);
}

void RateDiscntBill::operator=(const RollBackBill& right)
{
  m_accountId= right.m_accountId;
  m_userId   = right.m_userId;
  m_billId   = right.m_billId;
  m_value    = right.m_value;
}


void UserSumBill3::operator=(const RollBackBill& right)//回滚账单处理
{
 m_userId      = right.m_userId;
 m_tpObjId     = right.m_tpObjId;
 m_spCode      = right.m_spCode; 
 m_spBizCode   = right.m_spBizCode;
 m_beginTime   = right.m_beginTime;
 m_endTime     = right.m_endTime;
 m_cycleId     = right.m_billCycle;
 m_value       = right.m_value;
}
    
RateDiscntBill& RateDiscntBill::operator +=(const RateDiscntBill &r_right)
{
  m_value   += r_right.m_value;
  return *this;
}
RateDiscntBill& RateDiscntBill::operator -=(const RateDiscntBill &r_right)
{
  m_value   -= r_right.m_value;
  return *this;
}

ostream& operator<<(ostream& os,const RollBackBill& po)
{
	os << " channelNo : " << po.m_channelNo << " userId : " << po.m_userId;
	os << " billType : " << po.m_billType << " , " << " billCycle : " << po.m_billCycle;
	os << " accountId : " << po.m_accountId << " tpObjId : " << po.m_tpObjId << " beginTime : " << po.m_beginTime;
	os << " endTime : " << po.m_endTime << " billId : " << po.m_billId << " value : " << po.m_value
	   << " DiscntValue:"<<po.m_discntValue
	   << " a_discnt:"<<po.m_a_discnt
	   << " b_discnt:"<<po.m_b_discnt
	   << " adjustBefore:"<<po.m_adjustBefore;
	os << " groupUserId:"<<po.m_groupUserId<<" operFlag:"<<po.m_operFlag;
	os << " m_areaCode: "<<po.m_areaCode;
	os << " m_feeType: "<<po.m_feeType<<"m_spCode:"<<po.m_spCode<<"m_spBizCode"<<po.m_spBizCode;
	os << " m_serialNumber: "<<po.m_serialNumber;
	return os;
}

ostream& operator<<(ostream& os,const RateRunInfo& r_obj)
{
  
	os << " channelNo  : " << r_obj.m_channelNo << " m_state :     " << r_obj.m_state;
	os << " m_fileName : " << r_obj.m_fileName  << " m_billCycle : " << r_obj.m_billCycle;
	os << " m_rateDate : " << r_obj.m_rateDate  << " m_nextFileNo : " << r_obj.m_nextFileNo;
	os << " m_remark   : " << r_obj.m_remark;
  return os;
}

ostream& operator<<(ostream& os,const UserMonfeeBill& po)
{
  os<<"m_userId:"      <<po.m_userId      <<"m_dateType:"   <<po.m_dateType
    <<"m_itemCode:"    <<po.m_itemCode    <<"m_fee:"        <<po.m_fee
    <<"m_discntFee:"   <<po.m_discntFee   <<"m_displayType:"<<po.m_displayType
    <<"m_serialNumber:"<<po.m_serialNumber<<"m_spCode:"     <<po.m_spCode
    <<"m_spBizCode:"   <<po.m_spBizCode    <<"m_feeType:"   <<po.m_feeType;
	return os;
}

int operator<(const UserMonfeeBill &left,const UserMonfeeBill& right)
{
  if(left.m_userId < right.m_userId ) return true;
  if(right.m_userId < left.m_userId ) return false;
  
  if(left.m_spBizCode<right.m_spBizCode) return true;
  if(right.m_spBizCode<left.m_spBizCode) return false;
  
  if(left.m_spCode<right.m_spCode) return true;
  if(right.m_spCode<left.m_spCode) return false;
  
  if(left.m_feeType[0]<right.m_feeType[0]) return true;
  if(right.m_feeType[0]<left.m_feeType[0]) return false;  	  
  return (left.m_itemCode < right.m_itemCode);
}

int operator==(const UserMonfeeBill &left,const UserMonfeeBill& right)
{
  return (left.m_userId  == right.m_userId       &&
          left.m_itemCode  == right.m_itemCode   &&
          left.m_spCode  == right.m_spCode       &&
          left.m_spBizCode  == right.m_spBizCode &&
          left.m_feeType[0] == right.m_feeType[0]);
}


UserMonfeeBill& UserMonfeeBill::operator +=(const UserMonfeeBill &r_right)
{
  m_fee       += r_right.m_fee;
  m_discntFee += r_right.m_discntFee;
  return *this;
}

UserMonfeeBill& UserMonfeeBill::operator -=(const UserMonfeeBill &r_right)
{
  m_fee       -= r_right.m_fee;
  m_discntFee -= r_right.m_discntFee;
  return *this;
}

void UserMonfeeBill::operator=(const RollBackBill& right)
{
    m_userId       = right.m_userId;
    m_dateType[0]  = right.m_dateType[0];
    m_serialNumber = right.m_serialNumber;
    m_spBizCode    = right.m_spBizCode;            
    m_spCode       = right.m_spCode;            
    m_itemCode     = right.m_billId;
    m_fee          = right.m_discntValue;
    m_discntFee    = right.m_a_discnt;            
    m_displayType  = right.m_type;
    m_feeType[0]   = right.m_feeType[0];
} 

bool UserMonfeeBill::equal(const UserMonfeeBill &left)
{
	return((m_fee==left.m_fee)&&
	      (m_discntFee==left.m_discntFee));
}

ostream& operator<<(ostream& os,const UserSumBill3& po)
{
  	os << " userId    : " << po.m_userId
  	   << " tpObjId   : " << po.m_tpObjId
  	   << " spCode    : " << po.m_spCode
  	   << " spBizCode : " << po.m_spBizCode  
       << " beginTime : " << po.m_beginTime 
       << " endTime   : " << po.m_endTime 
       << " cycleId   : " << po.m_cycleId 
       << " value     : " << po.m_value;
     return os;
}

bool UserSumBill3::equal(const UserSumBill3 &left)
{
  return (m_value == left.m_value);
}

int operator <(const UserSumBill3 &left,const UserSumBill3& right)
{
	if (left.m_userId  < right.m_userId) return true;
  if (right.m_userId  < left.m_userId) return false;
	
  if (left.m_tpObjId  < right.m_tpObjId) return true;
  if (right.m_tpObjId  < left.m_tpObjId) return false;
  
  if (left.m_spCode  < right.m_spCode) return true;
  if (right.m_spCode  < left.m_spCode) return false;
  
  if (left.m_spBizCode  < right.m_spBizCode) return true;
  if (right.m_spBizCode  < left.m_spBizCode) return false;
  
  if (left.m_beginTime  < right.m_beginTime) return true;
  if (right.m_beginTime  < left.m_beginTime) return false;
   
  return (left.m_cycleId < right.m_cycleId);
}

int operator==(const UserSumBill3 &left,const UserSumBill3& right)
{
   return (left.m_userId      == right.m_userId   &&
           left.m_tpObjId     == right.m_tpObjId  &&
           left.m_spCode      == right.m_spCode   &&
           left.m_spBizCode   == right.m_spBizCode&&
           left.m_beginTime   == right.m_beginTime&&
           left.m_cycleId     == right.m_cycleId);
}

UserSumBill3& UserSumBill3::operator +=(const UserSumBill3 &r_right)
  {
     m_value += r_right.m_value;
       return *this;
  }
UserSumBill3& UserSumBill3::operator -=(const UserSumBill3 &r_right)
  {
     m_value -= r_right.m_value;
       return *this;
  }