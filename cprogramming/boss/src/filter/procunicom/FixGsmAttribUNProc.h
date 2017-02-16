#ifndef __FIXGSMATTRIBUNPROC_H__
#define __FIXGSMATTRIBUNPROC_H__

#include "config-all.h"
#include "AttribUNProc.h"

class FixGsmAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();

  protected:
  	void initialattr();

    bool getHomeAreaCodeA();    //固网求A归属
    bool transNumberProcess();  //固网求接入号
    int specialPhoneProcess();  //固网求特殊号码
    bool setFixLongType();      //固网求号段
    bool setSameSubAreaType();  //设定固网同营业区
    void setFixHomeAreaCodeB(const T_ATTRI_VAL &area_codeB); //设定固网B归属区号
    bool getSpecialPhoneGroup(); //固网求特殊号码组

    int m_channelNo;
    T_ATTRI_VAL m_patype;
};

#endif //__FIXGSMATTRIBUNPROC_H__
