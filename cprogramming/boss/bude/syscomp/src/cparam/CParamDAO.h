/** 
 * @file CParamDAO.h
 * 参数缓存类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __CPARAMDAO_H_
#define __CPARAMDAO_H_

#include <iostream>
#include <map>

using namespace std;

/**
 *  参数保存数据结构
 */
struct EData {
	struct EData *pNext;
	string value1, value2, value3, vresult;
};

/**
 * @class CParam	CParamDAO.h
 * \brief 参数类，以链表结构缓存一个键值下的所有参数
 */
class CParam {
private:
	EData *m_head;
	
protected:
	//! 拷贝构造函数，未实现，限制调用
	CParam(const CParam& rhs);
	
	//! 赋值运算符重载函数，未实现，限制调用
	const CParam& operator=(const CParam& rhs);
	
public:

	//! 构造函数
	CParam(): m_head(0) {};
	
	//! 析构函数
	virtual ~CParam();
	
	//! 加载参数数据，一个键值一个value、一个键值二个value
	void add(const string &val1, const string& val2, const string& vres);
	
	//! 加载参数数据，一个键值三个value
	void add(const string& val1, const string& val2, const string& val3, const string& vres);
	
	//! 匹配参数，一个键值一个value
	EData *find(const string& value1, string& vresult);
	
	//! 匹配参数，一个键值二个value
	EData *find(const string& value1, const string& value2, string& vresult);
	
	//! 匹配参数，一个键值三个value
	EData *find(const string& value1, const string& value2, const string& value3, string& vresult);

};


typedef counted_ptr<CParam> PCParam;
typedef map<string, PCParam> PCParams;


/**
 *  @class CParamDAO	CParamDAO.h
 *  \brief 参数DAO类，实现参数加载、缓存、匹配
 *
 *  参数DAO有两种用法：
 *  1. 应用初始化时调用参数DAO的Load()方法加载数据，之后getParam()方法即从缓存中取数据；
 *  2. 直接调用getParam()方法，如果从缓存中匹配不到数据，则从数据库中读取数据、缓存、返回匹配数据；
 *  当然也可以以上两种方式结合使用，即如果从已缓存数据中未匹配到结果，再试图从数据库中读取一次。
 *
 */
class CParamDAO {
private:
	static PCParams m_params;
	static PCParam insCParam(const string& key);
	static PCParam insCParamExt(const string& key);

public:

	//! 参数加载，从数据库中读取参数数据并缓存
	static void Load();

	//! 卸载缓存
	static void UnLoad();

	//! 键值加一个value匹配参数
	static string getParam(const string& key, const string& value1);

	//! 键值加二个value匹配参数
	static string getParam(const string& key, const string& value1, const string& value2);

	//! 键值加三个value匹配参数
	static string getParam(const string& key, const string& value1, const string& value2, const string& value3);

};

class CFmlBuf;

/**
 *  @class ConvParam CParamDAO.h
 *  \brief CODE-TO-NAME 转换类
 *
 *	根据fromFldName 从buf 中get 出对应的CODE 值，将该CODE 值
 *  与设定的键值和value 一起从参数缓存中匹配出NAME，在把NAME
 *  用toFldName 放入buf
 *
 *  使用样例如下：
 *	\code
 *  ConvParam  convParam("RES_KIND");
 *  convParam.SetParam(strTradeEparchyCode);
 *  convParam.SetParam(strResTypeCode);
 *  convParam.CodeToName(outBuf, "RES_KIND", "RES_KIND_NAME");
 * 
 *  convParam.SetKey("RES_KIND");
 *  convParam.SetParam(strTradeEparchyCode);
 *  convParam.SetParam(strResTypeCode);
 *  convParam.CodeToName(outBuf, "RES_KIND", "RES_KIND_NAME");
 *  \endcode
 */
class ConvParam {
public:
	//! 缺省构造函数
	ConvParam(): m_cnt(0) { }
	
	//! 构造函数
    ConvParam(const string& skey) : key(skey), m_cnt(0) { }
    
    //! 设定键值
    void SetKey(const string& skey) { key = skey; Clear(); }

	//! 设定value
    void SetValue(const string& value)
    {
        m_cnt++;
        if (m_cnt == 1) { value1 = value; } 
        else if (m_cnt == 2) { value2 = value; } 
        else m_cnt = -1;
    }

	//! 转换
    int CodeToName(CFmlBuf& buf, const char* fromFldName, const char* toFldName);

protected:
    ConvParam(const ConvParam& rhs);
    const ConvParam& operator=(const ConvParam& rhs);
    
    string key, value1, value2, value3;
    int m_cnt;

private:

    void Clear() { m_cnt = 0; value1.clear(); value2.clear(); value3.clear(); }

};


/**
 *  参数编码-名称转换函数
 *  1. 根据输入的fromFldName 从buf 中找到编码值;
 *  2. 以key 和编码值为条件从参数缓存中查询名称;
 *  3. 在把名称以toFldName 放入buf;
 */

int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName, const char* toFldName);

//Add by Gemj Begin@2005-4-6 17:16
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName1, const char* fromFldName2, const char* toFldName);
//Add by Gemj End@2005-4-6 17:23

#endif  /* __CPARAMDAO_H_ */
