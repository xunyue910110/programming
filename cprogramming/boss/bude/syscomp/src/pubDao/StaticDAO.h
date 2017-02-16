#ifndef _STATIC_DAO_H__
#define _STATIC_DAO_H__

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
class StaticDAO : public MTable {
public:
	//!
	static StaticDAO& getDAO();

	//!
	long jselect(CFmlBuf& buf, const string& sqlRef, const string& tabName);
	long jselect(CFmlBuf& buf, const string& sqlRef, const string& tabName, const string& swhere);

	//!
	long jselpage(CFmlBuf& buf, const string& sqlRef, const string& tabName, int page, int rowsPerPage);

	//!
	long jcount(const string& sqlRef, const string& tabName);
	long jcount(const string& sqlRef, const string& tabName, const string& swhere);

	//!
	long jupdate(const string& sqlRef, const string& tabName);
	long jdelete(const string& sqlRef, const string& tabName);
	long jinsert(const string& sqlRef, const string& tabName, int size=0);

private:
	//!
	StaticDAO():MTable("") { }

};

#endif

