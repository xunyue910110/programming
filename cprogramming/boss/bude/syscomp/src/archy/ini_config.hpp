//////////////////////////////////////////////////////////////////////
// 
// 类名: ini_config
// 描述: 读取ini格式文件配置类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-17 14:50
// 版本: V1.0.0 
// 版权: 南京联创系统集成股份有限公司
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// 配置文件格式约定:
//      1、注释必须使用#开始的行注释，这是注释的仅有形式
//      2、配置文件中段名不能重复
//      3、同一段中的键名不能重复
//      4、可以只存在键名，而不写键值
// 配置文件实例:
// [section1]
// key1 = val1
// key2= val2
// key3 = val3
// [section2]
// key1
// key2
//////////////////////////////////////////////////////////////////////
#if !defined(ARCHY_QFileConfig_H__62258012_83F2_47E9_ACFE_44703F1C4B9B__INCLUDED_)
#define ARCHY_QFileConfig_H__62258012_83F2_47E9_ACFE_44703F1C4B9B__INCLUDED_
#ifdef _MBCS
#pragma warning( disable : 4786)
#endif


#include <list>
#include <utility>
#include "archy_config.hpp"

USING_STD_NAMESPACE;

namespace archy {

class file;

class ini_config
{
public:

	// return 0
	int set_filename(const string& strFileName);
	
	// 1 find
	// 0 not find
	// -1 fail
	
	int section_exists(const string& strSection);

	// -1 fail
	// other lstSections.size()
	int read_sections(list<string>& lstSections);
	
	// -1 fail
	// other lstKeyVal.size()
	int read_section(const string& strSectionName,
		list< pair<string,string> >& lstKeyVal);

	// -1 fail
	// other lstKey.size()
	
	int read_section(const string& strSectionName,
		list< string >& lstKey);

	// -1 fail
	// 0  can not find section
	// 1 find and erase successfully
	int erase_section(const string& strSection);	
	int clear_key(const string& strSection, const string& strKeyName);
	
	int read_string(const string& strSectionName, const string& strKeyName, 
		string& strKeyValue);

/*
		Call ReadString to read a string value from an INI file. Section identifies the section in the file that contains the desired key. Ident is the name of the key from which to retrieve the value. Default is the string value to return if the:
	
	Section does not exist.
		Key does not exist.
		Data value for the key is not assigned.
*/
	
	string ReadString(const string& strSec, const string& strKey, const string& strDef = "");
	int read_integer(const string& strSectionName, 
		const string& strKeyName, 
		int& iKeyValue);

	int ReadInteger(const string& strSec, const string& strKey, int def = 0);

	int write_string(const string& strSectionName, 
		const string& strKeyName, 
		const string& strKeyValue);

	int write_integer(const string& strSectionName, 
		const string& strKeyName, 
		int iKeyValue);

private:
	// 定位key的位置
	int locate_key(file& ofile, string& strKey, string& strVal, long* pos = 0);

	// 定位section的位置
	int locate_section(file& ofile, string& strSection, long* pos = 0);

private:
	string m_filename;
};

} // namespace archy

#endif // !defined(ARCHY_QFileConfig_H__62258012_83F2_47E9_ACFE_44703F1C4B9B__INCLUDED_)
