#ifndef _UNIQUEPROC_H_200800827_
#define _UNIQUEPROC_H_200800827_

#include "config-all.h"
#include "ProcessBase.h"

#include "FilterRule.h"

class UniqueProc:public ProcessBase
{
  public:
    UniqueProc();
    virtual ~UniqueProc();

    virtual bool initialize();
		virtual int  reset();
    virtual int  execute();
  protected:
  	void setMaxLength(int r_maxLength);
  	bool AbstractField(const int &r_cdrid,const int &r_offset,const int &r_length,const string &fix_str,string &r_str);
    bool getConfigBySource(); //根据数据源获取排重配置
    bool getConfigByCdr();    //根据话单内容获取具体排重配置

    DividRule  *m_dividRule;
    DividRule::VectorDividParam *m_vectorDividParam;
    int m_maxLength;

};


#endif //_UNIQUEPROC_H_200800827_