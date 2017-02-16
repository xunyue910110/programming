/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：TTable.cpp
 *  文件标识：
 *  摘    要：
 * 
 *  当前版本：1.0
 *  作    者：WangFJ
 *  完成日期：2004年12月06日
 *
 *  取代版本： 
 *  原作者  ：
 *  完成日期：
 */        
#include "TTable.h"
#include "JSQL.h"

/** \class UDBI::TTable   TTable.h
 *  \brief SQL语句执行接口实现类，实现JTable接口
 *
 *  TTable 实现JTable接口类，实现了增加SQL语句、设置SQL语句中的参数值、
 *  执行SQL语句、获取SQL语句（如SELECT语句）执行结果集、访问结果字段数据
 *  等基本功能。同时支持简单的数据库实体定义元数据的访问功能（此部分功能
 *  可视需要进行扩展），目前提供字段类型、字段长度信息。
 *
 *  \par 实例
 *  TTable 类在使用时不直接实例，而是通过JConnect接口的CreateJTable方式实例，
 *  并返回JTable接口。在使用UDBI组件时看不到也不必关心TTable的实现细节。数据
 *  库访问方式是实例TTable的连接是在XA方式还是非XA方式而可以支持XA模式和非XA模式。
 *
 */
 

/**
 *  构造函数，使用OCIEnv*, OCISvcCtx* 初始化SQL语句执行环境
 *  @param vpEnv  OCI编程环境句柄
 *  @param vpSvc  数据库连接上下文句柄
 */
UDBI::TTable::TTable(OCIEnv* vpEnv, OCISvcCtx* vpSvc, const int prefetchRecs):
	m_envhp(vpEnv), m_svchp(vpSvc), m_errhp(0), m_stmthp(0), m_PreFetchRecs(prefetchRecs), m_TabName("")
{
    UDBI_TRACE("TTable::TTable(OCIEnv* vpEnv, OCISvcCtx* vpSvc, const int prefetchRecs)");

    Clear();
    //m_TabName.clear();

	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
}

/**
 *  构造函数，使用OCIEnv* , OCISvcCtx*  初始化SQL语句执行环境，\a tabName  参数用于关联SQL语句
 *  @param vpEnv  OCI编程环境句柄
 *  @param vpSvc  数据库连接上下文句柄
 *  @param tabName 表名，用于匹配SQL语句
 */
UDBI::TTable::TTable(OCIEnv* vpEnv, OCISvcCtx* vpSvc, const string& tabName, const int prefetchRecs):
	m_envhp(vpEnv), m_svchp(vpSvc), m_errhp(0), m_stmthp(0), m_PreFetchRecs(prefetchRecs), m_TabName(tabName)
{
    UDBI_DEBUG_P1("TTable::TTable(OCIEnv*, OCISvcCtx*, const string& tabName='%s', const int)", tabName.c_str());
    
    Clear();
    //m_TabName = tabName;
    
    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
}

/**
 *  \fn UDBI::TTable::~TTable() 
 *  析构函数，释放资源
 */
UDBI::TTable::~TTable()
{
	UDBI_TRACE("TTable::~TTable()");
	
    if (m_stmthp && m_errhp) {
        (void)OCIHandleFree(m_stmthp, OCI_HTYPE_STMT);
        (void)OCIHandleFree(m_errhp, OCI_HTYPE_ERROR);
    }
}

/**
 *  \fn void UDBI::TTable::SetNullText(const string& nullText)
 *  设置字符串字段在查询结果为空时返回的值
 */
void UDBI::TTable::SetNullText(const string& nullText)
{
	m_records.SetNullText(nullText);
}

/**
 *  \fn void UDBI::TTable::SetPrefetchRecs(const int prefetchRecs)
 *  设置每次从数据库获取记录数
 */
void UDBI::TTable::SetPrefetchRecs(const int prefetchRecs)
{
	m_PreFetchRecs = prefetchRecs;
}

/** 
 *  \fn void UDBI::TTable::AddSql(const string& sql)
 *  增加SQL语句函数，可以连续调用本函数拼接SQL语句（如SQL语句比较长的情况下）
 */
void UDBI::TTable::AddSql(const string& sql)
{
	UDBI_DEBUG_P1("TTable::AddSql(const string& sql='%s')", sql.c_str());
	
    m_sql += sql;
}

/**
 *  \fn void UDBI::TTable::SetParam(const string& par, int val, short* isNull)
 *  设置SQL语句绑定参数，IN，接收int型参数
 */

/** 
 *  \fn void UDBI::TTable::SetParam(const string& par, double val, short* isNull)
 *  设置SQL语句绑定参数，IN，接收double型参数
 */

/**
 *  \fn void UDBI::TTable::SetParam(const string& par, const string& val, short* isNull)
 *  设置SQL语句绑定参数，IN，接收const string& 型参数
 */

/**
 *  \fn void UDBI::TTable::SetParam(const string& par, vector< int >& vval, short* isNull)
 *  设置SQL语句绑定参数，IN，接收int数组型参数
 */

/**
 *  \fn void UDBI::TTable::SetParam(const string& par, vector< string >& vval, int len, short* isNull)
 *  设置SQL语句绑定参数，IN，接收string数组型参数
 */
void UDBI::TTable::SetParam(const string& par, int val, short* isNull)
{
	m_params.SetParam(par, val, isNull);
}
void UDBI::TTable::SetParam(const string& par, double val, short* isNull)
{
	m_params.SetParam(par, val, isNull);
}
void UDBI::TTable::SetParam(const string& par, const string& val, short* isNull)
{
	m_params.SetParam(par, val, isNull);
}
void UDBI::TTable::SetParam(const string& par, vector< int >& vval, short* isNull)
{
	m_params.SetParam(par, vval, isNull);
}
void UDBI::TTable::SetParam(const string& par, vector< string >& vval, int len, short* isNull)
{
	m_params.SetParam(par, vval, len, isNull);
}

/**
 *  \fn void UDBI::TTable::BindParam(const string& par, int& val, short* isNull)
 *  设置SQL语句绑定参数，IN/OUT，接收int& 型参数
 */
/**
 *  \fn void UDBI::TTable::BindParam(const string& par, double& val, short* isNull) 
 *  设置SQL语句绑定参数，IN/OUT，接收double&型参数
 */
 /**
 *  \fn void UDBI::TTable::BindParam(const string& par, char* buf, int buflen, short* isNull)
 *  设置SQL语句绑定参数，IN/OUT，接收char*型参数
 */
void UDBI::TTable::BindParam(const string& par, int& val, short* isNull)
{
	m_params.BindParam(par, val, isNull);
}
void UDBI::TTable::BindParam(const string& par, double& val, short* isNull)
{
	m_params.BindParam(par, val, isNull);
}
void UDBI::TTable::BindParam(const string& par, char* buf, int buflen, short* isNull)
{
	m_params.BindParam(par, buf, buflen, isNull);
}
//! 2005.12.20  WangFJ.
void UDBI::TTable::BindParam(const string& par, JCursor* pcur)
{
	m_params.BindParam(par, pcur);
}


/// 增补函数、用于数组参数绑定
void UDBI::TTable::SetParam(const string& par, int* pival, int arraylen, short* isNull)
{
	m_params.SetParam(par, pival, arraylen, isNull);
}

void UDBI::TTable::SetParam(const string& par, char* pcval, int clen, int arraylen, short* isNull)
{
	m_params.SetParam(par, pcval, clen, arraylen, isNull);
}

	
/**
 *  SQL语句prepare，本地化函数
 */
int UDBI::TTable::Prepare()
{
	//UDBI_TRACE("TTable::Prepare()");
	
    CHECKERR(m_errhp, OCIStmtPrepare(m_stmthp, m_errhp, (text *)m_sql.c_str(), (ub4)strlen(m_sql.c_str()), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT) );
    CHECKERR(m_errhp, OCIAttrGet((dvoid *)m_stmthp, (ub4)OCI_HTYPE_STMT, (dvoid *)&m_StmtType, (ub4)0, (ub4)OCI_ATTR_STMT_TYPE, m_errhp) );

    return (int)m_StmtType;
}

/**
 *  SQL语句执行
 */
int UDBI::TTable::Execute(int exTimes)
{
	UDBI_TRACE("TTable::Execute()");
	
	int rows_=0;
	
    if (m_StmtType == OCI_STMT_SELECT) {
        CHECKERR(m_errhp, OCIStmtExecute(m_svchp, m_stmthp, m_errhp, (ub4)0, (ub4)0, (CONST OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) );
    } else {
        CHECKERR(m_errhp, OCIStmtExecute(m_svchp, m_stmthp, m_errhp, (ub4)exTimes, (ub4)0, (CONST OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) ); 
        CHECKERR(m_errhp, OCIAttrGet(m_stmthp, (ub4)OCI_HTYPE_STMT, (ub4*)&rows_, (ub4*)0, (ub4)OCI_ATTR_ROW_COUNT, (OCIError*)m_errhp));
    }   
    
    return rows_;
}

/** 
 *  \fn int UDBI::TTable::ExecSelect(const string& selBy)
 *  SELECT语句执行函数
 *  @param selBy SQL语句引用标识，根据\a selBy 从SQL缓存中匹配对应的SQL语句
 */
int UDBI::TTable::ExecSelect(const string& selBy)
{
	UDBI_DEBUG_P1("TTable::ExecSelect(const string& selBy='%s')", selBy.c_str());
	
	if ( ! selBy.empty() ) {
		JSQL::getSQL(m_sql, m_TabName, selBy);
	}
	
    Prepare();
    m_params.Bind(m_stmthp, m_errhp);

    Execute();
    
    //! WangFJ. 2005.12.31  可能存在内存操作异常问题
    m_CurRec = 0;
    m_CanFetch = 0;
    m_LastFetchedRecs = 0;
    m_AllFetchedRecs = 0;

    //! WangFJ. 2005.12.18 防止连续执行jselect的内存泄漏
    m_records.Clear();
    m_records.init(m_stmthp, m_errhp, m_PreFetchRecs);
    m_records.Define(m_stmthp, m_errhp);
    
    return 0;
}

/** 
 *  \fn int UDBI::TTable::ExecInsert(const string& insBy)
 *  INSERT语句执行函数
 *  @param insBy SQL语句引用标识，根据\a insBy 从SQL缓存中匹配对应的SQL语句
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecInsert(const string& insBy)
{
	UDBI_DEBUG_P1("TTable::ExecInsert(const string& insBy='%s')", insBy.c_str());

	if ( ! insBy.empty() ) {
		//UDBI_DEBUG_P2("TTable::ExecInsert()[Tab:'%s', SelRef:'%s']", m_TabName.c_str(), insBy.c_str());
		JSQL::getSQL(m_sql, m_TabName, insBy);
	}

    Prepare();
    m_params.Bind(m_stmthp, m_errhp);

    return Execute();
}

/** 
 *  \fn int UDBI::TTable::ExecUpdate(const string& updBy)
 *  UPDATE语句执行函数
 *  @param updBy SQL语句引用标识，根据\a updBy 从SQL缓存中匹配对应的SQL语句
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecUpdate(const string& updBy)
{
	UDBI_DEBUG_P1("TTable::ExecUpdate(const string& updBy='%s')", updBy.c_str());

	if ( ! updBy.empty() ) {
		//UDBI_DEBUG_P2("TTable::ExecUpdate()[Tab:'%s', SelRef:'%s']", m_TabName.c_str(), updBy.c_str());
		JSQL::getSQL(m_sql, m_TabName, updBy);
	}

    Prepare();
    m_params.Bind(m_stmthp, m_errhp);

    return Execute();
}

/** 
 *  \fn int UDBI::TTable::ExecDelete(const string& delBy)
 *  DELETE语句执行函数
 *  @param delBy SQL语句引用标识，根据\a delBy 从SQL缓存中匹配对应的SQL语句
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecDelete(const string& delBy)
{
	UDBI_DEBUG_P1("TTable::ExecDelete(const string& delBy='%s')", delBy.c_str());

	string tmpStr(delBy);
	
	if ( ! delBy.empty() ) {
		//UDBI_DEBUG_P2("TTable::ExecDelete()[Tab:'%s', SelRef:'%s']", m_TabName.c_str(), delBy.c_str());
		JSQL::getSQL(m_sql, m_TabName, delBy);
	}

    Prepare();
    m_params.Bind(m_stmthp, m_errhp);

    return Execute();
}

/** 
 *  \fn int UDBI::TTable::ExecExt(int exTimes)
 *  扩展SQL语句执行方法，可以用于除select语句之外的所有SQL语句执行
 *  @param exTimes SQL语句执行次数，一般用于数组绑定情况
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecExt(int exTimes)
{
	UDBI_DEBUG_P1("TTable::ExecExt(int exTimes=%d)", exTimes);
	
	Prepare();
	m_params.Bind(m_stmthp, m_errhp);
	
	return Execute(exTimes);	
}

/** 
 *  \fn int UDBI::TTable::ExecExt(const string& extBy)
 *  扩展SQL语句执行方法，可以用于除select语句之外的所有SQL语句执行
 *  @param exTimes SQL语句执行次数，用于数组绑定情况
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecExt(const string& extBy)
{
	UDBI_DEBUG_P1("TTable::ExecExt(const string& extBy='%s')", extBy.c_str());
	
	if ( ! extBy.empty() ) {
		//UDBI_DEBUG_P2("TTable::ExecExt()[Tab:'%s', SelRef:'%s']", m_TabName.c_str(), updBy.c_str());
		JSQL::getSQL(m_sql, m_TabName, extBy);
	}

	Prepare();
	m_params.Bind(m_stmthp, m_errhp);
	
	return Execute();
}

/** 
 *  \fn int UDBI::TTable::ExecExt(const string& extBy, int exTimes)
 *  扩展SQL语句执行方法，可以用于除select语句之外的所有SQL语句执行
 *  @param extBy SQL语句引用标识，根据\a extBy 从SQL缓存中匹配对应的SQL语句
 *  @param exTimes SQL语句执行次数，用于数组绑定情况
 *  @return int 返回SQL语句执行影响记录数
 */
int UDBI::TTable::ExecExt(const string& extBy, int exTimes)
{
	UDBI_DEBUG_P2("TTable::ExecExt(const string& extBy='%s', int exTimes=%d)", extBy.c_str(), exTimes);
	
	if ( ! extBy.empty() ) {
		//UDBI_DEBUG_P2("TTable::ExecExt()[Tab:'%s', SelRef:'%s']", m_TabName.c_str(), updBy.c_str());
		JSQL::getSQL(m_sql, m_TabName, extBy);
	}

	Prepare();
	m_params.Bind(m_stmthp, m_errhp);
	
	return Execute(exTimes);
}

/**
 *  \fn bool UDBI::TTable::Fetch()
 *  查询结果Fetch函数，如果当前缓存仍有获取数据则移动记录指针，否则执行一次从数据库获取数据操作
 *  每次从数据库获取记录数由m_PreFetchRecs参数控制，该参数可以通过SetPrefetchRecs()方法设置。如
 *  果有结果数据则返回True，否则返回False
 *  @return bool 如果有结果，返回Ture，否则返回False
 */
bool UDBI::TTable::Fetch()
{
    UDBI_TRACE_P2("TTable::Fetch()[ARECS=%d, CURR=%d]", m_AllFetchedRecs, m_CurRec);
    
    unsigned tmpFetched=0;
    
    m_CurRec++;
    if (m_CurRec==m_LastFetchedRecs && m_LastFetchedRecs<m_PreFetchRecs ) {
        m_CanFetch = 0;
    } else if (m_CurRec==m_LastFetchedRecs ||!m_LastFetchedRecs) {
        OCIStmtFetch(m_stmthp, m_errhp, (ub4)m_PreFetchRecs, OCI_FETCH_NEXT, OCI_DEFAULT);
        tmpFetched = m_AllFetchedRecs;
        CHECKERR(m_errhp, OCIAttrGet(m_stmthp, OCI_HTYPE_STMT, &m_AllFetchedRecs, 0, OCI_ATTR_ROW_COUNT, m_errhp));
        m_LastFetchedRecs = m_AllFetchedRecs - tmpFetched;

		UDBI_DEBUG_P2("TTable::Fetch()[m_afr=%d, m_lfr=%d]", m_AllFetchedRecs, m_LastFetchedRecs);
		
        if (!m_LastFetchedRecs) {
             m_CanFetch = 0;
        } else {
            m_CanFetch = 1;
            m_CurRec = 0;
        }
    }

    return (m_CanFetch==1)?true:false;
}


/**
 *  \fn bool UDBI::TTable::isNull(const string& fldName)
 *  查询结果记录中指定字段值是否为空
 *  @return bool  如果为空，TRUE；不为空，FALSE
 */
/**
 *  \overload bool UDBI::TTable::isNull(const int fldNum)
 */
bool UDBI::TTable::isNull(const string& fldName)
{
	return m_records.isNull(fldName, m_CurRec);
}

bool UDBI::TTable::isNull(const int fldNum)
{
	return m_records.isNull(fldNum, m_CurRec);
}

/**
 *  \fn int UDBI::TTable::GetInt(const string& fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn int64_t UDBI::TTable::GetInt64(const string& fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int64_t 型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn std::string UDBI::TTable::GetString(const string& fldName, bool *ifNull)
 *  查询结果记录中指定字段值读取函数，返回字段的\c std::string 型值, 空返回SetNullText()方法设置的字符串, 缺省为空串
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 */
/**
 *  \overload int UDBI::TTable::GetInt(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload int64_t UDBI::TTable::GetInt64(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload std::string UDBI::TTable::GetString(const int fldNum, bool *ifNull)
 */ 
int UDBI::TTable::GetInt(const string& fldName, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
    return m_records.GetInt(fldName, m_CurRec, ifNull, nullValue);
}

int64_t UDBI::TTable::GetInt64(const string& fldName, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
	return m_records.GetInt64(fldName, m_CurRec, ifNull, nullValue);
}

std::string UDBI::TTable::GetString(const string& fldName, bool* ifNull)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
    return  m_records.GetString(fldName, m_CurRec, ifNull);
}

int UDBI::TTable::GetInt(const int fldNum, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetInt(fldNum, m_CurRec, ifNull, nullValue);
}

int64_t UDBI::TTable::GetInt64(const int fldNum, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetInt64(fldNum, m_CurRec, ifNull, nullValue);
}

std::string UDBI::TTable::GetString(const int fldNum, bool* ifNull)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetString(fldNum, m_CurRec, ifNull);
}

/**
 *  \fn int UDBI::TTable::GetFieldType(const string& fldName)
 *  元数据获取函数, 获取指定字段的数据类型
 */ 
/** 
 *  \fn int UDBI::TTable::GetFieldSize(const string& fldName)
 *  元数据获取函数, 获取指定字段的大小
 */
int UDBI::TTable::GetFieldType(const string& fldName)
{
    return m_records.GetFieldType(fldName);
}

int UDBI::TTable::GetFieldSize(const string& fldName)
{
    return m_records.GetFieldSize(fldName);
}

/**
 *  \fn void UDBI::TTable::Clear() 
 *  数据成员清空函数，在一个TTable实例下连续执行多个SQL语句时需要调用，如：
 *  \code
 *  JTable *pTab = pcon->CreateJTable();
 *  pTab->AddSql("select ... from .. where ...");
 *  pTab->ExecSelect();
 *  pTab->Fetch();
 *  pTab->GetString("fld_name");
 *  ...
 * 
 *  // 执行另一条SQL语句
 *  pTab->Clear();
 *  pTab->AddSql("select ... from .. where ...");
 *  pTab->ExecSelect();
 *  pTab->Fetch();
 *  pTab->GetInt("fld_name");
 *  ...
 *  \endcode
 */
void UDBI::TTable::Clear()
{
    //m_PreFetchRecs = 20;
    m_CurRec = 0;
    m_CanFetch = 0;
    m_LastFetchedRecs = 0;
    m_AllFetchedRecs = 0;
    
    //m_TabName.clear();
    m_StmtType = 0;
    m_sql.clear();

	m_params.Clear();
    m_records.Clear();
}	

string UDBI::TTable::parToString()
{
	return m_params.toString();
}

void UDBI::TTable::BindParams()
{
    //vector<Param *>::const_iterator iter=
	/*
	vector< TParam* >::const_iterator iter = m_params.begin();
	for ( ; iter != m_params.end(); iter++) {
		CHECKERR(m_errhp, OCIBindByName(m_stmthp, &(*iter)->bindhp, m_errhp, (text *)(*iter)->parName(), (sb4)strlen((*iter)->parName()),
						(dvoid *)(*iter)->pval(), (sb4)(*iter)->valLen(), (ub2)(*iter)->reqType(), (dvoid *)((*iter)->indp()),
						(ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT) );
    }
    */
}

///
int UDBI::TTable::GetFieldCount()
{
	return m_records.GetFieldCount();
}

string UDBI::TTable::GetFieldName(int fldNum)
{
	string strFieldName;
	m_records.GetFieldName(strFieldName, fldNum);
	return strFieldName;
}

