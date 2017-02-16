#include "mwci/CFmlBuf.h"
#include "udbi/udbi.h"
#include "ECParamDAO.h"

using namespace UDBI;

/**
 *  析构函数
 */
ECParam::~ECParam()
{
	///cout << "CParamDAO:IN ECParam::~ECParam ... " << endl;
	
	EEData* pCur;
	
	while (m_head) {
		pCur = m_head->pNext;
		delete m_head;
		m_head = pCur;
	}
	
	while (m_head_ext) {
		pCur = m_head_ext->pNext;
		delete m_head_ext;
		m_head_ext = pCur;
	}
}

/**
 *  删除基参数
 */
void ECParam::erase()
{
	///cout << "CParamDAO:IN ECParam::erase ... " << endl;
	
	EEData* pCur;
	
	while (m_head) {
		pCur = m_head->pNext;
		delete m_head;
		m_head = pCur;
	}
}

/**
 *  参数添加
 */
void ECParam::add(const string& val1, const string& val2, const string& val3, const string& vres)
{
	EEData* pCur = new EEData;
	pCur->pNext = m_head;
	pCur->value1.assign(val1);
	pCur->value2.assign(val2);
	pCur->value3.assign(val3);
	pCur->vresult.assign(vres);
	
	m_head = pCur; 
}

/**
 *  扩展参数添加
 */
void ECParam::addext(const string& val1, const string& val2, const string& val3, const string& vres)
{
	EEData* pCur = new EEData;
	pCur->pNext = m_head_ext;
	pCur->value1.assign(val1);
	pCur->value2.assign(val2);
	pCur->value3.assign(val3);
	pCur->vresult.assign(vres);
	
	m_head_ext = pCur; 
}

/**
 * 参数查询
 */
EEData *ECParam::find(const string& value1, string& vresult)
{
	///cout << "CParamDAO:IN ECParam::find1 ... " << endl;
	
	EEData* pCur = m_head;
	while (pCur) {
		if (pCur->value1==value1) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		

	//!
	pCur = m_head_ext;
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
 * 参数查询
 */
EEData* ECParam::find(const string& value1, const string& value2, string& vresult)
{
	///cout << "CParamDAO:IN ECParam::find2 ... " << endl;
	
	EEData* pCur = m_head;
	
	while (pCur) {
		if ((pCur->value1==value1)&&(pCur->value2==value2)) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		

	//!
	pCur = m_head_ext;
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
 * 参数查询
 */
EEData* ECParam::find(const string& value1, const string& value2, const string& value3, string& vresult)
{
	///cout << "CParamDAO:IN ECParam::find3 ... " << endl;
	
	EEData* pCur = m_head;
	
	while (pCur) {
		if ( (pCur->value1==value1)&&(pCur->value2==value2)&&(pCur->value3==value3) ) {
			vresult = pCur->vresult;
			return pCur;
		}
		
		pCur = pCur->pNext;
	}		

	//!
	pCur = m_head_ext;
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
 * 静态数据初始化
 */
PECParams ECParamDAO::m_params;

/**
 *  一般在应用初始化时调用此函数一次性缓存所有参数数据，本方法根据
 *  CPARAM DAO（数据库中有配置）的绑定表TD_S_CPARAM与参数数据加载SQL
 *  语句标识INIT_LOAD从数据库中查询到参数加载的SQL语句，然后执行该SQL
 *  获取参数数据。数据以键值为key缓存到静态map中，每个键值对应的数据
 *  以链表结构存放
 */
void ECParamDAO::Load()
{
	typedef map<string, PECParam>::value_type valType;
	PECParam param;
	
	JConnect* pcon=jdb::getConnect();
	JTable* ptab = pcon->CreateJTable("TD_S_CPARAM");
	try
	{
		ptab->ExecSelect("INIT_LOAD");
	
		string s1, s2, s3, s4, s0;		
		while (ptab->Fetch()) {
			s1 = ptab->GetString("KEY");
			s2 = ptab->GetString("VALUE1");
			s3 = ptab->GetString("VALUE2");
			s4 = ptab->GetString("VRESULT");
			
			if (s1 != s0) {
				param = PECParam(new ECParam());
				m_params.insert( valType(s1, param) );
				s0 = s1;
			}		
		
			param->add(s2, s3, "", s4);
		}
		delete ptab;
	}
	catch ( Exception& er)
	{
		delete ptab;
	}	
}

/**
 *  清空所有参数数据
 */
void ECParamDAO::UnLoad()
{
	m_params.clear();
}

/** ◆◇
 *  插入key对应的参数数据，一或二个value；本函数根据键值和TD_S_PARAM
 *  到数据库中查询到对应的参数数据刷新SQL语句并执行之，刷新到的数据附
 *  加到静态数据结构中
 *  
 *  如键值为'CUST.STATE'，则如果该键值数据需要在缓存中匹配不到时从数据
 *  库刷新，则对应TD_S_CPARAM与CUST.STATE在CPARAM DAO的配置定义中一定
 *  有一个SQL定义，该SQL定义绑定的SQL语句就是为了刷新这个键值的参数数据的
 */
PECParam ECParamDAO::insECParam(const string& key)
{
	///cout << "CParamDAO:IN ECParam::insECParam ... " << endl;
	
	typedef map<string, PECParam>::value_type valType;
	
	PECParam param;
	PECParams::iterator iter = m_params.find(key);
	if ( iter != m_params.end() )
	{
		param = (*iter).second;
		param->erase();
	}
	else
	{
		param = PECParam(new ECParam());
		m_params.insert( valType(key, param) );
	}
		
	JConnect* pcon=jdb::getConnect();
	JTable* ptab=pcon->CreateJTable("TD_S_CPARAM");
	try
	{
		string s1, s2, s3;
		
		ptab->SetParam(":VKEY", key);
		ptab->ExecSelect(key);
		while (ptab->Fetch()) {
			s1=ptab->GetString("VALUE1");
			s2=ptab->GetString("VALUE2");
			s3=ptab->GetString("VRESULT");
			
			param->add(s1, s2, "", s3);
		}

		delete ptab;
		return param;

		//m_params.insert( valType(key, param) );
	} 
	catch ( Exception& ex) 
	{
		delete ptab;
		return PECParam(0);
	}
}

/**
 *  同insParam()方法，本方法是刷新一个键值三个value类型的参数数据的
 *  而且该类型数据不在参数初始化加载函数中提供加载功能，只能通过本
 *  参数刷新方法获取参数并缓存
 */
PECParam ECParamDAO::insECParamExt(const string& key)
{
	///cout << "CParamDAO:IN ECParam::insECParamExt ... " << endl;
	
	typedef map<string, PECParam>::value_type valType;
		
	PECParam param;
	PECParams::iterator iter = m_params.find(key);
	if ( iter != m_params.end() )
	{
		param = (*iter).second;
		param->erase();
	}
	else
	{
		param = PECParam(new ECParam());
		m_params.insert( valType(key, param) );
	}

	JConnect* pcon=jdb::getConnect();
	JTable* ptab=pcon->CreateJTable("TD_S_CPARAM");
	try
	{
		string s1, s2, s3, s4;
		
		ptab->SetParam(":VKEY", key);
		ptab->ExecSelect(key);
		while (ptab->Fetch()) {
			s1=ptab->GetString("VALUE1");
			s2=ptab->GetString("VALUE2");
			s3=ptab->GetString("VALUE3");
			s4=ptab->GetString("VRESULT");
			
			param->add(s1, s2, s3, s4);
		}
		//m_params.insert( valType(key, param) );
		
		delete ptab;
		return param;
	} 
	catch ( Exception& ex) 
	{
		delete ptab;
		return PECParam(0);
	}	
}

/**
 * 获取参数值，一个键值与一个value
 *
 */
string ECParamDAO::getParam(const string& key, const string& value)
{
	///cout << "CParamDAO:IN ECParamDAO::getParam1 ... " << endl;
	
	string tmpStr;

	PECParams::iterator iter = m_params.find(key);
	if ( iter != m_params.end() ) {
		PECParam& param = (*iter).second;
		
		EEData* pdat = param->find(value, tmpStr);
		if ( (!pdat) && (!value.empty()) ) 
			param->erase();		/*!< 是清除库参数数据 */
		else 
			return tmpStr;
	} 
	
	PECParam param(ECParamDAO::insECParam(key));
	if (param) {
		EEData* pdat = param->find(value, tmpStr);
		
		//! 当初次加载或重载后仍没有该值，则增加一空值
		if ( (!pdat) && (!value.empty()) ) {
			param->addext(value, "", "", "");
			printf("ECParamDAO: key: %s, val: %s.\n", key.c_str(), value.c_str());
		}      
	}
	
	return tmpStr;
}

/**
 * 获取参数值，一个键值与二个value
 *
 */
string ECParamDAO::getParam(const string& key, const string& value1, const string& value2)
{
	///cout << "CParamDAO:IN ECParamDAO::getParam2 ... " << endl;
	
	string tmpStr;
	
	PECParams::iterator iter = m_params.find(key);
	if (iter != m_params.end() ) {
		PECParam& param = (*iter).second;
		EEData* pdat = param->find(value1, value2, tmpStr);
		if ( (!pdat) && (!value1.empty()) && (!value2.empty()) )	
			param->erase();
		else
			return tmpStr;
	} 
	
	PECParam param(ECParamDAO::insECParam(key));
	if (param) {
		EEData* pdat = param->find(value1, value2, tmpStr);
		
		//! 当初次加载或重载后仍没有该值，则增加一空值
		if ( (!pdat) && (!value1.empty()) && (!value2.empty()) ) {
			param->addext(value1, value2, "", "");
			printf("ECParamDAO: key: %s, val1: %s, val2: %s.\n", key.c_str(), value1.c_str(), value2.c_str());
		}      		
	}
	
	return tmpStr;
}

/**
 * 获取参数值，一个键值与三个value
 *
 */
string ECParamDAO::getParam(const string& key, const string& value1, const string& value2, const string& value3)
{
	///cout << "CParamDAO:IN ECParamDAO::getParam3 ... " << endl;
	
	string tmpStr;
	
	PECParams::iterator iter = m_params.find(key);
	if (iter != m_params.end() ) {
		PECParam& param = (*iter).second;
		EEData* pdat = param->find(value1, value2, value3, tmpStr);
		if ( (!pdat) && (!value1.empty()) && (!value2.empty()) && (!value3.empty()) )
			param->erase();
		else
			return tmpStr;
	} 
	
	PECParam param(ECParamDAO::insECParamExt(key));
	if (param) {
		EEData* pdat = param->find(value1, value2, value3, tmpStr);

		//! 当初次加载或重载后仍没有该值，则增加一空值
		if ( (!pdat) && (!value1.empty()) && (!value2.empty()) && (!value3.empty()) ) {
			param->addext(value1, value2, value3, "");
			printf("ECParamDAO: key: %s, val1: %s, val2: %s, val3: %s.\n", key.c_str(), value1.c_str(), value2.c_str(), value3.c_str());
		}      		
	}
	
	return tmpStr;
}

