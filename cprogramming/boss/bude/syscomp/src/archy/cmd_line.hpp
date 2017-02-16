//////////////////////////////////////////////////////////////////////
// 
// 类名: cmd_line
// 描述: 命令行输入解析, 例如 cmd name1[=val1] name2[=val2]
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////
#ifndef ARCHY_CMD_LINE_HEADER
#define ARCHY_CMD_LINE_HEADER
#include "archy_config.hpp"

namespace archy {

class cmd_line {
public:
	cmd_line(int argc, char *argv []);
	
	std::string get_param (int index)const;
	std::string get_param (const std::string &name, const std::string &default_value)const;
	char get_param (const std::string &name, char default_value)const;
	int get_param (const std::string &name, int default_value)const;
	bool exists (const std::string &name)const;
	int get_argc()const { return m_argc; }
	char** get_argv()const { return m_argv; }
private:
	int m_argc;
	char **m_argv;
};

}// namespace archy

#endif // ARCHY_CMD_LINE_HEADER
