/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：udbi.cpp
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

#include <stdio.h>
#include <time.h>
#include <iostream>
#include "udbi-inter.h"

using namespace UDBI;

static int g_jsqlBug=0;

inline const char* bugToStr(UDBI_DLEV bug) {
	static const char* __bug__str[] = {
		"FATAL",
		"ERROR",
		"DEBUG",
		"TRACE",
		"INFO"
	};
	
	return __bug__str[bug];
}

static void hostTime2Asc(char sTime[])
{
	time_t tTime;
	struct tm *pTm;
	
	time(&tTime);
	pTm = localtime(&tTime);
	
	sprintf(sTime, "%02d:%02d:%02d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
} 

void UDBI::setJSqlBug(const int val)
{
	g_jsqlBug = val;
}
	
void UDBI::DDEBUG_CONSOLE(UDBI_DLEV level, const char* file, int line, const char* msg, ...) {
	if ((level<=UDBI_DEBUG_LEVEL)&&(g_jsqlBug==JSQL_BUG_OPEN)) {
		va_list ap;
		char sMsg[2048]="";
		char sTime[10]="";
		
		va_start(ap, msg);
		vsprintf(sMsg, msg, ap);
		va_end(ap);
		
		hostTime2Asc(sTime);
		cout << sTime << " " << bugToStr(level) << " [UDBI] " << sMsg << "  " << file << ":" << line << endl;
	}
}

void UDBI::DDEBUG_FILE(UDBI_DLEV level,const char* file, int line, const char* fmt, ...) {
	if ((level<=UDBI_DEBUG_LEVEL)&&(g_jsqlBug==JSQL_BUG_OPEN)) {
		FILE* fp;
		va_list ap;
		char sMsg[2048]="";
		char sTime[10]="";

		fp=fopen("udbi_log.log", "a+");
		
		va_start(ap, fmt);
		vsprintf(sMsg, fmt, ap);
		va_end(ap);
		
		hostTime2Asc(sTime);
		fprintf(fp, "%s %s [UDBI] %s %s:%d \n", sTime, bugToStr(level), sMsg, file, line);
		fflush(fp);
		fclose(fp);
	}
}
/*
void UDBI::CHECKERR(OCIError* err, sword status) {
	if (status!=OCI_SUCCESS) {
		sb4 m_errcode;
		text* errText;
		errText=(text *)calloc(512,sizeof(text));
		(void) OCIErrorGet((dvoid *)err, (ub4) 1, (text *) NULL, &m_errcode, errText, 512, OCI_HTYPE_ERROR);
		errText[511]='\0';
		string tmpStr((char *)errText);
		free(errText);
		
		//cout << "|" << m_errcode << tmpStr << endl;
	
		THROW_AC(EUdbiException, OracleException(m_errcode, tmpStr), 90009, "OCI 调用异常");
	}
}
*/
void UDBI::version(void) {
	printf("%d.%d.%d\n", UDBILIBMAJOR, UDBILIBMINOR, UDBILIBMICRO);
}

/**
 *  \mainpage 统一数据库访问接口
 *  
 *  \section sec_intro 综述
 *  统一数据库访问接口组件（UDBI）封装数据库应用编程需要的连接、SQL语句执行、参数绑定、结果访问
 *  等功能，分别对应类JConnect、JTable、JParam、JRecord。并定义数据库应用编程环境初始化的静态类
 *  jdb。UDBI 的实现类图如下：
 *  \image HTML  udbi_class.jpg
 * 
 *  \section sec_apply  引用
 *  UDBI组件将以静态库(.a)形式发布，静态库名字为libudbi.a。使用UDBI组件的应用需要做以下工作：
 *  \arg 包含头文件udbi.h (#include "udbi.h")
 *  \arg 编译时连接libudbi.a ( -ludbi )
 *  
 *  其中udbi.h 中有几个参数会影响编译过程：
 *  \arg DEBUG_LEVEL：DEBUG级别，UDBI中定义了 FATAL，ERROR，DEBUG，INFO 四个DEBUG级别
 *  \arg OCILIBVER：Oracle OCI库的版本号，由于OracleOCI库不同版本之间会有一些小的差异，所以编译之前需要正确指定版本号
 * 
 */  

/**
 *  \example demo_tab.cpp  
 *  UDBI 组件的测试及样例程序
 */
/**
 *  \example demo_dao.cpp
 *  dao 组件的测试程序
 */
/**
 *  \example demo_svc.cpp
 *  tuxedo 服务测试程序
 */
/**
 *  \example demo_client.cpp
 *  tuxedo 服务客户端程序
 */

// ◆◇◇＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
/**
 *  \class UDBI::JConnect  JConnect.h
 *  \brief 数据库连接接口类
 *  
 *  JConnect 抽象了数据库连接相关接口，包括连接数据库、断开连接、事务控制、创建
 *  JTable 实例等。
 *  JConnect 接口实例通过静态类jdb的Connect方法或单件实例获得，有以下几种形式：
 *  
 *  \par 本地数据库连接
 *  \code
 *  jdb::init();
 *  JConnect *pcon = jdb::Connect(tns, user, password);
 *  \endcode
 *
 *  \par XA数据库连接
 *	\code
 *  jdb::initXa();
 *  JConnect *pcon = jdb::ConnectXA();
 *  \endcode
 *
 *  \par 单件实例，本地数据库连接
 *	\code
 *  jdb::jinit(tns, user, password);
 *  JConnect *pcon = jdb::geJConnect();
 *	\endcode
 *
 *  \par 单件实例，XA数据库连接
 *	\code
 *  jdb::jinitXa(tns, user, password);
 *  JConnect *pcon = jdb::geJConnect();
 *	\endcode
 */

/**
 *  \fn UDBI::JConnect::~JConnect() 
 *  析构函数，断开数据库连接，释放资源
 */

/**
 *  \fn void UDBI::JConnect::Connect(const string &cTns, const string &cUser, const string &cPassword)
 *  数据库连接函数，在初始化的OCIEnv *下，使用\a cTns, \a cUser, \a cPassword 参数建立本地数据库连接
 */

/**
 *  \fn void UDBI::JConnect::ConnectXA()
 *  数据库连接函数，在初始化的OCIEnv *下，调用xa函数建立XA数据库连接
 */

/**
 *  \fn void UDBI::JConnect::DisConnect()
 *  断开本地数据库连接，释放资源（包括会话、事务、服务连接等）
 */

/**
 *  \fn void UDBI::JConnect::TransBegin()
 *  开启事务，非XA 方式下的事务控制
 */

/**
 *  \fn void UDBI::JConnect::TransCommit() 
 *  提交事务，非XA 方式下的事务控制
 */

/**
 *  \fn void UDBI::JConnect::TransRollback()
 *  回滚事务，非XA 方式下的事务控制
 */

/**
 *  \fn bool UDBI::JConnect::isTrans() 
 *  UDBI事务判断，事务中返回TRUE，否则返回FALSE
 */

/**
 *  \fn UDBI::JTable *UDBI::JConnect::CreateJTable(const int prefetchRecs=UDBI_PREFETCH_RECS)
 *  创建JTable接口
 */

/**
 *  \fn UDBI::JTable *UDBI::JConnect::CreateJTable(const string &sTabName, const int prefetchRecs=UDBI_PREFETCH_RECS)
 *  创建JTable接口，并用sTabName参数初始化
 */

// ◆◇◇＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
/** \class UDBI::JTable   JTable.h
 *  \brief SQL语句执行接口类
 * 
 *  JTable 抽象了数据库访问需要的接口方法，包括设置参数、执行SQL语句、访问
 *  查询结果等。通过JTable抽象类隐藏了数据库访问的实现细节，层次清晰，使用
 *  方便。
 * 
 *  \par 参数绑定
 *  正常情况下，所有SQL语句的输入参数值都是在运行时刻给定的，JTable 抽象的
 *  参数设置函数提供了简单的参数传入接口，目前支持以下类型参数：
 *  \arg int &
 *  \arg char []
 *  \arg string &
 *  \arg int []
 *  \arg char **
 * 
 *  通常情况下，大部分参数都是输入参数，但对存储过程和SELECT INTO语句，亦需
 *  要输出参数。根据数据库编程接口的要求，所有参数在SQL语句执行过程中都应该
 *  存在，故应该保证所有给定参数值/变量，都应该与SQL语句的执行在同一代码段中。
 * 
 *  \par 查询结果访问  
 *  对查询语句，在执行之后需要访问查询到的结果数据，JTable抽象GetXX()类函数
 *  提供此类功能。在引用结果集数据时，一般先要执行fetch操作，每次fetch操作，
 *  JTable都会获取一条记录，直到返回空。GetXX()类函数通过输入字段名访问记录
 *  中的字段结果。目前支持以下结果数据类型：
 *  \arg 整型（int）：对应Oracle数据库的\c number(0-11)
 *  \arg 长整型（int64_t）：对应Oracle数据库的\c number(0-18)
 *  \arg 字符串型（string）：对应Oracle数据库的\c char, varchar2, number(12-22), date
 *
 *  查询结果访问的代码样例如下：
 *  \code
 *  while(jTab->Fetch()) 
 *  {
 *      jTab->GetInt('fld_name');
 *      jTab->GetInt64('fld_name');
 *      jTab->GetString('fld_name');
 *      ...
 *  }
 *  \endcode
 *
 *  \par SQL语句缓存
 *  通常的数据库访问应用都是在程序中明确拼写SQL语句，并通过数据库访问组件执行。
 *  但这种方式有一些缺点，一是由于SQL语句分散在各程序中并完全由程序员确定，无法
 *  保证SQL语句的合理复用，如对同一个表的相同条件的查询语句，将会根据使用场景的
 *  不同而有N 多，这将带来极大的维护困难（如SQL语句优化等等），并严重违背了复用
 *  的思想。二是如果有SQL语句调整时，需要修改并编译程序，这将影响业务系统运行。
 * 
 *  \par 
 *  对于BOSS1.5 研发项目特别定义了SQL语句集中存放，并支持动态修改。UDBI组件中封装
 *  了JSQL对象实现SQL语句的匹配查询和缓存功能。并在JTable类的所有SQL语句执行方法中
 *  定义了一个SQL语句引用标识参数。如果输入标识为空，则JTable执行AddSql()方法拼接
 *  的SQL语句，否则将根据输入标识从SQL缓存或数据库中匹配对应的SQL语句并执行。此功能
 *  使得数据库访问接口逻辑简洁、清晰，SQL语句集中易于维护和升级。
 * 
 *  \par一个简单的例子：
 *  \code
 * 
 *  jdb::init();
 *  JConnect *pcon = jdb::Connect(tns, user, password);
 * 
 *  JTable *pTab = pcon->CreateJTable();
 *  pTab->AddSql("select table_name, owner, constraint_name, constraint_type from user_constraints");
 *  pTab->ExecSelect();
 *  while( pTab->Fetch() ) {
 *     cout << pTab->GetString("owner") << "." << pTab->GetString("constraint_name") << endl;
 *  }
 *  delete pTab;
 *
 *  or
 *  // 使用SQL缓存
 *  int iStaffID;
 *  iStaffID = 110001;
 * 
 *  JTable *pTab = pcon->CreateJTable("TD_M_STAFF");
 *  pTab->SetParam(":VSTAFF_ID", iStaffID);
 *  pTab->ExecSelect("SEL_BY_PK");
 *  while (pTab->Fetch()) 
 *  {
 *     cout << pTab->GetString("DEPART_ID") << endl;
 *     cout << pTab->GetString("STAFF_NAME") << endl;
 *     cout << pTab->GetString("SEX") << endl;
 *     cout << pTab->GetString("UPDATE_TIME") << endl;
 *     ...
 *  }
 * 
 *  \endcode
 */
        
/**
 *  \fn void UDBI::JTable::SetNullText(const string &nullText)
 *  设置字符串字段在查询结果为空时返回的值
 */
 
/**
 *  \fn void UDBI::JTable::SetPrefetchRecs(const int prefetchRecs)
 *  设置每次从数据库获取记录数
 */
                  
/** 
 *  \fn void UDBI::JTable::AddSql(const string &sql)
 *  增加SQL语句函数，可以连续调用本函数拼接SQL语句（如SQL语句比较长的情况下）
 */
 
/**
 *  \fn UDBI::JTable::~JTable()
 *  析构函数，释放资源
 */

/** 
 *  \fn void UDBI::JTable::AddSql(const string &sql)
 *  增加SQL语句函数，可以连续调用本函数拼接SQL语句（如SQL语句比较长的情况下）
 */

/**
 *  \fn void UDBI::JTable::SetParam(const string &par, char buf[], int buflen, short *isNull)
 *  设置SQL语句绑定参数，接收char [] 型参数
 */

/** 
 *  \fn void UDBI::JTable::SetParam(const string &par, int &val, short *isNull)
 *  设置SQL语句绑定参数，接收int & 型参数
 */

/**
 *  \fn void UDBI::JTable::SetParam(const string &par, string &val, short *isNull)
 *  设置SQL语句绑定参数，接收string & 型参数
 */

/**
 *  \fn void UDBI::JTable::SetParam(const string &par, int aval[], int alen)
 *  设置SQL语句绑定参数，接收int [] 型参数
 */

/**
 *  \fn void UDBI::JTable::SetParam(const string &par, char ** abuf, int buflen, int alen) 
 *  设置SQL语句绑定参数，接收char ** 型参数
 */

/** 
 *  \fn int UDBI::JTable::ExecSelect(const string &selBy)
 *  SELECT语句执行函数
 *  @param selBy SQL语句引用标识，根据\a selBy 从SQL缓存中匹配对应的SQL语句
 */

/** 
 *  \fn int UDBI::JTable::ExecInsert(const string &insBy)
 *  INSERT语句执行函数
 */

/** 
 *  \fn int UDBI::JTable::ExecUpdate(const string &updBy)
 *  UPDATE语句执行函数
 *  @param updBy SQL语句引用标识，根据\a updBy 从SQL缓存中匹配对应的SQL语句
 */

/** 
 *  \fn int UDBI::JTable::ExecDelete(const string &delBy)
 *  DELETE语句执行函数
 *  @param delBy SQL语句引用标识，根据\a delBy 从SQL缓存中匹配对应的SQL语句
 */

/**
 *  \fn bool UDBI::JTable::Fetch()
 *  查询结果Fetch函数，如果当前缓存仍有获取数据则移动记录指针，否则执行一次从数据库获取数据操作
 *  每次从数据库获取记录数由m_PreFetchRecs参数控制，该参数可以通过SetPrefetchRecs()方法设置。如
 *  果有结果数据则返回True，否则返回False
 *  @return bool 如果有结果，返回Ture，否则返回False
 */
 
 /**
 *  \fn int UDBI::JTable::FetchToFml(CFmlBuf *pfmlBuf)
 *  把本次查询的所有结果记录直接获取并put 到传入FmlBuf指针指向的缓存中
 *  如果查询结果为空返回0，否则返回实际的记录数
 */

/**
 *  \fn bool UDBI::JTable::isNull(const string &fldName)
 *  查询结果记录中指定字段值是否为空
 *  @return bool  如果为空，TRUE；不为空，FALSE
 */
/**
 *  \overload bool UDBI::JTable::isNull(const int fldNum)
 */

/**
 *  \fn int UDBI::JTable::GetInt(const string &fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn int64_t UDBI::JTable::GetInt64(const string &fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int64_t 型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn std::string UDBI::JTable::GetString(const string &fldName, bool *ifNull)
 *  查询结果记录中指定字段值读取函数，返回字段的\c std::string 型值, 空返回SetNullText()方法设置的字符串, 缺省为空串
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 */
/**
 *  \overload int UDBI::JTable::GetInt(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload int64_t UDBI::JTable::GetInt64(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload std::string UDBI::JTable::GetString(const int fldNum, bool *ifNull)
 */ 

 
/**
 *  \fn int UDBI::JTable::GetFieldType(const string &fldName)
 *  元数据获取函数, 获取指定字段的数据类型
 */ 
/** 
 *  \fn int UDBI::JTable::GetFieldSize(const string &fldName)
 *  元数据获取函数, 获取指定字段的大小
 */


/**
 *  \fn void UDBI::JTable::Clear() 
 *  数据成员清空函数，在一个JTable实例下连续执行多个SQL语句时需要调用，如：
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

