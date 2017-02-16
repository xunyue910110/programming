
#ifndef __PATTERNMATCHER_H__
#define __PATTERNMATCHER_H__

///
/// @class PatternMatcher 
/// 
/// @brief 字符串模式匹配器
///
/// 
class PatternMatcher
{
public:
    /// 匹配字符串
    /// @pattern 匹配模式字符串
    /// @str 验证的字符串
    ///
	static bool matches(const char *pattern, const char *str);

};

#endif  /* __PATTERNMATCHER_H__  */


