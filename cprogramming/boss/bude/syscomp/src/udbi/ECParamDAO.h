/** 
 * @file CParamExtDAO.h
 * 参数缓存类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __ECPARAMDAO_H_
#define __ECPARAMDAO_H_

#include <iostream>
#include <map>

using namespace std;

/**
 *  参数保存数据结构
 */
struct EEData {
	struct EEData *pNext;
	string value1, value2, value3, vresult;
};

/**
 * @class CParam	CParamDAO.h
 * \brief 参数类，以链表结构缓存一个键值下的所有参数
 */
class ECParam {
private:
	EEData *m_head;
	EEData *m_head_ext;
	
protected:
	//! 拷贝构造函数，未实现，限制调用
	ECParam(const ECParam& rhs);
	
	//! 赋值运算符重载函数，未实现，限制调用
	const ECParam& operator=(const ECParam& rhs);
	
public:

	//! 构造函数
	ECParam(): m_head(0), m_head_ext(0) {};
	
	//! 析构函数
	virtual ~ECParam();
	
	//!
	void erase();
	
	//! 加载参数数据，一个键值三个value
	void add(const string& val1, const string& val2, const string& val3, const string& vres);
	
	//! 加载参数数据，一个键值三个value
	void addext(const string& val1, const string& val2, const string& val3, const string& vres);

	//! 匹配参数，一个键值一个value
	EEData *find(const string& value1, string& vresult);
	
	//! 匹配参数，一个键值二个value
	EEData *find(const string& value1, const string& value2, string& vresult);
	
	//! 匹配参数，一个键值三个value
	EEData *find(const string& value1, const string& value2, const string& value3, string& vresult);
	
	//!
	friend ostream& operator<<(ostream& out, const ECParam& rhs)
	{
		EEData* pCur = rhs.m_head;
		while (pCur) {
			out << "CParamDAO: " ;
			out << "value1: "<< pCur->value1 ;
			out << "value2: "<< pCur->value2 ;
			out << "value3: "<< pCur->value3 ;
			out << "result: "<< pCur->vresult << endl;
			
			pCur = pCur->pNext;
		}		
	
		//!
		pCur = rhs.m_head_ext;
		while (pCur) {
			out << "CParamDAO: " ;
			out << "ext_value1: "<< pCur->value1 ;
			out << "ext_value2: "<< pCur->value2 ;
			out << "ext_value3: "<< pCur->value3 ;
			out << "ext_result: "<< pCur->vresult << endl;

			
			pCur = pCur->pNext;
		}		
		
		return out;
	}

};


typedef counted_ptr<ECParam> PECParam;
typedef map<string, PECParam> PECParams;


/**
 *  @class ECParamDAO	ECParamDAO.h
 *  \brief 参数DAO类，实现参数加载、缓存、匹配
 *
 *  参数DAO有两种用法：
 *  1. 应用初始化时调用参数DAO的Load()方法加载数据，之后getParam()方法即从缓存中取数据；
 *  2. 直接调用getParam()方法，如果从缓存中匹配不到数据，则从数据库中读取数据、缓存、返回匹配数据；
 *  当然也可以以上两种方式结合使用，即如果从已缓存数据中未匹配到结果，再试图从数据库中读取一次。
 *
 */
class ECParamDAO {
private:
	static PECParams m_params;
	
	static PECParam insECParam(const string& key);
	static PECParam insECParamExt(const string& key);

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


#endif  /* __CPARAMEXTDAO_H_ */
