#ifndef _MACROADAPTER_H_
#define _MACROADAPTER_H_

#include "flowline/adapter/Adapter.h"
#include "flowline/Context.h"

#include "fileadapter/Util.h"
#include "flowline/flbase.h"
#include <string>
namespace flowline
{

class MacroAdapter :  public flowline::MacroAttribute //,public flowline::adapter::Adapter
{
public:
    MacroAdapter(Context& ctx) : m_ctx(ctx){
    }
	virtual ~MacroAdapter();   
    //父类要求的
    //virtual void configure(const string & configuration){};
    //根据宏名得到返回值
    virtual string getAttribute(const string& name) const;
    //得到匹配符
    virtual string getAttributeFilter(const string& name) const ;
    //设置取流水号关键字
    void setTargetId(const string& targetkey){
        targetname = targetkey;
    }
    
    const string& getTargetId()const{
       return targetname;
    }
    
private:
    Context& m_ctx;
    string targetname;
};

};

#endif //_MACROADAPTER_H_
