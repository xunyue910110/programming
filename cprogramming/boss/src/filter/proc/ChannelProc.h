#ifndef _CHANNELPROC_H_20080928_
#define _CHANNELPROC_H_20080928_

#include "config-all.h"
#include "CDRProc.h"
#include "FieldsXX.h"

#include "FilterRule.h"
#include "DetailChannelItem.h"

class ChannelProc:public CDRProc
{
  public:
    ChannelProc();
    virtual ~ChannelProc();

    virtual bool initialize();
		virtual int  reset();
    virtual int  execute();
  protected:
  	bool InitCdrProperty(); //获取分通道要素配置
  	bool getChanByFilterTable(); //根据分拣规则表获取通道名
  	
  	ChannelRule * m_channelRule;
  	T_ATTRI_VAL m_channel;


};


#endif //_CHANNELPROC_H_20080928_