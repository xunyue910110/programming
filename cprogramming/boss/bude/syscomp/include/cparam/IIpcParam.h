#ifndef __IIPCPARAM_H_
#define __IIPCPARAM_H_

#include <string>
#include <iostream>

#include "IpcParamdef.h"

using std::string;
using std::ostream;
using std::cout;


/**
 *  Usage Example:
 *
 *	IIpcParam::initialize();
 *		
 *  	// MODE-1:
 *		IIpcParam& ipar = IIpcParam::getIpcParam();
 *  	string srslt1 = ipar.getParam(key, value1, value2);
 *  	string srslt2 = ipar.getParam(key, value1, value2, value3);
 *
 *  	// MODE-2:
 *  	IIpcParam& ipar = IIpcParam::getIpcParam();
 *  	lckid = ipar.beginLock2(key);
 *      try
 * 		{
 *			for (... ) {
 *  			srslt = ipar.getTrans2(value1, value2);
 *				...
 *  		}
 *		} catch ( Exception& er)
 *		{
 *			ipar.endLock2(lckid);
 *			throw;
 *		}
 *      ipar.endLock2(lckid);
 *
 *  IIpcParam::uninitialize();
 *
 */
class IIpcParam {
public:

	/// static method

	static void initialize();
	static void uninitialize();
	
	static IIpcParam& getIpcParam();
	

	/// interface

	virtual ~IIpcParam() { };
	
	virtual int load(const string& key) = 0;
	virtual int refresh(const string& key) = 0;
	
	// for maintain
	virtual int status(ostream* osptr = &cout) = 0;
	virtual int dump(const string& key, ostream* osptr = &cout) = 0;

	virtual TSegHead headinfo(int outflag = 0, ostream* osptr = &cout) = 0;
	virtual TParIndex keyinfo(const string& key, int outflag = 0, ostream* osptr = &cout) = 0;

	virtual int recCnt(const string& key) = 0;
	virtual int recCopy(const string& key, TParRecord** pp) = 0;
	
	// MODE-1: for common code-to-name use
	virtual string getTrans(const string& key, const string& val1) = 0;
	virtual string getTrans(const string& key, const string& val1, const string& val2) = 0;
	virtual string getTrans(const string& key, const string& val1, const string& val2, const string& val3) = 0;

	// MODE-2: for the case of one key but too many records
	virtual int beginLock2(const string& key) = 0;
	virtual int endLock2(int lockid) = 0;
	virtual string getTrans2(const string& val1) = 0;
	virtual string getTrans2(const string& val1, const string& val2) = 0;
	virtual string getTrans2(const string& val1, const string& val2, const string& val3) = 0;
	
};


#endif
