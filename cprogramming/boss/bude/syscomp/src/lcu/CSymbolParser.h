/**
 *
 * <p> 标题: CSymbolParser </p>
 * <p> 描述：该类根据关键字来求出一些符号 </p>
 * <p> 版权: Copyright (c) 2003 </p>
 * <p> 公司: Linkage </p>
 * @author wangxr@lianchuang.com
 * @version 1.0
 */

#ifndef CSYMBOLPARSER_HEADER_INCLUDED
#define CSYMBOLPARSER_HEADER_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <ctype.h>

#include "CPair.h"

#define STRING_CHAR     '\''                                    //字符串常量的起始与结束符号
#define SYMBOL_INT      0                                       //整型值常量
#define SYMBOL_FLOAT    1                                       //浮点数常量
#define SYMBOL_STRING   2                                       //字符串型常量
#define SYMBOL_IDENT    3                                       //标识符
using std::string;
using std::map;
using std::vector;

/*
typedef CPair<int, string>      	SymbolPair;               	//作为getNextSymbol返回的结果
*/

//! 符号位置
struct SymbolLocation
{
    int LineNo;
    int LinePos;
    
    SymbolLocation(){};
    
    SymbolLocation(int l, int p){
        LineNo = l;
        LinePos = p;
    }
};

//! 符号重新定义，内置了符号所在位置
struct SymbolPair
{
    int first;
    string second;
    SymbolLocation location;
    
    //! 默认构造函数
    SymbolPair(){}
    
    bool operator == (const SymbolPair & right){
        return first == right.first;
    }
    
    SymbolPair & operator = (const SymbolPair & right){
        first = right.first;
        second = right.second;
        location = right.location;
        return *this;
    }
    
    SymbolPair( const SymbolPair & right ){
        *this = right;
    }
    
    //! 带参数的构造函数
    SymbolPair(int f, string s, SymbolLocation l){
        first = f;
        second = s;
        location = l;
    }
    
    //! 带参数的构造函数
    SymbolPair(int f, string s, int line_no, int line_pos){
        first = f;
        second = s;
        location.LineNo = line_no;
        location.LinePos = line_pos;
    }
};


const SymbolPair SYMBOL_FALSE(-2, "", -2, -2);
const SymbolPair SYMBOL_END(-1, "", -1, -1);

class CSymbolParser
{
public:

	typedef map<string, int>        CKeywordMap;                //存储关键字的map定义

public:
    
    /**
     * 函数名称：void setSourceText(const char * src)
     * 函数功能：设置要解析的文本
     * 使用注意：无
     * @param   src                     文本
     * @return  无
     * @throws
     */ 
    void setSourceText(const char * src){
        __StringBody    = src;
        __CurrentIndex  = 0;
    }
    
    /**
     * 函数名称：void statCdr(const CStdCdr & cdr)
     * 函数功能：增加一个关键字
     * 使用注意：无
     * @param   keyword                 关键字
     * @param   symbol                  符号，请输入大于3的正值，从4开始
     * @return  无
     * @throws
     */  
    void addKeyWord(const char * keyword, int symbol){
        __KeywordMap[keyword] = symbol;
        if( __MaxKeywordLength == -1 ){                         //第一次添加关键字
            __MaxKeywordLength = __MinKeywordLength = strlen(keyword);
            return;
        }
        
        if( strlen(keyword) > static_cast<unsigned int>(__MaxKeywordLength) )              //设置最长关键字长度
            __MaxKeywordLength = strlen(keyword);
        else if( strlen(keyword) < static_cast<unsigned int>(__MinKeywordLength) )         //设置最短关键字长度
            __MinKeywordLength = strlen(keyword);
    }
    
    /**
     * 函数名称：bool parseText(const char * text)
     * 函数功能：解析一个文本，结果保存到符号表里
     * 使用注意：无
     * @param   无
     * @return  true | false
     * @throws
     */  
	bool parseText(const char * text){
		//设置要解析的文本
		setSourceText(text);
		
		__CurrentLineNo = 1;
		__CurrentLinePos = 0;
		
		//清空符号表
		__SymbolList.clear();
		__CurrentSymbolIndex = 0;
        while(true){
            SymbolPair rtn = __getNextSymbol();
            //先保存起来
            __SymbolList.push_back(rtn);          
            if( rtn == SYMBOL_END )
                return true;
            else if( rtn == SYMBOL_FALSE )
                return false;
		}
    }
    
    /**
     * 函数名称：oid clear()
     * 函数功能：清空符号表以及脚本内容
     * 使用注意：无
     * @param   无
     * @return  void
     * @throws
     */  
    void clear(){
        __SymbolList.clear();
        __StringBody = "";
    }
    
    /**
     * 函数名称：SymbolPair & operator [] (int n)
     * 函数功能：返回符号表里的第n个符号
     * 使用注意：无
     * @param   无
     * @return  true | false
     * @throws
     */  
    SymbolPair & operator [] (int n){
        return __SymbolList[n];
    }
    
    int getSymbolCount() const{
        return __SymbolList.size();
    }
    
    SymbolPair & getNextSymbol(){
        __CurrentSymbolIndex++;
        return __SymbolList[__CurrentSymbolIndex - 1];
    }
    
    SymbolPair & testNextSymbol(){
        return __SymbolList[__CurrentSymbolIndex];
    }
    
    void unget(){
        __CurrentSymbolIndex --;
    }
    /**
     * 函数名称：const char * getErrorInfo()
     * 函数功能：获取最后一个错误信息
     * 使用注意：无
     * @param   无
     * @return  true | false
     * @throws
     */  
    const char * getErrorInfo() const{
        return __ErrorInfo.c_str();
    }      
    
    /**
     * 函数名称：const char * getKeyword(int symbol_id) const
     * 函数功能：根据符号ID获取关键字
     * 使用注意：无
     * @param   无
     * @return  true | false
     * @throws
     */  
    const char * getKeyword(int symbol_id) const{
        CKeywordMap::const_iterator itr;
        for(itr = __KeywordMap.begin(); itr != __KeywordMap.end(); itr++){
            if( (*itr).second == symbol_id )
                return (*itr).first.c_str();
        }
        
        return NULL;
    }
    
    int getSymbolId(const char * keyword){
        CKeywordMap::const_iterator itr;
        for(itr = __KeywordMap.begin(); itr != __KeywordMap.end(); itr++){
            if( (*itr).first == keyword )
                return (*itr).second;
        }
        return -1;
    }
    
protected:
    /**
     * 函数名称：SymbolPair  getNextSymbol()
     * 函数功能：获取下一个符号
     * 使用注意：无
     * @param   无
     * @return  SymbolPair，当SymbolPair.first为-1的时候表示已经解析到结尾
     *          当为-2的时候表示解析出错
     *          其他正值的时候表示符号，如果是常量的话则second表示常量
     * @throws
     */  
    SymbolPair  __getNextSymbol(){
        unsigned int i, j, k;
        string const_string;                                    //字符串常量

        for( i = __CurrentIndex; static_cast<unsigned int>(i) < __StringBody.length(); i++ ){
            if( isspace(__StringBody[i]) ){                     //如果是空格跳过 
                //! 如果是换行符，就行号++
                if( __StringBody[i] == '\n' ){
                    __CurrentLineNo++;
                    __CurrentLinePos = i;
                }
                __CurrentIndex++;
                continue;
            }        
            
            if( isdigit(__StringBody[i]) ){                     //如果是数字
                i++;
                for(;static_cast<unsigned int>(i) < __StringBody.length(); i++){
                    if(!isdigit(__StringBody[i]) && __StringBody[i] != '.'){
                        //应该是整型值
                        string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                        __CurrentIndex = i;
                        return SymbolPair(SYMBOL_INT, s, __CurrentLineNo, i - __CurrentLinePos);
                    }
					else if(__StringBody[i] == '.'){            //应该是浮点数
						i++;
						for(;static_cast<unsigned int>(i) < __StringBody.length(); i++){
                            if(!isdigit(__StringBody[i])){
                                string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                                __CurrentIndex = i;
								return SymbolPair(SYMBOL_FLOAT, s, __CurrentLineNo, i - __CurrentLinePos);
                            }
                        }
                        string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                        __CurrentIndex = i;
					    return SymbolPair(SYMBOL_FLOAT, s, __CurrentLineNo, i - __CurrentLinePos);
                        //! 如果这时已经到达脚本末尾
                    }
                }
                if(static_cast<unsigned int>(i) == __StringBody.length()){
                    string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                    __CurrentIndex = i;
                    return SymbolPair(SYMBOL_INT, s, __CurrentLineNo, i - __CurrentLinePos);
                }
            }
            else if( __StringBody[i] == STRING_CHAR ){          //字符串常量的开始
                i++;
                for(;static_cast<unsigned int>(i) < __StringBody.length(); i++){
                    if( __StringBody[i] == '\\' ){              //转义字符
                        i++;
                        if( static_cast<unsigned int>(i) == __StringBody.length() ){
                            __ErrorInfo = "没有结束的字符串.";
                            return SYMBOL_FALSE;
                        }
                        else if( __StringBody[i] == 't' )
                            const_string += '\t';
                        else if( __StringBody[i] == 'r' )
                            const_string += '\r';
                        else if( __StringBody[i] == 'n' )
                            const_string += '\n';
                        else
                            const_string += __StringBody[i];
                        continue;
                    }
					else if( __StringBody[i] == STRING_CHAR ){  //常量字符结束
						i++;
                        __CurrentIndex = i;
                        return SymbolPair(SYMBOL_STRING, const_string, __CurrentLineNo, i - __CurrentLinePos);
                    }
                    else
                        const_string += __StringBody[i];
                }
                if(i == __StringBody.length()){
                    __ErrorInfo = "没有结束的字符串.";
                    return SYMBOL_FALSE;
                }
            }
            else if( !isIdentChar(__StringBody[i]) ){           //是其他字母形成的关键字
                unsigned int count = __MaxKeywordLength - __MinKeywordLength;
                for(j = 0; j <= count; j++){
                    string s = __StringBody.substr(i, __MaxKeywordLength - j);
                    CKeywordMap::iterator itr;
                    itr = __KeywordMap.find(s);
                    if( itr != __KeywordMap.end() ){            //找到了，说明是关键字
						i += __MaxKeywordLength - j;
                        __CurrentIndex = i;
                        return SymbolPair((*itr).second, s, __CurrentLineNo, i - __CurrentLinePos);
                    }
                }
                //没找到的话则返回错误
                __ErrorInfo = "无法识别的符号.";
                return SYMBOL_FALSE;
            }
            else{
                unsigned int count = __MaxKeywordLength - __MinKeywordLength;
                for(j = 0; j <= count; j++){
                    string s = __StringBody.substr(i, __MaxKeywordLength - j);
                    CKeywordMap::iterator itr;
                    itr = __KeywordMap.find(s);
                    if( itr != __KeywordMap.end() ){            //找到了，说明是关键字
                        i += __MaxKeywordLength - j;
                        if( i != __StringBody.length() &&       //如果后面没有结束，且后面是identchar或者是数字，跳出循环
                            (isdigit(__StringBody[i]) ||
                            isIdentChar(__StringBody[i])) ){
                            i -= __MaxKeywordLength - j;
                            break;
                        }
                        else{                                   //说明是一个关键字
                            string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                            __CurrentIndex = i;
                            return SymbolPair((*itr).second, s, __CurrentLineNo, i - __CurrentLinePos);
                        }
                    }
                }
                //没找到的话则表明是ident，往后找，直到结束，或者是其他的符号
                i++;
                for(;i < __StringBody.length(); i++){
                    if( isdigit(__StringBody[i]) || isIdentChar(__StringBody[i]) )
                        continue;
                    else{
                        string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                        __CurrentIndex = i;
                        return SymbolPair(SYMBOL_IDENT, s, __CurrentLineNo, i - __CurrentLinePos);
                    }
                }
                string s = __StringBody.substr(__CurrentIndex, i - __CurrentIndex);
                __CurrentIndex = i;
                return SymbolPair(SYMBOL_IDENT, s, __CurrentLineNo, i - __CurrentLinePos);
            }
        }
        return SYMBOL_END;
    }   
    
    bool isIdentChar(char ch){
        const char * s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
        const char * pos = s;
        while( *pos != '\0' ){
            if( *pos == ch )
                return true;
            else
                pos++;
        }
        return false;
    }
    
private:
    
    string              __StringBody;                           //要解析的字符串
    
    int                 __CurrentIndex;                         //当前所在索引号
    
    CKeywordMap         __KeywordMap;                           //存储关键字的map
    
    string              __ErrorInfo;                            //错误信息
    
    int                 __MinKeywordLength;                     //最短的关键字长度
    
    int                 __MaxKeywordLength;                     //最长的关键字长度
    
    vector<SymbolPair>  __SymbolList;                           //存储解析出来的符号
    
    int                 __CurrentSymbolIndex;                   //当前符号的索引位置
    
    int                 __CurrentLineNo;                        //当前行号
    int                 __CurrentLinePos;                       //当前行的所在位置
     
public:

    CSymbolParser(){
        __MinKeywordLength = __MaxKeywordLength = -1;
    }
    
    virtual ~CSymbolParser(){}
    
};

#endif  /* CSYMBOLPARSER_HEADER_INCLUDED */
