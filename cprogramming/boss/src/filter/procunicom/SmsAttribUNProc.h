#ifndef __SmsAttribUNProc_H__
#define __SmsAttribUNProc_H__

#include "config-all.h"

#include "AttribUNProc.h"

class SmsAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();
  protected:
  	bool getSmAHByMsisdn(); //获取A的归属
  	bool getSmBHByMsisdn(); //获取B的归属
  	bool getFixSmAHByMsisdn(); //获取固网A的归属
  	
  	void setSmRoamType(); //设置短信漫游类型
  	
  	int m_channelNo;
};


#endif

