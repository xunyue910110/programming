#ifndef _ADUUP_CYCLERULE_H_
#define _ADUUP_CYCLERULE_H_

#include "FieldsXX.h"

class RateGlobalInfo;
class RateTpInfo;
class AddupCycleRule
{
public:
	AddupCycleRule(){}
	~AddupCycleRule(){}
	bool operator < (const AddupCycleRule &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const AddupCycleRule &right) const
	{
		return (m_id == right.m_id);
	}

public:
    //根据话单起始时间、周期定义信息 计算 当前话单所在的周期信息
    //Modified By Sunam 2009/12/29 10:20:16 根据指定的时间r_cdrStartTime 求累计量周期
    //bool getSumTimeInfo(RateGlobalInfo       *r_pGlobalInfo,
    //                    const RateTpInfo     *r_rateTpInfo,
    //                          long           &r_cycleBeginTime,
    //                          long           &r_cycleEndTime) const;
    bool getSumTimeInfo(RateGlobalInfo       *r_pGlobalInfo,
                        const RateTpInfo     *r_rateTpInfo,
                        const T_DATE_TIME    &r_cdrStartTime,
                              long           &r_cycleBeginTime,
                              long           &r_cycleEndTime) const;
protected:
    //根据计费实例时间计算第一个周期的时间点
    bool getFSumTimeInfo( RateGlobalInfo *r_pGlobalInfo,
                         const T_DATE_TIME    &r_tpObjTime,
                               long           &r_fStartTime) const;
    //偏移量r_offSetValue格式MMDDHHMISS,
    //r_dateTime  YYYYMMDDHHMISS
    void addOffSetTime(const char *r_offSetValue,long &r_dateTime) const;
    //根据第一个周期、话单时间、周期定义计算当前话单所在周期
    void getSumTimeInfo( RateGlobalInfo *r_pGlobalInfo,
                        const T_DATE_TIME    &r_cdrTime,
                        const long           &r_fStartTime,
                              long           &r_cycleBeginTime,
                              long           &r_cycleEndTime) const;
  	//取余
	  long absMode(const long &r_number,const int &r_mode) const;
public:	
	void dump(char *prefix);

public:
	char m_cycleType;            //周期类型编码
	char m_defineTag;            //周期偏移值来源
	int  m_cycleNum;             //周期数量
	int  m_id;                   //周期规则标识
  T_OBJECT_ATTR m_offsetValue; //周期偏移值
};	

#endif
