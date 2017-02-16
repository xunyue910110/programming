//////////////////////////////////////////////////////////////////////
// 
// 描述: 实用小函数
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-17 13:40
// 版本: V1.0.0 
// 版权: 南京联创系统集成股份有限公司
//////////////////////////////////////////////////////////////////////
#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#if !defined(ARCHY_EXPERT_UTILS_H)
#define ARCHY_EXPERT_UTILS_H
#include <string>
#include <list>
#include <vector>
#include "archy_config.hpp"
#include "utils.hpp"
USING_STD_NAMESPACE;

namespace archy {

string& trim_left(string& str, const string& strSet = " \r\n\t");
string& trim_right(string& str, const string& strSet = " \r\t");
string& trim(string& str, const string& strSet = " \r\n\t");
string& to_upper(string& str);
string& to_lower(string& str);
string  to_string(long iNum, int iSize = 0);
char *itoa(long i, char *buf, int base);
string& add_slash(string& strDir);
bool is_number(const string& strSrc);

list<string> split_string(const string& strSrc, const string& strSplitSet, 
						  bool bSkipEmpty = true);
vector<string> split_string_to_vector(const string& strSrc, 
									  const string& strSplitSet, bool bSkipEmpty = true);
int ReadFixedHeadTailMsg(char* buf, 
						 const char* fixedHead, const char* fixedTail);
}

#endif //ARCHY_EXPERT_UTILS_H

