/** 
 * @file JRecord.h
 * JRecord查询结果类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */

#ifndef __JRECORD_H_
#define __JRECORD_H_

#include <map>
#include "udbi-inter.h"
#include "JField.h"

namespace UDBI {

using std::string;
using std::map;

class JRecord {
public:
	JRecord();
	JRecord(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs=1);
	virtual ~JRecord();
	
	//! 记录包含字段数
	int GetFieldCount() const { return m_nFields; }

	//! 初始化函数，创建接收OCIStmt语句查询记录的字段变量
	void init(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs=1);
	
	//! 接收字段变量与SQL语句关联
	void Define(OCIStmt* vpStmt, OCIError* vpErr);
	
	//! 记录元数据信息查询
	void GetFieldName(string& vFieldName, int fldNum) const;
	int GetFieldSize(int fldNum) const;
	int GetFieldSize(const string& fldName) const;
	int GetFieldType(int fldNum) const;
	int GetFieldType(const string& fldName) const;
	void GetFieldTypeName(string& vFieldTypeName, int fldNum) const;
	void GetFieldTypeName(string& vFieldTypeName, const string& fldName) const;
	
	//! 设置查询结果中字符型字段为空时的返回值
	void SetNullText(const string& nullText);
	
	//! 判断查询结果中指定记录的指定字段是否为空
	bool isNull(int fldNum, int recNum) const;
	bool isNull(const string& fldName, int recNum) const;

	//! GET查询结果中指定记录的指定字段的结果值
    int GetInt(const string& fldName, int recNum, bool* ifNull=0, const int nullValue=0);
	int64_t GetInt64(const string& fldName, int recNum, bool* ifNull=0, const int nullValue=0);
	string GetString(const string& fldName, int recNum, bool* ifNull=0);

    int GetInt(int fldNum, int recNum, bool* ifNull=0, const int nullValue=0);
    int64_t GetInt64(int fldNum, int recNum, bool* ifNull=0, const int nullValue=0);
    string GetString(int fldNum, int recNum, bool* ifNull=0);

	//! 记录定义清空函数    
    void Clear();
	
private:
	map<string, int> m_fields_map;
	string m_nullText;
	int m_nFields;
	JField **m_ppFields;
	OCIStmt* m_stmthp;
	OCIError* m_errhp;
	
};


} // namespace UDBI


#endif /* __JRECORD_H_ */
