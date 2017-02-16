//////////////////////////////////////////////////////////////////////
// 
// 类名: xml_config
// 描述: 读写形如xml格式的配置文件
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-18 7:55
// 版本: V1.0.1 
// 版权: 南京联创科技股份有限公司
// 修改: 增加向上连接，形成双链树,archy, 2003-7-29
//////////////////////////////////////////////////////////////////////
#ifndef ARCHY_XML_CONFIG_INCLUDE_HEADER
#define ARCHY_XML_CONFIG_INCLUDE_HEADER

#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include <exception>
#include <list>
#include <utility>

#include "archy_config.hpp"
USING_STD_NAMESPACE;

namespace archy
{
class file;

class xml_config
{
public:
	// ctor & copy ctor & assign  & dtor	
	xml_config();
	xml_config(const xml_config& xml);
	xml_config& operator = (const xml_config& xml);
	~xml_config();

    // 生成一个标记名为name的xml节点
    xml_config(const string& name);
    // 生成一个标记名为name, 值为val的xml节点
    xml_config(const string& name, const string& val);

	// parse a xml file 
	bool parse(const string& strFileName);

	// read methods
	bool exist_property(const string& strKey)const;
	string get_property(const string& strKey)const throw(runtime_error);
	const list<pair<string, string> >& get_property()const{ return m_lstPair; }	
	
	string get_val()const { return m_strval; }
	// 最后的一个参数必须是NULL
	string get_val(const char* sec1, ...);
	bool exist_val(const string& strSec);

	// 象unix切换目录, /根目录, .当前目录, ..父目录
	xml_config* cd(const string& name);
	const string& get_name()const { return m_secName; }
	const list<xml_config*>&  ls()const { return m_lstSection; }
	// 获取当前目录下与name同名的节点
    list<xml_config*> ls(const string& name) ;
	
	// create a xml file according to this object
	bool create_xml_file(const string& strFileName);
	
    // clear all the content in this object
	void clear();

    // write methods
    // 添加一个xml子节点，if( xml == 0 || xml == this ) return false
    // 如果当前标记存在值，也返回false
    
    bool add_xml(xml_config* xml);
    // 清楚所有的xml字节点，返回true
    bool clear_xml();
    // 清楚当前节点的所有属性,返回true
    bool clear_properties();
    // 添加当前节点的一个属性，如果属性已存在，返回false，否则返回true
    bool add_property(const string& strKey, const string& strVal);
    // 修改当前节点的一个属性，如果属性不存在，返回false，否则返回true
    bool update_property(const string& strKey, const string& strVal);
	// 修改当前的标记名称
    bool update_name(const string& strSecName);
    // 修改当前标记的值，如果当前标记有子标记存在，则返回false
    // 否则返回true
    bool update_val(const string& strVal);
    // 清楚当前节点的值
    bool clear_val();

        
private:
	bool create_xml(file& ofile, string& strtabs);
	
	bool parse_internal(string& strline, xml_config* xml, file& ofile, int& iLine);
	
	bool parse_single(const string& strBetween, xml_config* xml, int iLine);
	
private:

	// 标记名
	string m_secName;

	xml_config* m_father; // 父标记
	// 子标记
	list<xml_config*>  m_lstSection;
	
	// 标记的属性
	list<pair<string, string> > m_lstPair;
	
	// 当前标记的值，该值与子标记互不相容，有该值存在就不应该有子标记存在
	// 反之，有子标记存在就不应该有该值存在
	string m_strval;

    bool m_bNeedNextLine;
}; 

} // namespace archy

#endif

