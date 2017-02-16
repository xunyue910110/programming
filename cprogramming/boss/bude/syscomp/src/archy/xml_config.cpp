//////////////////////////////////////////////////////////////////////
// 
// 类名: xml_config
// 描述: 读取形如xml格式的配置文件
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-18 7:55
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#include "xml_config.hpp"
#include "stringutils.hpp"
#include "file.hpp"
#include <ctype.h>
#include "debug.hpp"
#include <stdarg.h>
#include "ustr.hpp"


namespace archy {

xml_config::xml_config()
:m_father(0)
{
}

xml_config::~xml_config()
{
	xml_config::clear();
}

xml_config::xml_config(const xml_config& xml)
:m_father(0)
{
	*this = xml;
}

xml_config::xml_config(const string& name)
:m_father(0)
,m_secName(name)
{
}

xml_config::xml_config(const string& name, const string& val)
:m_father(0)
,m_secName(name)
,m_strval(val)
{
}


xml_config& xml_config::operator = (const xml_config& xml)
{
	if( this != &xml) {
		this->clear();
		m_secName = xml.m_secName;
		m_lstPair = xml.m_lstPair;
		m_strval = xml.m_strval;
		
		list<xml_config*>::const_iterator it;
		for( it = xml.m_lstSection.begin(); it != xml.m_lstSection.end(); ++it) {
			xml_config* pXml = new xml_config(*(*it));
			pXml->m_father = this;
			m_lstSection.push_back(pXml);
		}
	}
	return *this;
}

bool xml_config::exist_property(const string& strKey)const
{
	list<pair<string, string> >::const_iterator it;
	for( it = m_lstPair.begin(); it != m_lstPair.end(); ++it ) {
//        	cout << "key:" << it->first << endl;
		if(it->first == strKey ) return true;
    }
	return false;
}


string xml_config::get_property(const string& strKey)const throw(runtime_error)
{
//	cout << "Tag:" << m_secName << endl;
	list<pair<string, string> >::const_iterator it;
	for( it = m_lstPair.begin(); it != m_lstPair.end(); ++it ) {
//        	cout << "key:" << it->first << endl;
		if(it->first == strKey ) return it->second;
        }
	
	throw runtime_error(string("Not Found Corresponding Key:") + strKey );
}



xml_config* xml_config::cd(const string& name)
{
	xml_config* retXml = 0;
	if( name.empty() ) return retXml;
	
	retXml = this;
	if( name[0] == '/') { // 从根开始遍历
		while(1) {
			if( this->m_father == 0)
				break;
			retXml = this->m_father;
		}
	}
    list<string> lst;
    
    UStr::Split(name, "/", lst);
	
	list<string>::iterator it0 = lst.begin();
	for( ; retXml!= 0 && it0 != lst.end(); ++it0 ) {
		if( *it0 == "." )
			continue;
		if( *it0 == ".." ) {
			retXml = retXml->m_father;
			continue;
		}
		list<xml_config*>::iterator it = retXml->m_lstSection.begin();
		for( ; it != retXml->m_lstSection.end(); ++it ) {
			if( (*it)->get_name() == *it0 ) {
				retXml = (*it);
				break;
			}
		}
		if( it == retXml->m_lstSection.end())
			retXml = 0;
	}
	return retXml;

}

list<xml_config*> xml_config::ls(const string& name) 
{
	list<xml_config*> ret;

	list<xml_config*>::iterator it = m_lstSection.begin();
	for( ; it != m_lstSection.end(); ++it ) {
		if( (*it)->get_name() == name ) 
			ret.push_back(*it);
	}
	return ret;
}


void xml_config::clear()
{
	list<xml_config*>::iterator it;
	for( it = m_lstSection.begin(); it != m_lstSection.end(); ++it) {
		delete *it;
	}
	m_lstSection.clear();
	m_lstPair.clear();
}

bool xml_config::parse_single(const string& strBetween, xml_config* xml, int iLine)
{
	string::size_type pos = strBetween.find_first_of(" \t");
	if( pos == string::npos ) {
		xml->m_secName = strBetween;
		return true;
	}
        else {
        	string secName = strBetween.substr(0, pos);
                trim(secName);
                if( secName.empty() ) {
                	ARCHY_CERR << "Lack of flag at line:" << iLine << endl;
                	return false;
                }
                xml->m_secName = secName;
        }
	string strKey, strVal;
	string strTemp = strBetween;
	string::size_type pos1;
        strTemp.erase(0, pos + 1);
	while(1) {
		pos1 = strTemp.find('=' );
		if( pos1 == string::npos ) {
			ARCHY_CERR << "lack of = at the line:" << iLine << endl;
			return false;
		}
		strKey = strTemp.substr(0, pos1);
		trim(strKey);
		if( strKey.empty() ) {
			ARCHY_CERR << "lack of key before = at the line:" << iLine << endl;
			return false;
		}
		strTemp.erase(0, pos1 + 1);
		trim(strTemp);
		if( strTemp.empty() ) {
			ARCHY_CERR << "lack of val after = at the line:" << iLine << endl;
			return false;
		}
		if( strTemp[0] != '"' ) {
			ARCHY_CERR << "Val hasnot a left quotation mark(\") at the line:" << iLine << endl;
			return false;
		}
		strTemp.erase(0, 1);
		pos = strTemp.find('"');
		if( pos == string::npos ) {
			ARCHY_CERR << "val hasnot a right quotation mark(\") at the line:" << iLine << endl;
			return false;
		}
		
		strVal = strTemp.substr(0, pos);
		trim(strVal);

		// 查看属性是否已经重复
                list<pair<string, string> >::iterator it;
                for( it = xml->m_lstPair.begin(); it != xml->m_lstPair.end(); ++it ) {
                	if( it->first == strKey ) {
				ARCHY_CERR << "found a duplicated property at the line:" << iLine << endl;
				return false;
                        }
                }
		xml->m_lstPair.push_back(make_pair(strKey, strVal));
		strTemp.erase(0, pos + 1);
		trim(strTemp);

		if( strTemp.empty() )
			break;
	}	
	return true;
}



bool xml_config::parse_internal(string& strline, xml_config* xml, file& ofile, int& iLine)
{
	string strNewLine;
	
	string::size_type pos1;
        int ret;
	while(1) {
		// 如果需要读取下一行或者当前处理行为空的话，则读取一行
		if( m_bNeedNextLine || strline.empty() ) {
			ret = ofile.read_line(strNewLine);
			if( ret < 0 ) return false;
			else if( ret == 0 ) break;
			++iLine;
			trim(strNewLine);
			if( strNewLine.empty()) continue;
			
			strline += strNewLine;
		}
		m_bNeedNextLine = false;
		
		//  处理结束标记
		if( strncmp(strline.c_str(), "</", 2 ) == 0 ) {
			pos1 = strline.find('>', 2 );
			if( pos1 == string::npos ) {
				ARCHY_CERR << "Lack a > match </ at the same line: " << iLine << endl;
				return false;
			}
			string endtag = strline.substr(2, pos1 - 2);
			trim_right(endtag);
			if( endtag == xml->m_secName ) {
				strline.erase(0, pos1 + 1);
				return true;
			}
			if(xml->m_secName.empty() ) {
				ARCHY_CERR << "No HeadFlag matches this Endflag " << xml->m_secName << " at line:" << iLine << endl;				
			}
			else {
				ARCHY_CERR << "Endflag not match " << xml->m_secName << " at line:" << iLine << endl;
			}
			return false;	
		}
		
		// 处理注释部分
		if( strncmp(strline.c_str(), "<!--", 3 ) == 0 ) {
			pos1 = strline.find("-->", 4 );
			if( pos1 == string::npos ) {
				if( strline.length() > 512 ) {
					ARCHY_CERR << "Cannot find endflag --> within 512 Bytes. " 
						<< "The remark block's maxsize is 512 Byte!"
						<< "Please check the file" << endl;
					return false;
				}
				m_bNeedNextLine = true;
				continue;
			}
			// ignore the comment part
			strline.erase(0, pos1 + 3);
			trim_left(strline);
			continue;	
		}
		// 处理标题<?xml version="1.0" encoding="GB2312"?>
                if( strncmp(strline.c_str(), "<?xml", 5 ) == 0 ) {
			pos1 = strline.find("?>", 5 );
			if( pos1 == string::npos ) {
				ARCHY_CERR << "Cannot find endflag --> within 512 Bytes. "
					<< "The remark block's maxsize is 512 Byte!"
					<< "Please check the file" << endl;
                                return false;
                        }
			strline.erase(0, pos1 + 2);
			trim_left(strline);
			continue;
                }
		// 处理正常开始的标记
		if( strline[0] == '<' ) {
			pos1 = strline.find('>', 1 );
			if( pos1 == string::npos ) {
				ARCHY_CERR << "Can not find > at the same line:" << iLine << endl;
				return false;
			}
			string strBetween  = strline.substr(1, pos1 - 1);
			strline.erase(0, pos1 + 1 );
			trim_left(strline);
			
			if( strBetween.empty() ) {
				ARCHY_CERR << "Empty int '<' and '>' at line:" << iLine << endl;
				return false;
			}
			if( !(isalpha(strBetween[0])) && !(isdigit(strBetween[0]))) {
				ARCHY_CERR << "Illegle character after < at line:" << iLine << endl;
				return false;
			}
			xml_config* newxml = new xml_config;
			newxml->m_father = this;
			xml->m_lstSection.push_back(newxml);
			bool bSingleRet;				
			if( strBetween[strBetween.length() - 1 ] == '/' ) {
				strBetween.erase(strBetween.length() - 1);
				trim_right(strBetween);
				
				bSingleRet = parse_single(strBetween, newxml, iLine);
				if( bSingleRet == false ) return false;
				continue;
			}
			trim_right(strBetween);
			bSingleRet = parse_single(strBetween, newxml, iLine);
			if( bSingleRet == false ) return false;
                        //cerr << "Enter:" << newxml->m_secName << endl;
			bSingleRet = parse_internal(strline, newxml, ofile, iLine );
                        //cerr << "Outer:" << newxml->m_secName << endl;
			if( bSingleRet == false ) return false;
			continue;			
		}
		
		// 开始处理值, 值与子标记互不相容，有值存在就不应该有子标记存在
		if( !xml->m_lstSection.empty() ) {
			ARCHY_CERR << "Val is not permitted at line: " << iLine << endl;
			return false;
		}
		
		// 寻找<
		pos1 = strline.find('<');
		if( pos1 == string::npos ) {
			if( strline.length() > 512 ) {
				ARCHY_CERR << "Cannot find  flag > within 512 Bytes. " 
					<< "The value block's maxsize is 512 Byte!"
					<< "Please check the file" << endl;
				return false;
			}
			m_bNeedNextLine = true;
			continue;		
		}
		string strValBlock = strline.substr(0, pos1);
		strline.erase(0, pos1);
		
		xml->m_strval = trim_right(strValBlock);
	}
        if( xml->m_secName.empty())return true;
        
	return false;
}

bool xml_config::parse(const string& strFileName)
{
        m_bNeedNextLine = true;
	archy::file ofile;
	int ret = ofile.open(strFileName, "r");
	
	if( ret < 0 ) return false;
	
	int iLine = 0;
	string strLine;
	bool bRet = parse_internal(strLine, this, ofile, iLine);
	
	if( !bRet ) {
		xml_config::clear();
		return false;
	}

        return true;
}


bool xml_config::create_xml(file& ofile, string& strtabs)
{
	string strline;
	
	strline += strtabs + string("<")  + (m_secName.empty()? string("root") : m_secName);
	
	list<pair<string, string> >::iterator it;
	for( it = m_lstPair.begin(); it != m_lstPair.end(); ++it ) {
		strline += string(" ") + it->first + string("=\"") + it->second + string("\"");
	}
        if( m_strval.empty() && m_lstSection.size() == 0 ) {
        	strline += "/";
        }
	if( !m_strval.empty() ) {
        	strline += string(">") + m_strval;
                strline += "</"  + (m_secName.empty()? string("root") : m_secName);
        }

        strline += ">\n";

	
	int ret = ofile.write_line(strline);
	if( ret < 1 ) return false;

	if( !m_strval.empty() ) return true;

        if( m_strval.empty() && m_lstSection.size() == 0 ) return true;

	list<xml_config*>::iterator itt;
	string strTabNext = strtabs + "  ";
	for( itt = m_lstSection.begin(); itt != m_lstSection.end(); ++itt) {
		if( !(*itt)->create_xml(ofile, strTabNext) )
			return false;
	}
	
	strline = strtabs;
	strline += "</"  + (m_secName.empty()? string("root") : m_secName) + string(">\n");
	ret = ofile.write_line(strline);
	if( ret < 1 ) return false;
	
	return true;
}

bool xml_config::create_xml_file(const string& strFileName)
{
	archy::file ofile;
	int ret = ofile.open(strFileName, "w");
	
	if( ret < 0 ) return false;
	
	string strtabs;
	list<xml_config*>::iterator itt;

	for( itt = m_lstSection.begin(); itt != m_lstSection.end(); ++itt) {
		if( !(*itt)->create_xml(ofile, strtabs) )
			return false;
	}

	return true;
}

bool xml_config::exist_val(const string& strSec)
{
	archy::xml_config* xml = this;
	xml = xml->cd(strSec);
	if( xml == 0 ) 
		return false;
	return true;
}
string xml_config::get_val(const char* sec1, ...)
{
	va_list ap;
	va_start(ap, sec1);
	archy::xml_config* xml = this;
	while(sec1 != NULL) {
		xml = xml->cd(sec1);
		if( xml == 0 ) {
			va_end(ap);
			return "";
		}
		sec1 = va_arg(ap, const char*);
	}
	va_end(ap);
	return xml->get_val();

}

// write methods
// 添加一个xml子节点，if( xml == 0 || xml == this ) return false
bool xml_config::add_xml(xml_config* xml)
{
	if( xml == 0 || xml == this || !m_strval.empty() ) return false;
	m_lstSection.push_back(xml);
	return true;
}

// 清楚所有的xml字节点，返回true
bool xml_config::clear_xml()
{
	list<xml_config*>::iterator it;
	for( it = m_lstSection.begin(); it != m_lstSection.end(); ++it) {
		(*it)->clear();
	}
	m_lstSection.clear();
	
	return true;
}
// 清楚当前节点的所有属性,返回true
bool xml_config::clear_properties()
{
	m_lstPair.clear();
	return true;
}
// 添加当前节点的一个属性，如果属性已存在，返回false，否则返回true
bool xml_config::add_property(const string& strKey, const string& strVal)
{
	// 查看属性是否已经重复
        list<pair<string, string> >::iterator it;
        for( it = m_lstPair.begin(); it != m_lstPair.end(); ++it ) {
        	if( it->first == strKey ) {
			ARCHY_CERR << "found a duplicated property" << endl;
			return false;
                }
        }
	m_lstPair.push_back(make_pair(strKey, strVal));
	return true;
	
}
// 修改当前节点的一个属性，如果属性不存在，返回false，否则返回true
bool xml_config::update_property(const string& strKey, const string& strVal)
{
	// 查看属性是否已经重复
        list<pair<string, string> >::iterator it;
        for( it = m_lstPair.begin(); it != m_lstPair.end(); ++it ) {
        	if( it->first == strKey ) {
			it->second = strVal;
			return true;
                }
        }
        return false;
}
// 修改当前的标记名称
bool xml_config::update_name(const string& strSecName)
{
	if( m_secName.empty() )
		return false;
	
	m_secName = strSecName;
	return true;
	
}
// 修改当前标记的值，如果当前标记有子标记存在，则返回false
// 否则返回true
bool xml_config::update_val(const string& strVal)
{
	if(m_lstSection.empty() ) {
		m_strval = strVal;
		return true;
	}
	return false;
}

bool xml_config::clear_val()
{
	m_strval.erase();
        return true;
}

} // namespace archy


