/**
*   基本话单结构.
*   定义话单的基本结构,并封装话单和字段串之间的转换接口
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-7-31
*/
#ifndef __CDRBASE_H_INCLUDE_20080731_
#define __CDRBASE_H_INCLUDE_20080731_

#include "config-all.h"
#include "AttrIdConstDef.h"
#include "base/SmartPtr.h"
#include "base/PString.h"
#include "base/Types.h"
#include "FieldsXX.h"

class CdrFormat;
class CdrBase 
{
  public:
     CdrBase();
    virtual ~CdrBase();
  public:
    void setErrorCode(const int &r_errorCode);
    virtual bool initialize(const int &r_maxNum);
    virtual bool reset();       //读取话单前初始化接口
            void setStrCdr(const char * r_strCdr);
            bool inputCdr(const CdrFormat *r_cdrFormat,const int &r_fldNum=0);
    virtual bool canonicalize();//读取话单后规整接口
    virtual bool finalize();    //话单处理后接口
            char *outputCdr(const CdrFormat *r_cdrFormat);
    int   getMaxIndex();
    bool  setSpLongValue(const int &r_index,const char * r_strValue);
    const char* getSpLongValue(const int &r_index);
    T_ATTRIBUTE_VALUE& operator[](const int& r_index); 
    //测试时，查看话单的信息    
    void dumpCdrInfo(const CdrFormat *r_cdrFormat) const;
  protected:
    virtual void clear();
  public:
    T_ATTRIBUTE_VALUE *m_attrList;      //初始化时申请
    char              *m_orgBuff;       //存放原始话单
    int                m_orgBufLen;     //m_orgBuff缓冲区大小
    char              *m_desBuff;       //存放输出话单信息
    int                m_desBuffLen;    //m_desBuff缓冲区大小
    
    PString            m_splongFld[ATTRI_SPLONG_MAX-ATTRI_SPLONG_MIN+1+1];
    int                m_errorCode; //错误代码
  protected:
    int                m_maxNum;   //话单属性个数
  private:
    int               *m_fldDelimListr; //字段分割符下标位置
    int                m_fldNum ;       //字段个数
};


// 为CDR定义一个指针包装类,以方便访问
// 注：只系统访问普通属性接口,其它接口需通过CdrBase访问
class PCDR : public counted_ptr<CdrBase> 
{
  public:
    PCDR(CdrBase * r_pcdr =0):counted_ptr<CdrBase>(r_pcdr){};
    T_ATTRIBUTE_VALUE &operator [] (const int& r_index);
    const T_ATTRIBUTE_VALUE &operator [] (const int& r_index) const;
};

#endif //__CDRBASE_H_INCLUDE_20080731_

