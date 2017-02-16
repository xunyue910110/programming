#include "mwci/CFmlBuf.h"
#include "udbi/udbi.h"
#include "CParamDAO.h"
//#include "ECParamDAO.h"

#include "IIpcParam.h"		// share-memory mode support

using namespace UDBI;


////////////////////////////////////////////////
// ◆以下代码[无用] 

/**
 *  [无用] 析构函数
 */
CParam::~CParam()
{
	EData* pCur;
	
	while (m_head) {
		pCur = m_head->pNext;
		delete m_head;
		m_head = pCur;
	}
}

/**
 *  [无用] 参数添加
 */
void CParam::add(const string& val1, const string& val2, const string& vres)
{
	EData* pCur = new EData;
	pCur->pNext = m_head;
	pCur->value1.assign(val1);
	pCur->value2.assign(val2);
	pCur->vresult.assign(vres);
	
	m_head = pCur; 
}

/**
 *  [无用] 参数添加
 */
void CParam::add(const string& val1, const string& val2, const string& val3, const string& vres)
{
	EData* pCur = new EData;
	pCur->pNext = m_head;
	pCur->value1.assign(val1);
	pCur->value2.assign(val2);
	pCur->value3.assign(val3);
	pCur->vresult.assign(vres);
	
	m_head = pCur; 
}

/**
 * [无用] 参数查询
 */
EData *CParam::find(const string& value1, string& vresult)
{
	EData* pCur = m_head;
	
	while (pCur) {
		if (pCur->value1==value1) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		
	return 0;
}

/**
 * [无用] 参数查询
 */
EData* CParam::find(const string& value1, const string& value2, string& vresult)
{
	EData* pCur = m_head;
	
	while (pCur) {
		if ((pCur->value1==value1)&&(pCur->value2==value2)) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		
	return 0;
}

/**
 * [无用] 参数查询
 */
EData* CParam::find(const string& value1, const string& value2, const string& value3, string& vresult)
{
	EData* pCur = m_head;
	
	while (pCur) {
		if ( (pCur->value1==value1)&&(pCur->value2==value2)&&(pCur->value3==value3) ) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		
	return 0;
}	

/**
 * [无用] 静态数据初始化
 */
PCParams CParamDAO::m_params;

/**
 *  一般在应用初始化时调用此函数一次性缓存所有参数数据，本方法根据
 *  CPARAM DAO（数据库中有配置）的绑定表TD_S_CPARAM与参数数据加载SQL
 *  语句标识INIT_LOAD从数据库中查询到参数加载的SQL语句，然后执行该SQL
 *  获取参数数据。数据以键值为key缓存到静态map中，每个键值对应的数据
 *  以链表结构存放
 */
void CParamDAO::Load()
{
	//ECParamDAO::Load();
}

/**
 *  清空所有参数数据
 */
void CParamDAO::UnLoad()
{
	//ECParamDAO::UnLoad();
}

/**
 *  [无用] 
 *  插入key对应的参数数据，一或二个value；本函数根据键值和TD_S_PARAM
 *  到数据库中查询到对应的参数数据刷新SQL语句并执行之，刷新到的数据附
 *  加到静态数据结构中
 *  
 *  如键值为'CUST.STATE'，则如果该键值数据需要在缓存中匹配不到时从数据
 *  库刷新，则对应TD_S_CPARAM与CUST.STATE在CPARAM DAO的配置定义中一定
 *  有一个SQL定义，该SQL定义绑定的SQL语句就是为了刷新这个键值的参数数据的
 */
PCParam CParamDAO::insCParam(const string& key)
{
	return PCParam(0);
}

/**
 *  [无用] 
 *  同insParam()方法，本方法是刷新一个键值三个value类型的参数数据的
 *  而且该类型数据不在参数初始化加载函数中提供加载功能，只能通过本
 *  参数刷新方法获取参数并缓存
 */
PCParam CParamDAO::insCParamExt(const string& key)
{
	return PCParam(0);
}

// ◆以上代码[无用] 
////////////////////////////////////////////////


/**
 * 获取参数值，一个键值与一个value
 *
 */
string CParamDAO::getParam(const string& key, const string& value)
{
	IIpcParam& _ipp = IIpcParam::getIpcParam();
	return _ipp.getTrans(key, value);
}

/**
 * 获取参数值，一个键值与二个value
 *
 */
string CParamDAO::getParam(const string& key, const string& value1, const string& value2)
{
	IIpcParam& _ipp = IIpcParam::getIpcParam();
	return 	_ipp.getTrans(key, value1, value2);
}

/**
 * 获取参数值，一个键值与三个value
 *
 */
string CParamDAO::getParam(const string& key, const string& value1, const string& value2, const string& value3)
{
	IIpcParam& _ipp = IIpcParam::getIpcParam();
	return _ipp.getTrans(key, value1, value2, value3);
}


/** ConvParam
 *  code-to-name 转换
 */
int ConvParam::CodeToName(CFmlBuf& buf, const char* fromFldName, const char* toFldName)
{
    if (m_cnt < 0) { return m_cnt; }

    int i=0;
    int fldoc;
    string tmpCode, tmpName;

	IIpcParam& _ipp = IIpcParam::getIpcParam();
    
    fldoc = buf.GetCount(fromFldName);

	//	_ipp.refresh(key);
	int lckid = _ipp.beginLock2(key);
	if (lckid < 0) {
		//Exception
		return -1;
	}
	
	try
	{
		if (fldoc < 100) {    
		    if (m_cnt == 0) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(tmpCode);
		            buf.SetString(toFldName, tmpName, i);
		        }
		    } else if (m_cnt == 1) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(value1, tmpCode);
		            buf.SetString(toFldName, tmpName, i);
		        }
		    } else if (m_cnt == 2) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(value1, value2, tmpCode);
		            buf.SetString(toFldName, tmpName, i);
		        }
		    }
		} else {
			CFmlBuf abuf1;
		    if (m_cnt == 0) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(tmpCode);
		            abuf1.SetString(toFldName, tmpName, i);
		        }
		    } else if (m_cnt == 1) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(value1, tmpCode);
		            abuf1.SetString(toFldName, tmpName, i);
		        }
		    } else if (m_cnt == 2) {
		        for (i=0; i<fldoc; i++) {
		            tmpCode = buf.GetString(fromFldName, i);
		            tmpName = _ipp.getTrans2(value1, value2, tmpCode);
		            abuf1.SetString(toFldName, tmpName, i);
		        }
		    }
			buf.Append(abuf1);		
		}
	} catch ( Exception& er) {
		_ipp.endLock2(lckid);
		throw;
	}
	
	_ipp.endLock2(lckid);
	
    return i;
}


/**
 *  参数编码-名称转换函数
 *  1. 根据输入的fromFldName 从buf 中找到编码值;
 *  2. 以key 和编码值为条件从参数缓存中查询名称;
 *  3. 在把名称以toFldName 放入buf;
 *
 *	ConvertCodeToName(outBuf, "PRODUCT.PRODUCT_TYPE", "PRODUCT_TYPE", "PRODUCT_TYPE_NAME");
 *  outBuf.CodeToName("PRODUCT.PRODUCT_TYPE", "PRODUCT_TYPE", "PRODUCT_TYPE_NAME");
 */
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName, const char* toFldName)
{
	///cout << "CParamDAO:IN ConvertCodeToName ... " << endl;
	
	int fldoc;
	string tmpCode, tmpName;
	
	// share-memory mode support
	IIpcParam& _ipp = IIpcParam::getIpcParam();
		
	fldoc = buf.GetCount(fromFldName);
	
	//	_ipp.refresh(skey);
	int lckid = _ipp.beginLock2(skey);
	if (lckid < 0) {
		//Exception
		return -1;
	}
		
	int i=0;
	
	try
	{
		if (fldoc < 100) {
			for (i=0; i<fldoc; i++) {
				tmpCode = buf.GetString(fromFldName, i);
				tmpName = _ipp.getTrans2(tmpCode);
				buf.SetString(toFldName, tmpName, i);
			}
		} else {
			CFmlBuf abuf1;
			for (i=0; i<fldoc; i++) {
				tmpCode = buf.GetString(fromFldName, i);
				tmpName = _ipp.getTrans2(tmpCode);
				abuf1.SetString(toFldName, tmpName, i);
			}
			buf.Append(abuf1);
		}
	} catch ( Exception& er) {
		_ipp.endLock2(lckid);
		throw;
	}
	
	_ipp.endLock2(lckid);
	
	return i;
}

//Add by Gemj Begin@2005-4-6 17:16
/**
 *  参数编码-名称转换函数
 *  1. 根据输入的fromFldName1 和 fromFldName2 从buf 中找到编码值;
 *  2. 以key 和编码值为条件从参数缓存中查询名称;
 *  3. 在把名称以toFldName 放入buf;
 *
 *	ConvertCodeToName(outBuf, "ItemName", "ITEM_CODE", "ITEM_TYPE", "ITEM_NAME");
 */
int ConvertCodeToName(CFmlBuf& buf, const string& skey, const char* fromFldName1, const char* fromFldName2, const char* toFldName)
{
	int fldoc;
	string tmpCode1, tmpCode2, tmpName;

	// share-memory mode support
	IIpcParam& _ipp = IIpcParam::getIpcParam();
	
	fldoc = buf.GetCount(fromFldName1);

	int i=0;

	//	_ipp.refresh(skey);
	int lckid = _ipp.beginLock2(skey);
	if (lckid < 0) {
		//Exception
		return -1;
	}
	
	try
	{
		if (fldoc < 100) {
			for (i=0; i<fldoc; i++) {
				tmpCode1 = buf.GetString(fromFldName1, i);
				tmpCode2 = buf.GetString(fromFldName2, i);
				tmpName = _ipp.getTrans2(tmpCode1, tmpCode2);
				buf.SetString(toFldName, tmpName, i);
			}
		} else {
			CFmlBuf abuf1;
			for (i=0; i<fldoc; i++) {
				tmpCode1 = buf.GetString(fromFldName1, i);
				tmpCode2 = buf.GetString(fromFldName2, i);
				tmpName = _ipp.getTrans2(tmpCode1, tmpCode2);
				abuf1.SetString(toFldName, tmpName, i);
			}
			buf.Append(abuf1);			
		}
	} catch ( Exception& er) {
		_ipp.endLock2(lckid);
		throw;
	}

	_ipp.endLock2(lckid);
	
	return i;
}
//Add by Gemj End@2005-4-6 17:23


/**
 *  \mainpage 静态参数DAO组件
 *  
 *  \section sec_intro 综述
 *  静态参数组件封装了两个类，一个参数类、一个参数DAO类，协助实现了静态参数的加载、缓存、刷新和
 *  匹配功能。静态参数组件一般有两种使用方式，一种方式是初始化加载并提供动态刷新，即在参数匹配
 *  时正常直接从缓存中取数据，如果没有匹配到再从数据库中刷新一次。第二种方式是不加载，每次匹配
 *  参数时第一次完成参数数据刷新并缓存，之后从缓存中获取。
 * 
 *	\section sec_config 配置
 *  静态参数DAO组件涉及两类配置，一是初始化参数加载SQL语句定义，二是参数刷新SQL语句定义，以上
 *  两种定义都可以通过代码生成工具完成。
 *  现约定配置的DAO为cparam，DAO绑定表为TD_S_CPARAM，并缺省定义初始化参数加载SQL语句（直接从
 *  TD_S_CPARAM表中加载数据）。
 *  使用人员需要视自己的需求配置相应KEY（键值）的参数刷新SQL语句，目前有两种格式的参数刷新SQL
 *  语句可以配置，一种是取一个value或二个value，格式相同，为：
 *
 *  \code
 *  SELECT fld1 key, fld2 value1, fld3 value2, fld4 vresult FROM xx_table WHERE 'XX_KEY' = :vkey
 *  \endcode
 *  其中个字段可以根据自己的需要引用，但必须用指定的几个别名，同时XX_KEY一定要等于fld1的值，例如	
 *  可以有如下几种写法：
 *  
 *  \code
 *  SELECT 'CUST.STATE' key, col_name value1, col_val value2, col_desc vresult 
 *    FROM cust_state
 *   WHERE 'CUST.STATE' = :vkey
 *
 *  SELECT key, value1, value2, vresult 
 *    FROM td_s_cparam
 *   WHERE key = :vkey
 *  \endcode
 *
 *  第二种格式与上述相似，只是多了一个字段，用于一个键值三个value的参数数据刷新，为：
 *  \code
 *  SELECT fld1 key, fld2 value1, fld3 value2, fld4 value3, fld5 vresult FROM xx_table WHERE 'XX_KEY'=:vkey
 *  \endcode
 *
 *
 *  同时该组件中还定义了一个转换函数，用于从指定的buf中取出code值、从缓存中匹配name、再把name写回到buf，
 *  详细实现参见该函数的说明
 *  
 *  \section sec_apply  引用
 *  静态参数DAO组件以动态库形式发布，动态库名为libcparamDAO.so，头文件为CParamDAO.h
 *  \arg 使用时直接引用头文件即可
 *  
 * 
 */  
