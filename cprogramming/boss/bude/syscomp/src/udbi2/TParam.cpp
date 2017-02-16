#include <sstream>
#include "TParam.h"
#include "udbi-inter.h"

UDBI::TParam::TParam(const string& par):
	m_ParamName(par),bindhp(0),m_ParamInOut(_PAR_IN),m_ifNull(0),m_pvalue(0)
{
	UDBI_TRACE_P1("TParam::TParam(par='%s')", m_ParamName.c_str());
}

UDBI::TParam::~TParam()
{
	UDBI_TRACE_P1("TParam::~TParam()[ParName='%s']", m_ParamName.c_str());

    switch (m_ParamType) {
    case PTYPE_CINT:
        delete m_pival;
        break;
    case PTYPE_CFLT:
        delete m_pdval;
        break;
    case PTYPE_CSTR:
        //delete [] m_pcval;
        break;
    case PTYPE_INT:
    case PTYPE_FLT:
    case PTYPE_BUF:
    	break;
    case PTYPE_INT_ARRAY:
    	delete [] m_pival;
    	break;
    case PTYPE_STR_ARRAY:
    	delete [] m_pcval;
    	break;        
    default:
        break;
    }
}

void UDBI::TParam::SetParam(const int val, short* isNull)
{
	UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%d]", m_ParamName.c_str(), val);
	
    m_ParamType = PTYPE_CINT;
    m_ReqType = SQLT_INT;
    m_ifNull = isNull;
    
    m_ValueLen = sizeof(int);
    m_pival = new int(val);
    
    m_pvalue = (void*)m_pival;
}

void UDBI::TParam::SetParam(const double val, short* isNull)
{
	UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%10.2d]", m_ParamName.c_str(), val);
	
    m_ParamType = PTYPE_CFLT;
    m_ReqType = SQLT_FLT;
    m_ifNull = isNull;
    
    m_ValueLen = sizeof(double);
    m_pdval = new double(val);
    
    m_pvalue = (void*)m_pdval;
}

void UDBI::TParam::SetParam(const string& val, short* isNull)
{
	UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal='%s']", m_ParamName.c_str(), val.c_str());
	
    m_ParamType = PTYPE_CSTR;
    m_ReqType = SQLT_STR;
    m_ifNull = isNull;
    
    m_strvalue = val;
    m_ValueLen = strlen(m_strvalue.c_str())+1;
    m_pvalue = (void*)m_strvalue.c_str();

	/*    
    m_ValueLen = strlen(val.c_str())+1;
    m_pcval = new char[m_ValueLen];
    
    strcpy(m_pcval, val.c_str());
    m_pvalue = (void*)m_pcval;
    */
}

void UDBI::TParam::SetParam(vector< int >& val, short* isNull)
{
	m_ParamType = PTYPE_INT_ARRAY;
	m_ReqType = SQLT_INT;
	m_ifNull = isNull;
	
	m_ValueLen = sizeof(int);
	m_pival = new int[val.size()];
	
	int i=0;
	vector< int >::const_iterator iter;
	for ( iter=val.begin(); iter!=val.end(); iter++ )
	{
		*(m_pival+i) = *iter;
		i++;
	}
	m_ArrayLen = i;
	
	m_pvalue = m_pival;
}

void UDBI::TParam::SetParam(vector< string >& val, int len, short* isNull)
{
	m_ParamType = PTYPE_STR_ARRAY;
	m_ReqType = SQLT_STR;
	m_ifNull = isNull;
	
	m_ValueLen = len;
	m_pcval = new char[val.size()*len];
	
	int i=0;
	vector< string >::const_iterator iter;
	for ( iter=val.begin(); iter!=val.end(); iter++ )
	{
		strcpy((m_pcval+i*len), (*iter).c_str());
		//*(m_pcval+i*len-1) = '\0';
		i++;
	}
	m_ArrayLen = i;
	
	m_pvalue = m_pcval;
}

void UDBI::TParam::BindParam(int& val, short* isNull)
{
	UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%d]", m_ParamName.c_str(), val);
	
	m_ParamInOut = _PAR_OUT;
	m_ParamType = PTYPE_INT;
	m_ReqType = SQLT_INT;  
	m_ifNull = isNull;
	
	m_ValueLen = sizeof(int);
    m_pival = &val;

    m_pvalue = (void*)m_pival;
}

void UDBI::TParam::BindParam(double& val, short* isNull)
{
	UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%10.2d]", m_ParamName.c_str(), val);
	
    m_ParamInOut = _PAR_OUT;
    m_ParamType = PTYPE_FLT;  
    m_ReqType = SQLT_FLT;
    m_ifNull = isNull;
    
	m_ValueLen = sizeof(double);
    m_pdval = &val;

    m_pvalue = (void*)m_pdval;
}

void UDBI::TParam::BindParam(char* val, int len, short* isNull)
{
	UDBI_DEBUG_P2("TParam::BindParam()[ParName='%s', ParVal='%s']", m_ParamName.c_str(), val);
	
	m_ParamInOut = _PAR_OUT;
    m_ParamType = PTYPE_BUF; 
    m_ReqType = SQLT_STR;
    m_ifNull = isNull;
    
    m_ValueLen = len;
    m_pcval = val;

    m_pvalue = (void*)m_pcval;
}

//! 2005.12.20  WangFJ.
void UDBI::TParam::BindParam(JCursor* pcur)
{
	UDBI_DEBUG_P1("TParam::BindParam()[ParName='%s', cursor]", m_ParamName.c_str());		
	
	m_ParamInOut = _PAR_OUT;
	m_ParamType = PTYPE_CURSOR;
	m_ReqType = SQLT_RSET;
	m_ifNull = 0;
	
	m_ValueLen = 0;
	m_pcursor = pcur;
	m_pvalue = (void*)m_pcursor->stmtHandle();
}

/**
 * 增补函数，绑定整型数组，传递整型数组指针
 * ※ 外部定义整型数组指针并分配内存，外部负责整型数组指针所指内存的释放
 */
void UDBI::TParam::SetParam(int* pival, int arraylen, short* isNull)
{
	//UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%d]", m_ParamName.c_str(), val);
	
	m_ParamInOut = _PAR_IN;
	m_ParamType = PTYPE_INT;
	m_ReqType = SQLT_INT;  
	m_ifNull = isNull;
	
	m_ArrayLen = arraylen;
	m_ValueLen = sizeof(int);
    m_pival = pival;

    m_pvalue = (void*)m_pival;	
}

/**
 * 增补函数，绑定字符串数组，传递字符串数组指针
 * ※ 外部负责指针所指内存的分配与释放
 */
void UDBI::TParam::SetParam(char* pcval, int clen, int arraylen, short* isNull)
{
	//UDBI_DEBUG_P2("TParam::SetParam()[ParName='%s', ParVal=%d]", m_ParamName.c_str(), val);
	
	m_ParamInOut = _PAR_IN;
	m_ParamType = PTYPE_BUF;
	m_ReqType = SQLT_STR;  
	m_ifNull = isNull;
	
	m_ArrayLen = arraylen;
	m_ValueLen = clen;
    m_pcval = pcval;

    m_pvalue = (void*)m_pcval;		
}

//!
UDBI::JParam::~JParam()
{
	UDBI_TRACE_P1("JParam::~JParam()[ParamsCount=%d]", m_params.size());	

	vector< TParam* >::iterator iter = m_params.begin();
	for ( ; iter!=m_params.end(); iter++ )
	{
		delete (*iter);
	}
	
	m_params.clear();
}

void UDBI::JParam::Clear()
{
	vector< TParam* >::const_iterator iter = m_params.begin();
	for ( ; iter!=m_params.end(); iter++ )
	{
		delete (*iter);
	}
	
	m_params.clear();
}

string UDBI::JParam::toString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}

void UDBI::JParam::Bind(OCIStmt *vpStmt, OCIError *vpErr)
{
	UDBI_TRACE_P1("JParam::Bind(OCIStmt *stmthp, OCIError *errhp)[ParamsCount=%d]", m_params.size());
	
	vector< TParam* >::const_iterator iter = m_params.begin();
	for ( ; iter!=m_params.end(); iter++) {
		CHECKERR(vpErr, OCIBindByName(vpStmt, &(*iter)->bindhp, vpErr, (text *)(*iter)->parName(), (sb4)strlen((*iter)->parName()),
						(dvoid *)(*iter)->pval(), (sb4)(*iter)->valLen(), (ub2)(*iter)->reqType(), (dvoid *)((*iter)->indp()),
						(ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT) );
	}
}
