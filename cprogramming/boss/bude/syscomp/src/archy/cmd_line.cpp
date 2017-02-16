#include "cmd_line.hpp"

USING_STD_NAMESPACE;

namespace archy {


cmd_line::cmd_line(int argc, char *argv []) 
:m_argc (argc), m_argv (argv) 
{
}


string cmd_line::get_param (int index)const {
	return string(m_argv [index]);
}
string cmd_line::get_param (const string &name,
                                     const string &default_value)const {
	string search = name + "=";
	for (int i = 0; i < m_argc; i++) {
		string tmp (m_argv [i]);
		if (tmp.find (search) == 0)
			return tmp.substr (search.length ());
	}
	return string (default_value);
}
int cmd_line::get_param (const string &name, int default_value) const{
	string tmp = get_param (name, "");
	if (tmp == "")
    		return default_value;
	return atoi (tmp.c_str ());
}
char cmd_line::get_param (const string &name, char default_value)const {
	string tmp = get_param (name, "");
	if (tmp == "")
    		return default_value;
	return tmp [0];
}
bool cmd_line::exists (const string &name) const{
	for (int i = 0; i < m_argc; i++) {
	   	if (m_argv [i] == name)
	        	return true;
	    	string tmp = m_argv [i];
	    	if (tmp.find (name + "=") == 0)
	        	return true;
	}
	return false;
}


} // namespace archy

