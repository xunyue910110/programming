//////////////////////////////////////////////////////////////////////
// 
// 类名: ini_config
// 描述: 读取ini格式文件配置类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-17 14:50
// 版本: V1.0.0 
// 版权: 南京联创系统集成股份有限公司
//////////////////////////////////////////////////////////////////////
#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include "ini_config.hpp"
#include "stringutils.hpp"
#include "debug.hpp"
#include "file.hpp"

namespace archy {

int ini_config::set_filename(const string& strFileName) {
	m_filename = strFileName;
	return 0;
}

int ini_config::section_exists(const string& strSection)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	while(1) {
		ret = locate_section(m_file, section);
		if( ret < 1 ) break;
		if( section == strSection ) {
			return 1;
		}
	}
	return 0;
}


int ini_config::read_sections(list<string>& lstSections)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	lstSections.clear();
	while(1) {
		ret = locate_section(m_file, section);
		if( ret < 1 ) break;
		lstSections.push_back(section);
	}

	return lstSections.size();
}



int ini_config::read_section(const string& strSectionName,
	list< pair<string,string> >& lstKeyVal)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	lstKeyVal.clear();
	string key, val;
	while(1) {
		ret = locate_section(m_file, section);
		if( ret < 1 ) break;
		if( section == strSectionName ) {
			while(1) {
				ret = locate_key(m_file, key, val);
				if( ret < 1 ) break;
				lstKeyVal.push_back(make_pair(key, val));
			}
			break;
		}
	}
	return lstKeyVal.size();
}



int ini_config::read_section(const string& strSectionName,
	list< string >& lstKey)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	lstKey.clear();
	string key, val;
	while(1) {
		ret = locate_section(m_file, section);
		if( ret < 1 ) break;
		if( section == strSectionName ) {
			while(1) {
				ret = locate_key(m_file, key, val);
				if( ret < 1 ) break;
				lstKey.push_back(key);
			}
			break;
		}
	}
	return lstKey.size();
}

// -1 fail
// 0  can not find section
// 1 find and erase successfully

int ini_config::erase_section(const string& strSection)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	long lastPos; bool bFind(false);
	while(1) {
		ret = locate_section(m_file, section, &lastPos);
		if( ret < 1 ) break;
		if( section == strSection ) {
			if( ret < 1 ) break;
			bFind = true;
			break;
		}
	}
	if( bFind == false )
		return 0;
	
	archy::file ofile;
	string filename = m_filename + ".temp";
	ret = ofile.open(filename, "w");
	if( ret < 0 ) {
		return -1;
	}
	m_file.seek(0, file::seek_begin);
	string strline;
	while(m_file.tell() < lastPos ) {
		ret = m_file.read_line(strline);
		if( ret < 0 ) return -1;
		
		ret = ofile.write_line(strline);
		if( ret < 0 ) return -1;
	}
	ret = locate_section(m_file, section, &lastPos);
	ret = locate_section(m_file, section, &lastPos);
	if( ret > 0 ) {
		m_file.seek(lastPos, file::seek_begin);
		while(1 ) {
			ret = m_file.read_line(strline);
			if( ret < 0 ) return -1;
			if( ret == 0 ) break;
			
			ret = ofile.write_line(strline);
			if( ret < 0 ) return -1;
		}
	}
	m_file.close();
	ofile.close();
	
	::remove(m_filename.c_str());
	ret = ::rename(filename.c_str(), m_filename.c_str());
	if( ret != 0 )
		return -1;
	return 1;

}


// -1 fail
// 0  can not find section
// 1 find and erase successfully
int ini_config::clear_key(const string& strSection, const string& strKeyName)
{
	return ini_config::write_string(strSection, strKeyName, "");
}


// -1 fail
// 0  can not find section
// 1 find and get successfully

int ini_config::read_string(const string& strSectionName, const string& strKeyName, 
	string& strKeyValue)
{
	archy::file m_file;
	int ret;
	string section;
	
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	string key, val;
	while(1) {
		ret = locate_section(m_file, section);
		if( ret < 1 ) break;
		if( section == strSectionName ) {
			while(1) {
				ret = locate_key(m_file, key, val);
				if( ret < 1 ) break;
				if( key == strKeyName ) {
					strKeyValue = val;
					return 1;
				}
			}
			break;
		}
	}
	return 0;
}

string ini_config::ReadString(const string& strSec, const string& strKey, const string& strDef)
{
	string strVal;
	int iRet = read_string(strSec, strKey, strVal);
	if( iRet <= 0 )
		return strDef;
	return strVal;
}

int ini_config::read_integer(const string& strSectionName, 
	const string& strKeyName, 
	int& iKeyValue)
{
	string val;
	int ret = read_string(strSectionName, strKeyName, val);
	if( ret < 1 ) return ret;
	iKeyValue = atoi(val.c_str());
	return 1;
}

int ini_config::ReadInteger(const string& strSec, const string& strKey, int def)
{
	int iVal;
	int iRet = read_integer(strSec, strKey, iVal);
	if( iRet <= 0 )
		return def;
	return iVal;
}

int ini_config::write_string(const string& strSectionName, 
	const string& strKeyName, 
	const string& strKeyValue)
{
	int ret;
	string section;
        archy::file m_file;
	ret = m_file.open(m_filename, "r");
	if( ret < 0 ) return -1;
	
	string key, val;
	bool bFind(false);
	bool bFindSec(false);
	long pos1, pos2;
	while(1) {
		ret = locate_section(m_file, section, &pos1);
		if( ret < 1 ) break;
		if( section == strSectionName ) {
			bFindSec = true;
			while(1) {
				ret = locate_key(m_file, key, val, &pos2);
				if( ret < 1 ) break;
				if( key == strKeyName ) {
					bFind = true;
					break;
				}
			}
			break;
		}
	}
	
	
	archy::file ofile;
	string filename = m_filename + ".temp";
	ret = ofile.open(filename, "w");
	if( ret < 0 ) {
		return -1;
	}
	m_file.seek(0, file::seek_begin);
	bool bFirst = true;
	string strline;
	while(1) {
		if( bFirst ) {
                	if( bFind && m_file.tell() >= pos2 ) {
				bFirst = false;
				ret = m_file.read_line(strline);if( ret < 0 ) return -1;
				strline = strKeyName + " = " + strKeyValue;
				ret = ofile.write_line(strline);if( ret < 0 ) return -1;
                        }
                        else if ( !bFind && bFindSec && m_file.tell() >= pos1 ) {
				bFirst = false;
				ret = m_file.read_line(strline);if( ret < 0 ) return -1;
                                ret = ofile.write_line(strline);if( ret < 0 ) return -1;
				strline = strKeyName + " = " + strKeyValue;
				ret = ofile.write_line(strline);if( ret < 0 ) return -1;
                        }
		}
		ret = m_file.read_line(strline);
		if( ret < 0 ) return -1;
		if( ret == 0 ) break;
		
		ret = ofile.write_line(strline);
		if( ret < 0 ) return -1;
	}
	
	if ( bFindSec ==  false ) {
		strline = string("\n[") + strSectionName + string("]");
		ret = ofile.write_line(strline);if( ret < 0 ) return -1;
		strline = strKeyName + " = " + strKeyValue;
		ret = ofile.write_line(strline);if( ret < 0 ) return -1;	
	}
	m_file.close();
	ofile.close();
	
	::remove(m_filename.c_str());
	ret = ::rename(filename.c_str(), m_filename.c_str());
	if( ret != 0 )
		return -1;
	return 1;
}



int ini_config::write_integer(const string& strSectionName, 
	const string& strKeyName, 
	int iKeyValue)
{
	return write_string(strSectionName, strKeyName, to_string(iKeyValue));
}

int ini_config::locate_section(file& ofile, string& strSection, long* pos)
{
	string m_strline;
	int ret;
	while( 1 ) {
		if( pos ) *pos = ofile.tell();
		ret = ofile.read_line(m_strline);
		if( ret < 1 ) break;
		
		trim(m_strline);
		if( m_strline.empty())continue;

		// ignore the line that is not section line
		if( m_strline[0] != '[' ) continue;

		string::size_type pos = m_strline.find(']', 1);
		if( pos == string::npos ) {
			ARCHY_CERR << "There is a line format error, no banlance in [ and ] " << endl;
			continue;
		}
		string sec = m_strline.substr(1, pos - 1);
		trim(sec);
		if( sec.empty() ) {
			ARCHY_CERR << "There is an empty section in [] in config file" << endl;
			continue;
		}
		strSection = sec;
		return 1;	
	}
	return 0;
}

// 定位key的位置
int ini_config::locate_key(file& ofile, string& strKey, string& strVal, long* pos)
{
	string m_strline;
	int ret;
	while( 1 ) {
		if( pos ) *pos = ofile.tell();
		ret = ofile.read_line(m_strline);
		if( ret < 1 ) break;

		trim(m_strline);
		if( m_strline.empty())continue;
		
		if( m_strline[0] == '[' )
			return 0;
		
		if( m_strline[0] == '#' )
			continue;
		
		string::size_type pos = m_strline.find('=', 1);
		string key;
		if( pos == string::npos ) {
			strKey = m_strline;
			strVal = "";
			return 1;
		}
		else {
			key = m_strline.substr(0, pos);
			trim(key);
			if( key.empty() ) {
				ARCHY_CERR << "There is a line which has no key " << endl;
			}
			else {
				strKey = key;
				key = m_strline.substr(pos + 1);
				strVal = trim(key);
				return 1;
			}		
		}		
	}
	return 0;	
}


} // namespace archy



