#ifndef __ATTRIBUNPROC_H__
#define __ATTRIBUNPROC_H__

#include "config-all.h"
#include "base/TimeUtil.h"
#include "AttribProc.h"

class AttribUNProc:public AttribProc
{
  public:
  	//Added by Sunam 2006-6-22 9:00
  	bool initialize();

    virtual int execute();
/*    
    bool getGsmAttribMethod();
    bool getSmsAttribMethod();
    bool getGprsAttribMethod();
    bool getMiscAttribMethod();
*/
    //判断是否是无条件呼转话单
    bool isUnConditionalForw();

	  //Added by Sunam 2006-3-13 14:07
	  void getExtendAttr(void);

    //求A方的归属
    virtual bool getHomeAreaCodeA();
    //求A方的到访地
    virtual bool getVisitAreaCodeA();
    //求B方的归属
    virtual bool getHomeAreaCodeB();
    //求B方的归属
    virtual bool getVisitAreaCodeB();

	  //求C方的属性,主要是 USER_TYPE_C
	  virtual bool getUserTypeC();
	  virtual bool setUserServiceTypeA();

    //处理特殊号码
	  virtual int specialPhoneProcess();
	  //处理特殊号码组
	  virtual bool getSpecialPhoneGroup();

		//判断是否是专网转接号码
		virtual void setUnicomTrans();

		//边界漫游处理
		virtual void edgeRoamProcess();
		//处理接入号码
		virtual bool transNumberProcess();

		//求中继信息
		virtual bool getTrunkInfo();

		//求长途
		virtual void setLongType();
		virtual void setMocLongType();
		virtual void setMtcLongType();
		virtual void setForwLongType();


		//漫游话单的属性求解方法
		bool getRoHomeAreaCodeA();
		bool getRoAHByMsisdn();
		bool getRoAHByImsi();
		bool getRoVisitAreaCodeA();
		bool getRoAVByMsc();
		bool getRoHomeAreaCodeB();
		bool getRoBHByMsisdn();
		bool getRoVisitAreaCodeB();
		bool getRoBVByMsrn();
		void setRoRoamType();
		void setRoLongType();

		//判断是否需要做计费政策(套餐)
		virtual bool isNoNeedTP();

    ////获得主被叫归属地和号码
    bool getTollHomeAreaA();

    ////获得固网特殊主叫归属地和号码
    bool getFixHomeAreaA();

    ////获取城市代码
    bool getCPNCode();
    
    //十进制地址格式转换为16进制
    void setdecTohexAddr(const T_ATTRI_VAL &decAddr,T_ATTRI_VAL &hexAddr);
    
    //B号码设定为本地市话
    bool setDefaultHomeAreaB();

    //B号码设定为网通本地市话
    bool setCNCDefaultHomeAreaB();
    
    //B号码固网号段求取
    int getFixTelephoneSeg();
  protected:

/*
  private :
  	bool unique_flag; //是否要排重标志
*/
  public :
  	vector<string>  m_interHead;   //国际字冠头组
};

#endif //__ATTRIBUNPROC_H__

