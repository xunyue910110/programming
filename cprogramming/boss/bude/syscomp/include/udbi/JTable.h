/** 
 * @file JTable.h
 * SQL语句操作接口类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @bug	2004.12.22	WangFJ	增加isNull(const string &fldName) 方法
 *						ExecInsert() 方法增加const string &insBy 参数
 * @bug	2005.01.25	WangFJ	去掉对查询结果直接导入FmlBuf的支持
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __JTABLE_H_
#define __JTABLE_H_

#include <string>
#include <vector>

namespace UDBI {
using std::string;
using std::vector;

//! SQL语句操作接口类，封装参数设置、语句执行、查询结果访问功能
class JTable {
public:
	
	//! 析构函数
	virtual ~JTable() {};
	
	//! 设置从数据库端获取数据时每次获取记录数
	virtual void SetPrefetchRecs(const int prefetchRecs)=0;
	
	//! 设置查询结果中字符型字段为空时返回的值（对所有字符型字段）
    virtual void SetNullText(const string& nullText)=0;
    
    //! 添加SQL语句
    virtual void AddSql(const string& sql)=0;
    virtual string GetSql(void)=0;
	    	
	//! 参数设置函数，支持int、double、string、vector<int>、vector<string>类型参数	    	
    virtual void SetParam(const string& par, int val, short* isNull=0)=0;
    virtual void SetParam(const string& par, double val, short* isNull=0)=0;
    virtual void SetParam(const string& par, const string& val, short* isNull=0)=0;
    //! 系统由vector<>.size()自动记录数组纬度
    //! 对vector<string>注意len设置范围为‘最大实际字符串长度+1 ～ 对应表字段长度+1’
    virtual void SetParam(const string& par, vector<int>& vval, short* isNull=0)=0;
    virtual void SetParam(const string& par, vector<string>& vval, int len, short* isNull=0)=0;

    virtual void BindParam(const string& par, int& val, short* isNull=0)=0;
    virtual void BindParam(const string& par, double& val, short* isNull=0)=0;
    virtual void BindParam(const string& par, char* buf, int buflen, short* isNull=0)=0;
    
    /// 增补函数、用于数组参数绑定
    virtual void SetParam(const string& par, int* pival, int arraylen, short* isNull=0)=0;
    virtual void SetParam(const string& par, char* pcval, int clen, int arraylen, short* isNull=0)=0;

    
    //! SQL 语句执行函数，执行增、删、改、查
    //! 如果需要引用缓存的SQL语句则需要传入xxBy 参数，缺省为空，则执行AddSql设置的SQL语句
    //! 如同时传入xxBy参数和通过AddSql设置了SQL语句，则执行xxBy引用到的SQL语句
    virtual int ExecSelect(const string& selBy="")=0;
    virtual int ExecUpdate(const string& updBy="")=0;
    virtual int ExecDelete(const string& delBy="")=0;
    virtual int ExecInsert(const string& insBy="")=0;
    
    //! 扩展的SQL语句执行函数，可以用于执行除select外的其他的所有语句，
    //! 如update、delete、insert、call proc等，exTimes为执行次数
    virtual int ExecExt(int exTimes)=0;
    virtual int ExecExt(const string& extBy="")=0;
	virtual int ExecExt(const string& extBy, int exTimes)=0;

	//! 从数据库端获取数据，每次fetch一条记录，没有数据时返回False
    virtual bool Fetch()=0;
    
    //! 元数据信息获取，已实现字段的数据类型、字段大小
    virtual int GetFieldType(const string& fldName)=0;
    virtual int GetFieldSize(const string& fldName)=0;

	//! 判断查询结果中当前记录的指定字段是否为空
    virtual bool isNull(const string& fldName)=0;
    virtual bool isNull(const int fldNum)=0;

	//! GET查询结果中字段值，目前实现返回int, int64_t, string型值
    virtual int GetInt(const string& fldName, bool *ifNull=0, const int nullValue=-1)=0;
    virtual int64_t GetInt64(const string& fldName, bool *ifNull=0, const int nullValue=-1)=0;
    virtual string GetString(const string& fldName, bool *ifNull=0)=0;
    
    virtual int GetInt(const int fldNum, bool *ifNull=0, const int nullValue=-1)=0;
    virtual int64_t GetInt64(const int fldNum, bool *ifNull=0, const int nullValue=-1)=0;
    virtual string GetString(const int fldNum, bool *ifNull=0)=0;

	//! 清空函数
    virtual void Clear()=0;

	//! 参数调试信息输出
	virtual string parToString()=0;
	
};

} // namespace UDBI


#endif /* __JTABLE_H_ */
