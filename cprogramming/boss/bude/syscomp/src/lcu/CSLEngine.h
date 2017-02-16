/**
 *
 * <p> 标题: CSLEngine </p>
 * <p> 描述：simple language（简单脚本语言）类 </p>
 * <p> 版权: Copyright (c) 2003 </p>
 * <p> 公司: Linkage </p>
 * @author wangxr@lianchuang.com
 * @version 1.0
 */

#ifndef CSLEngine_HEADER_INCLUDED
#define CSLEngine_HEADER_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "CDataList.h"
#include "CCdrAttr.h"

#include "CPair.h"
#include "CSymbolParser.h"
#include "CSLEngineFunction.h"
using std::ios;
//using std::in;

struct KeywordStruct {
    string keyword;
    int symbol;
    
    KeywordStruct(string str, int sym){
        keyword = str;
        symbol = sym;
    }
};

namespace engine{
	enum
	{
	    SYMBOL_WHERE = 4,           //where
	    SYMBOL_STAT,                //stat
	    SYMBOL_GROUP,               //group
	    SYMBOL_BY,                  //by
	    SYMBOL_IF,                  //if
	    SYMBOL_ELSE,                //else
	    SYMBOL_CALL,                //call
	    SYMBOL_DOT,                 //.
	    SYMBOL_PLUS,                // +     
	    SYMBOL_SUB,                 // -     
	    SYMBOL_MULTI,               // *     
	    SYMBOL_DIV,                 // /     
	    SYMBOL_MOD,                 // % 
	    SYMBOL_LIKE,                // like    
	    SYMBOL_GT,                  // >     
	    SYMBOL_LT,                  // <     
	    SYMBOL_GE,                  // >=    
	    SYMBOL_LE,                  // <=    
	    SYMBOL_EQ,                  // =     
	    SYMBOL_EE,                  // ==    
	    SYMBOL_NOT,                 // !,not 
	    SYMBOL_NOTE,                // !=    
	    SYMBOL_AND,                 // and   &&
	    SYMBOL_OR,                  // or  ||
	    SYMBOL_CAT,                 // ||    
	    SYMBOL_COMMA,               // ,           
	    SYMBOL_SEMICOLON,           // ;     
	    SYMBOL_LBRACKET,            // (     
	    SYMBOL_RBRACKET,            // )
	    SYMBOL_LCS,                 // {
	    SYMBOL_RCS                  // }
	};
};

const KeywordStruct KeywordArray[]  =                           //engine目前所用到的关键字
{
    KeywordStruct("if", engine::SYMBOL_IF),
    KeywordStruct("如果", engine::SYMBOL_IF),
    KeywordStruct("else", engine::SYMBOL_ELSE),
    KeywordStruct("stat", engine::SYMBOL_STAT),
    KeywordStruct("where", engine::SYMBOL_WHERE),
    KeywordStruct("group", engine::SYMBOL_GROUP),     
    KeywordStruct("by", engine::SYMBOL_BY),    
    KeywordStruct("call", engine::SYMBOL_CALL),    
    KeywordStruct(".", engine::SYMBOL_DOT),       
    KeywordStruct("+", engine::SYMBOL_PLUS),      
    KeywordStruct("-", engine::SYMBOL_SUB),       
    KeywordStruct("*", engine::SYMBOL_MULTI),     
    KeywordStruct("/", engine::SYMBOL_DIV),       
    KeywordStruct("%", engine::SYMBOL_MOD),       
    KeywordStruct("like", engine::SYMBOL_LIKE),      
    KeywordStruct(">", engine::SYMBOL_GT),        
    KeywordStruct("<", engine::SYMBOL_LT),        
    KeywordStruct(">=", engine::SYMBOL_GE),        
    KeywordStruct("<=", engine::SYMBOL_LE),        
    KeywordStruct("=", engine::SYMBOL_EQ),        
    KeywordStruct("==", engine::SYMBOL_EE),        
    KeywordStruct("!", engine::SYMBOL_NOT),       
    KeywordStruct("!=", engine::SYMBOL_NOTE),      
    KeywordStruct("and", engine::SYMBOL_AND),       
    KeywordStruct("or", engine::SYMBOL_OR),    
    KeywordStruct("&&", engine::SYMBOL_AND),  
    KeywordStruct("||", engine::SYMBOL_OR),       
    KeywordStruct(",", engine::SYMBOL_COMMA),     
    KeywordStruct(";", engine::SYMBOL_SEMICOLON), 
    KeywordStruct("(", engine::SYMBOL_LBRACKET),  
	KeywordStruct(")", engine::SYMBOL_RBRACKET),
	KeywordStruct("{", engine::SYMBOL_LCS),
    KeywordStruct("}", engine::SYMBOL_RCS)
};

typedef void (*pFunctionHandle)(int & t, CDataList & data_stack);   //函数句柄，函数内部负责出栈，主要是为了不用传值

struct CLocalVarStruct                                              //程序内置变量结构体
{
    string  name;
    int     type;
    CVar    value;
};

typedef  CPair<vector<CLocalVarStruct> *, int> CLocalVarReferrence; //本地变量的引用

inline CVar & getVarRef(CLocalVarReferrence & var)
{
    return (*var.first)[var.second].value;
}

class CSLEngine
{
public:
    
    typedef CPair<string, pFunctionHandle>  CFunctionPair;          //函数对
    
    typedef vector<CFunctionPair> CFunctionMap;                     //函数句柄存储类
    
    enum Func{
        LOAD_VAR,                                                   //变量进栈
        LOAD_LOCAL_VAR,                                             //加载程序内部变量
        LOAD_CONST,                                                 //常量数据进栈
        OPER,                                                       //栈顶与次栈顶的内容进行运算，结果放次栈顶
        STOR,                                                       //栈顶的内容给变量
        STOR_LOCAL_VAR,                                             //将栈顶的内容保存到程序内部变量中去
        CALL,                                                       //函数调用，结果保存在栈顶
        JUMP,                                                       //无条件转移
        JUMP_Y,                                                     //根据栈顶值转移，如果栈顶值为1则转移
        JUMP_N                                                      //根据栈顶值转移，如果栈顶值为0则转移
    };
    
    struct OperationCommand{
        Func func;                                                  //功能
        int level;                                                  //层差
        int op_switch;                                              //操作软开关
        OperationCommand(Func f, int l, int o){                     //构造函数
            func = f;
            level = l;
            op_switch = o;
        }
    };
    
    
public:
    
    /**
     * 函数名称：bool compile(const char * stat_rule, const char * cdr_type_def_file)
     * 函数功能：编译功能能
     * 使用注意：无
     * @param   stat_rule           统计规则，例如 count(*), sum(calling_duration) where area_code = '025' group by area_code
     * @param   cdr_type_def_file   话单类型定义文件，例如 StdCdr.ct
     * @return  bool | false
     * @throws
     */   
    bool compile(const char * stat_rule, const char * cdr_type_def_file = NULL);
    bool LogicExpressionCompile(const char * stat_rule, const char * cdr_type_def_file = NULL);
    bool MathExpressionCompile(const char * stat_rule, const char * cdr_type_def_file = NULL);
    
    
    /**
     * 函数名称：bool compileFile(const char * ScriptFile, const char * cdr_type_def_file)
     * 函数功能：编译一个文件
     * 使用注意：无
     * @param   ScriptFile          脚本文件
     * @param   cdr_type_def_file   话单类型定义文件，例如 StdCdr.ct
     * @return  bool | false
     * @throws
     */   
    bool compileFile(const char * ScriptFile, const char * cdr_type_def_file);
    
    /**
     * 函数名称：CLocalVarReferrence addLocalVar(const char * var_name, int data_type)
     * 函数功能：增加程序内部变量
     * 使用注意：无
     * @param   var_name            变量名称
     * @param   data_type           变量的数据类型
     * @return  CLocalVarReferrence 返回给外部的变量类型
     * @throws
     */  
    CLocalVarReferrence addLocalVar(const char * var_name, int data_type){
        __LocalVars.resize(__LocalVars.size() + 1);
        __LocalVars.rbegin()->name = var_name;
        __LocalVars.rbegin()->type = data_type;
        if( data_type == 1 )
            __LocalVars.rbegin()->value = 0;
        else if( data_type == 2)
            __LocalVars.rbegin()->value = 0.0;
        else if( data_type == 3 )
            __LocalVars.rbegin()->value = ""; 
            
        return CLocalVarReferrence(&__LocalVars, __LocalVars.size() - 1);
    }
    
    /**
     * 函数名称：void addFunction(const char * func_name, pFunctionHandle func)
     * 函数功能：增加函数
     * 使用注意：无
     * @param   func_name           函数名称
     * @param   func                函数句柄
     * @return  无
     * @throws
     */  
    void addFunction(const char * func_name, pFunctionHandle func){
        __FunctionMap.push_back(CFunctionPair(func_name, func));
    }
    
    /**
     * 函数名称：bool execute(CDataList & cdr)
     * 函数功能：执行代码
     * 使用注意：无
     * @param   cdr                 变量区
     * @return  无
     * @throws
     */  
    bool execute(vector<CVar> & cdr);                  //执行代码
    
    
    //! 获取逻辑表达式、算术表达式的执行结果
    const CVar & getExecuteResult(){
        return * __StackTopValue;
    }
    
    /**
     * 函数名称：vector<string> & getErrorMsgList()
     * 函数功能：获取错误信息列表
     * 使用注意：无
     * @param   
     * @return  vector<string> &                    列表引用
     * @throws
     */  
    vector<string> & getErrorMsgList(){
        return __ErrorMsgList;
    }
    
    void output_code(){
        for( unsigned int i = 0; i < __Code.size(); i++ ){
            cout << i << "\t\t";
            switch(__Code[i].func){
                case LOAD_VAR: 
                    cout << "LOAD_VAR";
                    break;
                case LOAD_LOCAL_VAR: 
                    cout << "LOAD_LOCAL_VAR";
                    break;
                case LOAD_CONST:
                    cout << "LOAD_CONST";
                    break;
                case OPER:     
                    cout << "OPER";
                    break;
                case STOR:     
                    cout << "STOR";
                    break;
                case STOR_LOCAL_VAR:     
                    cout << "STOR_LOCAL_VAR";
                    break;
                case CALL:     
                    cout << "CALL";
                    break;
                case JUMP:     
                    cout << "JUMP";
                    break;
                case JUMP_Y:   
                    cout << "JUMP_Y";
                    break;
                case JUMP_N:    
                    cout << "JUMP_N";
                    break;
            }
            cout << "\t\t";
            cout << __Code[i].op_switch << endl;
        }
    }
    
protected:
    
    CCdrAttr                    *__CdrType;         //话单结构定义，也是变量声明区
    
    static CSymbolParser        __Lex;              //词法解析器
    static bool                 __IsLexInit;        //判断词法解析器是否已经初始化过了
    
    vector<OperationCommand>    __Code;             //执行代码
    
    vector<CVar>                __ConstVars;        //常量数组
    
    CDataList                   __DataStack;        //数据堆栈区，可以自动扩展大小，内存不会再减小
    
    CFunctionMap                __FunctionMap;      //函数句柄存储
    
    vector<CLocalVarStruct>     __LocalVars;        //程序内部供外部使用的变量
        
    void block();                                   //开始一个代码块
    
    void function();                                //函数
    
    void statement();                               //语句
    
    void conditions();                              //条件表达式
        
    void condition();                              
    
    void condfactor();
    
    void comp_clause();
    
    void expression();                              //表达式
    
    void factor();                                  //因子
    
    void term();                                    //项
    
    void getsym();                                  //获取下一个符号
    
    SymbolPair      __Symbol;                       //上一个符号标识
    
    vector<string>  __ErrorMsgList;                 //错误信息列表
    
    CVar *          __StackTopValue;                //指向执行结果后的最后栈顶值， 用于逻辑表达式或者是算术表达式的最后结果

public:

    CSLEngine();
    
    virtual ~CSLEngine(){}
};

#endif  /* CSLEngine_HEADER_INCLUDED */
