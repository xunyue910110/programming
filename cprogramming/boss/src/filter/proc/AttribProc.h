#ifndef __ATTRIBPROC_H__
#define __ATTRIBPROC_H__

#include "config-all.h"
#include "base/TimeUtil.h"
#include "ProcessBase.h"

class AttribProc:public ProcessBase
{
  public:
  	//Added by Sunam 2006-6-22 9:00
  	bool initialize();

    virtual int execute();

	  //判此条话单是否合法
	  bool isValidCDR();
	  //错误话单判断过程
	  virtual bool setErrorCode();

    //判断是否漫游
    bool isRoam();

    //判断号码是否为一个手机号码
    bool isMobile(const T_ATTRI_VAL &phoneNubmer);
    //判断此号码是否为本运行商号码
    bool isSelfNumber(const T_ATTRI_VAL &msisdn);
    //判断此号码为手机号码但不是本运行商 add by shenglj
    bool isOtherNumber(const T_ATTRI_VAL &msisdn);
    //判断是否是无条件呼转话单
    //-------//bool isUnConditionalForw();
    bool isVoiceBox(const T_ATTRI_VAL &msisdn);
    bool isOriginalCall();
    bool isVirtualArea(const T_ATTRI_VAL &r_areaCode1,const T_ATTRI_VAL &r_areaCode2,const T_ATTRI_VAL &r_hometype);
    bool isSameArea(const T_ATTRI_VAL &areaCode1,const T_ATTRI_VAL &areaCode2);

    //Added By Sunam For OCS 2006-6-16 14:50 根据交换机代码判断是否是省际漫游
    //暂不使用
    //bool isInterProvRoamOut(const T_ATTRI_VAL &orgMsc);

    //求 HOME_CODE_A的两种方法
    bool getAHByImsi(const T_ATTRI_VAL &imsi_number);
    bool getAHByMsisdn(const T_ATTRI_VAL &msisdn);
    //求 VISIT_CODE_A 的六种方法
    bool getAVByMscLacCellId();
    bool getAVByCellId();
    bool getAVByMscLac();
    bool getAVByMscCellId();
    bool getAVByLac();

    //Modified by Sunam 2006-3-27 18:55
    bool getAVByMsc(const MSC_TYPE &msc_type);
    bool getAVByLacCellId();

    //取B归属及B到访的方法
    bool getBHByMsisdn(const T_ATTRI_VAL &msisdn);     //未定义 add by shenglj
    bool getBHByMsisdnImsi(); //未定义
    bool getBVByMsrn();
    bool getBVByLacMsrn();
    bool getBVByHome();

    bool getFixTelephone();
	  //Added by Sunam 2006-3-13 14:07
	  //-------//void getExtendAttr(void);

    bool msisdnToImsi(const T_ATTRI_VAL &msisdn, T_ATTRI_VAL &imsi);
		//新增
		bool imsiToMsisdn(const T_ATTRI_VAL &imsi, T_ATTRI_VAL &msisdn);

    //求A方的归属
    //-------//virtual bool getHomeAreaCodeA();
    //求A方的到访地
    //-------//virtual bool getVisitAreaCodeA();
    //求B方的归属
    //-------//virtual bool getHomeAreaCodeB();
    //求B方的归属
    //-------//virtual bool getVisitAreaCodeB();

	  //求C方的属性,主要是 USER_TYPE_C
	  //-------//virtual bool getUserTypeC();
	  //-------//virtual bool setUserServiceTypeA();

	  //Added by Sunam 2006-9-5 15:06
	  virtual void setUserAspB(const char &asp);

    //求漫游属性
    virtual void setRoamType();

    //处理特殊号码
	  //-------//virtual int specialPhoneProcess();
	  //处理特殊号码组
	  //-------//virtual bool getSpecialPhoneGroup();

		//判断是否是专网转接号码
		//-------//virtual void setUnicomTrans();

		//边界漫游处理
		//-------//virtual void edgeRoamProcess();
		//处理接入号码
		//-------//virtual bool transNumberProcess();

		//求中继信息
		//-------//virtual bool getTrunkInfo();

		//求长途
		//-------//virtual void setLongType();
		//-------//virtual void setMocLongType();
		//-------//virtual void setMtcLongType();
		//-------//virtual void setForwLongType();


		//漫游话单的属性求解方法
		//-------//bool getRoHomeAreaCodeA();
		//-------//bool getRoAHByMsisdn();
		//-------//bool getRoVisitAreaCodeA();
		//-------//bool getRoAVByMsc();
		//-------//bool getRoHomeAreaCodeB();
		//-------//bool getRoBHByMsisdn();
		//-------//bool getRoVisitAreaCodeB();
		//-------//bool getRoBVByMsrn();
		//-------//void setRoRoamType();
		//-------//void setRoLongType();

		//判断是否需要做计费政策(套餐)
		//-------//virtual bool isNoNeedTP();

    virtual bool specialPreProcessCdr();
    virtual bool otherPartyStandardization();


    //计费号码规整
    virtual void msisdnStandardization();
    //主叫号码去掉17911,193等前缀
    virtual bool callingCodePrefixProcess();
    virtual bool calledCodePrefixProcess();


  protected:

  private :
  	vector<string>  m_specialPhone;
  public :
  	//-------//vector<string>  m_interHead;   //国际字冠头组
};

#endif

