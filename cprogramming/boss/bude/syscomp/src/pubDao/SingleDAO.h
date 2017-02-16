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
class SingleDAO : public MTable {
public:
	SingleDAO(const string& tabName):MTable(tabName), __tabName(tabName) { }

	virtual ~SingleDAO() { }

	//!
	long jselect(CFmlBuf& buf, const string& sqlRef);
	long jselect(CFmlBuf& buf, const string& sqlRef, const string& swhere);

	//!
	long jselecta(CFmlBuf& buf, const string& sqlRef);
	long jselecta(CFmlBuf& buf, const string& sqlRef, const string& swhere);
	
	//!
	long jselpage(CFmlBuf& buf, const string& sqlRef, int page, int rowsPerPage);

	//!
	long jcount(const string& sqlRef);
	long jcount(const string& sqlRef, const string& swhere);

	//!
	long jupdate(const string& sqlRef);
	long jdelete(const string& sqlRef);
	long jinsert(const string& sqlRef, int size=0);

private:
	//!
	string __tabName;
	
	//! 拷贝构造函数与赋值运算符，未实现，限制调用
	SingleDAO(const SingleDAO& rhs);
	SingleDAO& operator=(const SingleDAO& rhs);

};

#endif

