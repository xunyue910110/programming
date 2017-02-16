#ifndef _PROVCDRPROC_H_20081110_
#define _PROVCDRPROC_H_20081110_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

#include "UserProperty.h"

class ProvCDRProc:public CDRProc
{
  public:
    virtual int execute();

  private:
  	//属性初始化
  	bool InitCdrProperty();
  	//trash求取
  	bool getTrash();
  	//分通道 绑资料
  	bool getDisChannelProperty();

    //根据手机号绑资料
    bool getChannelByBindPhone();
    //根据IMSI号绑资料
    bool getChannelByBindImsi();
    //先根据手机号后根据IMSI号绑资料
    bool getChannelByBindPhoneImsi();
    //根据专网号绑资料 手机号 + 接入中继号(逻辑号码+物理号码)
    bool getChannelByBindTrunk();

    //根据对方号码获取用户资料
    void bindOtherInfo();
    //绑定资料
    void bindInfo(const UserServInfo &r_userServInfo, const UserServInfo &m_otherServInfo);
    //替换无效的msisdn imsi
    void replaceMsisdn(const UserServInfo &r_userServInfo);

  private:
  	UserServInfo m_userServInfo;  //计费方资料
  	UserServInfo m_otherServInfo; //对端资料

};

#endif //_PROVCDRPROC_H_20081110_
