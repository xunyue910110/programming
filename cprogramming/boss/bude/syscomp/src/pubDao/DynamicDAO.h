#ifndef _SINGLE_DAO_H__
#define _SINGLE_DAO_H__

#include <string>

#include "base/config-all.h"
#include "udbiEx/udbi.h"

using std::string;
using namespace UDBI;


///
class CFmlBuf;

/**
 * 
 */
class DynamicDAO : public MTable {
public:
	DynamicDAO(const string& tabName)
		:MTable(tabName), __tabName(tabName), __appendWhere(" WHERE (1=1) ") { }

	virtual ~DynamicDAO() { }

	//!
	void Clear()
	{
		__appendWhere.clear();
		__appendUpdate.clear();
		MTable::Clear();
		__appendWhere = " WHERE (1=1) ";
	}

	//!
	void appendUpdate(const string& fldName, const string& value, int type=0);
	void appendUpdate(CFmlBuf& buf, const string& fldName, int type=0);
	
	void appendWhere(const string& andclause);
	void appendWhere(CFmlBuf& buf, const string& fldName, int type=0);
	
	long xselect(CFmlBuf& buf, const string& sqlRef);
	long xupdate(const string& sqlRef);

private:
	//!
	string __tabName;
	
	//!
	string __appendWhere;
	string __appendUpdate;

	//! 拷贝构造函数与赋值运算符，未实现，限制调用
	DynamicDAO(const DynamicDAO& rhs);
	DynamicDAO& operator=(const DynamicDAO& rhs);

};

#endif

