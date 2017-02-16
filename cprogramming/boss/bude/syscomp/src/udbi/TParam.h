#ifndef __TPARAM_H_
#define __TPARAM_H_

#include <string>
#include <vector>
#include "udbi-inter.h"

//! 2005.12.20  WangFJ.
#include "JCursor.h"

class OCIBind;
class OCIStmt;
class OCIError;

namespace UDBI {
using std::string;
using std::vector;
	
//! 2005.12.20  WangFJ.	
enum PARAM_TYPE { 
	PTYPE_CINT=1, PTYPE_CFLT=2, PTYPE_CSTR=3, 
	 PTYPE_INT=4, PTYPE_FLT=5, PTYPE_BUF=6, 
	  PTYPE_INT_ARRAY=7, PTYPE_STR_ARRAY=8,
	  PTYPE_CURSOR=9  
};

//! 定义绑定参数类	
class TParam {
public:
    TParam(const string& parName);
    virtual ~TParam();

	//! 接口定义    
    void SetParam(const int val, short* isNull=0);
    void SetParam(const double val, short* isNull=0);
    void SetParam(const string& val, short* isNull=0);
    void SetParam(vector< int >& val, short* isNull=0);
    void SetParam(vector< string >& val, int len, short* isNull=0);
    void BindParam(int& val, short* isNull=0);
    void BindParam(double& val, short* isNull=0);
    void BindParam(char* val, int len, short* isNull=0);
    
    //! 2005.03.24 增补函数，用于数组绑定
    void SetParam(int* pival, int arraylen, short* isNull=0);
	void SetParam(char* pcval, int clen, int arraylen, short* isNull=0);

    //! 2005.12.20  WangFJ.
    void BindParam(JCursor* pcur);

	//! 内联函数返回内部成员，用于JParam 的绑定操作
	inline PARAM_TYPE parType() const 
	{ 
		return m_ParamType; 
	}
	inline const char* parName() const 
	{ 
		return m_ParamName.c_str(); 
	}
	inline int parInOut() const 
	{ 
		return m_ParamInOut; 
	}
	inline int reqType() const 
	{  
		return m_ReqType; 
	}
	inline int valLen() const 
	{ 
		return m_ValueLen; 
	}
	inline int* arrayLen() 
	{ 
		return &m_ArrayLen; 
	}
	inline void* pval() const 
	{ 
		return m_pvalue; 
	}
	inline short* indp() const 
	{ 
		return m_ifNull; 
	}
	
	//! 格式化输出参数名称及参数值
	friend ostream& operator<<(ostream& out, const TParam& rhs)
	{
	    switch (rhs.m_ParamType) {
		    case PTYPE_CINT:
		    case PTYPE_INT:
		        out << "[ParName='" << rhs.m_ParamName << "', ParVal=" << *rhs.m_pival << "]" << endl;
		        break;
		    case PTYPE_CFLT:
		    case PTYPE_FLT:
		        out << "[ParName='" << rhs.m_ParamName << "', ParVal=" << *rhs.m_pdval << "]" << endl;
		        break;
		    case PTYPE_CSTR:
		    	out << "[ParName='" << rhs.m_ParamName << "', ParVal='" << rhs.m_strvalue << "']" << endl;
		        break;
		    case PTYPE_BUF:
		        out << "[ParName='" << rhs.m_ParamName << "', ParVal='" << *rhs.m_pcval << "']" << endl;
		        break;
		    case PTYPE_INT_ARRAY:
		    case PTYPE_STR_ARRAY:
		    default:
		        break;
	    }
	    
	    return out;
	}	

public:
	//! 绑定句柄（stmt 隐身创建）
	OCIBind* bindhp;
	
private:
	//! 屏蔽缺省构造函数、拷贝构造函数和赋值运算符
	TParam();
    TParam(const TParam& rhs);
    TParam& operator=(const TParam& rhs);
    
    //! 参数输入、输出类型
    enum PARAM_INOUT { _PAR_IN=1, _PAR_OUT=2, _PAR_INOUT=3 };
    
    //! 私有成员定义
    string m_ParamName;
    PARAM_TYPE m_ParamType;
    PARAM_INOUT m_ParamInOut;
    int m_ReqType;
    int m_ValueLen;
    int m_ArrayLen;		/** 暂未使用 */
    short* m_ifNull;
	void* m_pvalue;
	string m_strvalue;
    union {
        int* m_pival;
        double* m_pdval;
        char* m_pcval;
        JCursor* m_pcursor;	//! 2005.12.20  WangFJ.
    };
};

/**
 * @class JParam  "TParam.h"
 * @brief 参数集合类
 */
class JParam {
public:
	JParam() { UDBI_TRACE("JParam::JParam()"); }
	virtual ~JParam();
	
#define SET_PARAM \
	TParam* param = new TParam(par); \
	param->SetParam(val, isNull); \
	m_params.push_back(param);
	
	inline void SetParam(const string& par, int val, short* isNull=0)
	{
		SET_PARAM
	}
	inline void SetParam(const string& par, double val, short* isNull=0)
	{
		SET_PARAM
	}
	inline void SetParam(const string& par, const string& val, short* isNull=0)
	{
		SET_PARAM
	}
	inline void SetParam(const string& par, vector< int >& val, short* isNull=0)
	{
		SET_PARAM
	}
	inline void SetParam(const string& par, vector< string >& val, int len, short* isNull=0)
	{
		TParam* param = new TParam(par);
		param->SetParam(val, len, isNull);
		m_params.push_back(param);
	}
#undef SET_PARAM

	inline void BindParam(const string& par, int& val, short* isNull=0)
	{
		TParam* param = new TParam(par);
		param->BindParam(val, isNull);
		m_params.push_back(param);
	}
	inline void BindParam(const string& par, double& val, short* isNull=0)
	{
		TParam* param = new TParam(par);
		param->BindParam(val, isNull);
		m_params.push_back(param);
	}
	inline void BindParam(const string& par, char* buf, int buflen, short* isNull=0)
	{
		TParam* param = new TParam(par);
		param->BindParam(buf, buflen, isNull);
		m_params.push_back(param);
	}	
	//! 2005.12.20  WangFJ.
	inline void BindParam(const string& par, JCursor* pcur)
	{
		TParam* param = new TParam(par);
		param->BindParam(pcur);
		m_params.push_back(param);
	}
	
    inline void SetParam(const string& par, int* pival, int arraylen, short* isNull=0)
    {
    	TParam* param = new TParam(par);
    	param->SetParam(pival, arraylen, isNull);
    	m_params.push_back(param);
    }
    	
	inline void SetParam(const string& par, char* pcval, int clen, int arraylen, short* isNull=0)
	{
		TParam* param = new TParam(par);
		param->SetParam(pcval, clen, arraylen, isNull);
		m_params.push_back(param);
	}	
	
		
	//! 清空函数
	void Clear();

	//! 参数数据输出
	string toString();

	//! SQL语句参数绑定
	void Bind(OCIStmt *vpStmt, OCIError *vpErr);
	
	//! 参数数据流输出
	friend ostream& operator<<(ostream& out, const JParam& rhs)
	{
		vector< TParam* >::const_iterator iter = rhs.m_params.begin();
		for ( ; iter!=rhs.m_params.end(); iter++) {
			out << *(*iter);
		}			
		
		return out;
	}
	
protected:
	JParam(const JParam& rhs);
	JParam& operator=(const JParam& rhs);
	
private:
	vector< TParam* > m_params;
	
};

	
} // namespace UDBI


#endif  /* __TPARAM_H_  */
