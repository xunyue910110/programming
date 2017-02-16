#include "CSLEngine.h"
#include "CException.h"
#include <fstream>
#include <sstream>

CSymbolParser        CSLEngine::__Lex;
bool                 CSLEngine::__IsLexInit = false;

CSLEngine::CSLEngine()
{
	__CdrType = NULL;
	addFunction("substr", engine_substr);
	addFunction("atoi", engine_atoi);
	addFunction("atof", engine_atof);
	addFunction("trim", engine_trim);
	addFunction("strlen", engine_strlen);
	addFunction("lower", engine_lower);
	addFunction("upper", engine_upper);
	addFunction("atol", engine_atol);
	//时间日期处理函数
	addFunction("datetimeadd", engine_datetimeadd);
	addFunction("datetimediff", engine_datetimediff);
	addFunction("sysdate", engine_sysdatetime);
	//过程体
	addFunction("print", engine_print);
	
	//词法解析器初始化
	if( !CSLEngine::__IsLexInit ){
	    for( int i = 0; i < sizeof(KeywordArray)/sizeof(KeywordStruct); i++){
	    	__Lex.addKeyWord(KeywordArray[i].keyword.c_str(), KeywordArray[i].symbol);
	    }
	    CSLEngine::__IsLexInit = true;
    }
}

bool CSLEngine::compile(const char * stat_rule, const char * cdr_type_def_file)
{
    __ErrorMsgList.clear();
    
    //! 话单定义
    if( cdr_type_def_file != NULL ){
	    __CdrType = new CCdrAttr;                                   //话单定义文件解析
	    if( !__CdrType->parseFile(cdr_type_def_file) ){
	        __ErrorMsgList.push_back(string(cdr_type_def_file) + "格式不正确，请检查.");
	        return false;
	    }
    }
    else{
        __CdrType = new CCdrAttr;
    }
	
	stringstream ss;
    ss << stat_rule;
    
    //! 开始去除注释行
    char line[1024*4];
	string StringBody;
    while(ss.getline(line, sizeof(line))){
        //去除注释行
        char * pos = line;
        while( isspace(*pos) ) pos++;
        if( !strncmp(pos, "//", 2) )    continue;
        //加入到编译体中去
		StringBody += line;
        StringBody += '\n';
    }
	
	if( !__Lex.parseText(StringBody.c_str()) ){
	    __ErrorMsgList.push_back(__Lex.getErrorInfo());
	    return false;
	}
	
	getsym();
	
	block();
	
	__Code.push_back(OperationCommand(OPER, 0, 0));             //退出处理
	
	delete __CdrType;       __CdrType = NULL;                   //删除话单定义对象
	
	__Lex.clear();
	
	if( __ErrorMsgList.size() > 0 )
	    return false;
    return true;
}

bool CSLEngine::LogicExpressionCompile(const char * stat_rule, const char * cdr_type_def_file)
{
    __ErrorMsgList.clear();
	
	//! 话单定义
    if( cdr_type_def_file != NULL ){
	    __CdrType = new CCdrAttr;                                   //话单定义文件解析
	    if( !__CdrType->parseFile(cdr_type_def_file) ){
	        __ErrorMsgList.push_back(string(cdr_type_def_file) + "格式不正确，请检查.");
	        return false;
	    }
    }
    else{
        __CdrType = new CCdrAttr;
    }
	
	if( !__Lex.parseText(stat_rule) ){
	    __ErrorMsgList.push_back(__Lex.getErrorInfo());
	    return false;
	}
	
	getsym();
	
	conditions();
	
	__Code.push_back(OperationCommand(OPER, 0, 0));             //退出处理
	delete __CdrType;       __CdrType = NULL;                   //删除话单定义对象
	
	__Lex.clear();
	
	if( __ErrorMsgList.size() > 0 ){       
	    common_exception e;
        for( unsigned int i = 0; i < __ErrorMsgList.size(); i++ ){
            e << __ErrorMsgList[i] << endl;
        }
	    throw e;
	}
    return true;
}

bool CSLEngine::MathExpressionCompile(const char * stat_rule, const char * cdr_type_def_file)
{
    __ErrorMsgList.clear();
	
	//! 话单定义
    if( cdr_type_def_file != NULL ){
	    __CdrType = new CCdrAttr;                                   //话单定义文件解析
	    if( !__CdrType->parseFile(cdr_type_def_file) ){
	        __ErrorMsgList.push_back(string(cdr_type_def_file) + "格式不正确，请检查.");
	        return false;
	    }
    }
    else{
        __CdrType = new CCdrAttr;
    }
	
	if( !__Lex.parseText(stat_rule) ){
	    __ErrorMsgList.push_back(__Lex.getErrorInfo());
	    return false;
	}
	
	getsym();
	
	expression();
	
	__Code.push_back(OperationCommand(OPER, 0, 0));             //退出处理
	
	delete __CdrType;       __CdrType = NULL;                   //删除话单定义对象
	
	__Lex.clear();
	
	if( __ErrorMsgList.size() > 0 ){
	    common_exception e;
        for( unsigned int i = 0; i < __ErrorMsgList.size(); i++ ){
            e << __ErrorMsgList[i] << endl;
        }
	    throw e;
	}
	
    return true;
}

bool CSLEngine::compileFile(const char * ScriptFile, const char * cdr_type_def_file)
{
    fstream fs;
    fs.open(ScriptFile, ios::in);
    if( fs.fail() ){
        __ErrorMsgList.push_back(string("无法打开文件") + ScriptFile);
        return false;
    }
	char line[1024*4];
	string StringBody;
    while(fs.getline(line, sizeof(line))){
        //去除注释行
        char * pos = line;
        while( isspace(*pos) ) pos++;
        if( !strncmp(pos, "//", 2) )    continue;
        //加入到编译体中去
		StringBody += line;
        StringBody += '\n';
    }
    fs.close();
	return compile(StringBody.c_str(), cdr_type_def_file);
}

void CSLEngine::block()
{
    statement();
    while( __Symbol.first == engine::SYMBOL_SEMICOLON ){
        getsym();
        statement();
    }
}

void CSLEngine::statement()
{
    if( __Symbol == SYMBOL_END )    return;
    
    if(__Symbol.first == engine::SYMBOL_STAT){
        getsym();
        expression();                                           //统计子句
	    while(__Symbol.first == engine::SYMBOL_COMMA){
	    	getsym();
	    	expression();
        }
        if( __Symbol.first == engine::SYMBOL_WHERE ){
            getsym();
            conditions();
        }
        if( __Symbol.first != engine::SYMBOL_GROUP ){
            __ErrorMsgList.push_back("必须有group by 子句.");
            return;
        }
        getsym();
        if( __Symbol.first != engine::SYMBOL_BY ){
            __ErrorMsgList.push_back("group 后面需要by.");
            return;
        }
        
        getsym();
        if( __Symbol.first > SYMBOL_IDENT ){
            __ErrorMsgList.push_back("group by 后面必须是常量或者是变量，或者是表达式.");
            return;
        }
        else{                                                   //登记group by 选项
            getsym();
        }
        while(__Symbol.first == engine::SYMBOL_COMMA){
            getsym();
            if( __Symbol.first >= 0 && __Symbol.first <= SYMBOL_IDENT ){
                //登记group by 选项
            }
            else if(__Symbol == SYMBOL_END ){
                return;
            }
            else{
                __ErrorMsgList.push_back("group by 后面必须是常量或者是变量，或者是表达式.");
                return;
            }
        }
    }
    else if( __Symbol.first == engine::SYMBOL_CALL ){
        getsym();
        function();
    }
    else if( __Symbol.first == SYMBOL_IDENT ){                  //应该是变量赋值语句
        int var_flag = 0;                                       //1外部变量，2程序内部变量       
        int index = __CdrType->index(__Symbol.second.c_str());
        if( index == -1 ){
            vector<CLocalVarStruct>::iterator itr;
            for(itr = __LocalVars.begin(); itr != __LocalVars.end(); itr++){
                if( itr->name == __Symbol.second )
                    break;
            } 
            if( itr != __LocalVars.end() ){                     //找到了，表示是程序内部变量
                var_flag = 2;
                index = itr - __LocalVars.begin();
            }
            else{                                               //内部也没找到，说明是未知的变量
                __ErrorMsgList.push_back(__Symbol.second + "是个未声明的变量.");
                return;
            }
        }
        else                                                    //不等于-1，表示是程序外部变量
            var_flag = 1;
        getsym();
        if( __Symbol.first != engine::SYMBOL_EQ ){
            __ErrorMsgList.push_back("缺少等于号 = ");
            return;
        }
        getsym();
        expression();
        if( var_flag == 1 )
            __Code.push_back(OperationCommand(STOR, 0, index)); //将栈顶的值赋值给变量
        else
			__Code.push_back(OperationCommand(STOR_LOCAL_VAR, 0, index));
    }
	else if( __Symbol.first == engine::SYMBOL_IF ){                     //应该是if - else 语句
       unsigned int i;
        getsym();
        if(__Symbol.first != engine::SYMBOL_LBRACKET ){
            __ErrorMsgList.push_back("if后缺少(");
            return;
        }
        getsym();
        conditions();
        if(__Symbol.first != engine::SYMBOL_RBRACKET ){
            __ErrorMsgList.push_back("if后缺少)");
            return;
        }
        getsym();                                               //后面应该是语句
        int insert_pos = __Code.size();
        if(__Symbol.first == engine::SYMBOL_LCS ){
            getsym();
            block();
            if( __Symbol.first != engine::SYMBOL_RCS ){
                __ErrorMsgList.push_back("缺少}");
                return;
			}
			getsym();
        }
		else{
			__ErrorMsgList.push_back("if条件后，必须有大括号{}");
			return;
		}
		for( i = insert_pos; i < __Code.size(); i++ ){      //插入前，先将后面的代码中要jump的+插入的代码数量
		    if(__Code[i].func == JUMP_Y || __Code[i].func == JUMP_N || __Code[i].func == JUMP )
		        __Code[i].op_switch += 2;
		}
        __Code.insert(__Code.begin() + insert_pos, OperationCommand(JUMP_Y, 0, insert_pos + 2));
        __Code.insert(__Code.begin() + insert_pos + 1, OperationCommand(JUMP_N, 0, __Code.size() + 2));
		insert_pos = __Code.size();

		if(__Symbol.first == engine::SYMBOL_ELSE){                      //如果有else子句
            getsym();
            if(__Symbol.first == engine::SYMBOL_LCS ){
                getsym();
                block();
                if( __Symbol.first != engine::SYMBOL_RCS ){
                    __ErrorMsgList.push_back("缺少}");
                    return;
				}
				getsym();
            }
			else{
				__ErrorMsgList.push_back("else条件后必须有{}");
				return;
			}
        }
        for(i = insert_pos; i < __Code.size(); i++){
		    if(__Code[i].func == JUMP_Y || __Code[i].func == JUMP_N || __Code[i].func == JUMP )
		        __Code[i].op_switch += 1;
		}
        __Code.insert(__Code.begin() + insert_pos, OperationCommand(JUMP, 0, __Code.size() + 1));
	}
}

void CSLEngine::function()
{
    if(__Symbol.first != SYMBOL_IDENT ){
        __ErrorMsgList.push_back("不是一个函数或者过程标志符.");
        return;
    }
    
    CFunctionMap::iterator itr;
    
    for(itr = __FunctionMap.begin(); itr != __FunctionMap.end(); itr++ ){
        if( (*itr).first == __Symbol.second )
            break;
    }
	
	if( itr == __FunctionMap.end() ){
	    __ErrorMsgList.push_back(__Symbol.second + "是个未知的函数.");
	    return;
	}
	
	//pFunctionHandle handle = (*itr).second;
    
    getsym();
    if( __Symbol.first != engine::SYMBOL_LBRACKET ){
        __ErrorMsgList.push_back("函数后面必须是(.");
        return;
    }
    
    getsym();
    expression();
    while(__Symbol.first == engine::SYMBOL_COMMA){
        getsym();
        expression();
    }
    
    if( __Symbol.first != engine::SYMBOL_RBRACKET ){
        __ErrorMsgList.push_back("函数必须以)结束.");
        return;
	}
	
	__Code.push_back(OperationCommand(CALL, 0, itr - __FunctionMap.begin()));
	
	getsym();
}

void CSLEngine::conditions()
{
    //conditions OPOR condition | condition
    condition();
    if( __Symbol.first == engine::SYMBOL_OR ){
        getsym();
        conditions();
        __Code.push_back(OperationCommand(OPER, 0, 15));
    } 
}
        
void CSLEngine::condition()
{
    //condition OPAND condfactor | condfactor
    condfactor();
    if( __Symbol.first == engine::SYMBOL_AND ){
        getsym();
        condition();
        __Code.push_back(OperationCommand(OPER, 0, 14));
    }
}                              

void CSLEngine::condfactor()
{
    //'(' conditions ')' | OPNOT condfactor | comp_clause
    if( __Symbol.first == engine::SYMBOL_LBRACKET ){
        getsym();
        conditions();
        if( __Symbol.first != engine::SYMBOL_RBRACKET ){
            __ErrorMsgList.push_back("缺少)");
            return;
		}
		getsym();
    }
    else if( __Symbol.first == engine::SYMBOL_NOT ){
        getsym();
        condfactor();
        __Code.push_back(OperationCommand(OPER, 0, 6));
    }
    else{
        comp_clause();
    }
}

void CSLEngine::comp_clause()
{
    expression();
    int last_symbol = __Symbol.first;
    getsym();
    expression();
    switch(last_symbol){
        case engine::SYMBOL_LIKE:
            __Code.push_back(OperationCommand(OPER, 0, 8));
            break;
        case engine::SYMBOL_GT:
            __Code.push_back(OperationCommand(OPER, 0, 12));
            break;
        case engine::SYMBOL_LT:
            __Code.push_back(OperationCommand(OPER, 0, 10));
            break;
        case engine::SYMBOL_GE:
            __Code.push_back(OperationCommand(OPER, 0, 11));
            break;
        case engine::SYMBOL_LE:
            __Code.push_back(OperationCommand(OPER, 0, 13));
            break;
        case engine::SYMBOL_EQ:
            __Code.push_back(OperationCommand(OPER, 0, 8));
            break;
        case engine::SYMBOL_EE:
            __Code.push_back(OperationCommand(OPER, 0, 8));
            break;
        case engine::SYMBOL_NOTE:
            __Code.push_back(OperationCommand(OPER, 0, 9));
            break;
    }
}

void CSLEngine::expression()
{
    //表达式可以-、(开头，单个的项
    if( __Symbol.first == engine::SYMBOL_SUB || __Symbol.first == engine::SYMBOL_PLUS ){
        //int last_symbol = __Symbol.first;
        getsym();
        term();
        if( __Symbol.first == engine::SYMBOL_SUB ){
            __Code.push_back(OperationCommand(OPER, 0, 1));
        }
    }
    else
        term();
   
    //项 +/- 项
    while( __Symbol.first == engine::SYMBOL_SUB || __Symbol.first == engine::SYMBOL_PLUS ){
        int last_symbol = __Symbol.first;
		getsym();
		term();
		if(last_symbol == engine::SYMBOL_PLUS)
			__Code.push_back(OperationCommand(OPER, 0, 2));
		else
			__Code.push_back(OperationCommand(OPER, 0, 3));
    }
}
    
void CSLEngine::term()
{
    if( __Symbol == SYMBOL_END )
        return;
    
    int last_symbol;
	
	factor();
	
	while( __Symbol.first == engine::SYMBOL_MULTI || __Symbol.first == engine::SYMBOL_DIV )
	{
		last_symbol= __Symbol.first;
		getsym();
		factor();
		if(last_symbol == engine::SYMBOL_MULTI)
			__Code.push_back(OperationCommand(OPER, 0, 4));
		else
			__Code.push_back(OperationCommand(OPER, 0, 5));
	}
}

void CSLEngine::factor()
{
    int i;
	while((__Symbol.first >= 0 && __Symbol.first <= SYMBOL_IDENT) || __Symbol.first == engine::SYMBOL_LBRACKET)
	{
		if( __Symbol.first == SYMBOL_IDENT ){
		    int index = __CdrType->index(__Symbol.second.c_str());
		    if( index != -1 ){
				__Code.push_back(OperationCommand(LOAD_VAR, 0, index));     //变量压栈
				getsym();
		        return;
		    }
		    else{                                                           //查看是不是程序内部变量
		        vector<CLocalVarStruct>::iterator itr;
                for(itr = __LocalVars.begin(); itr != __LocalVars.end(); itr++){
                    if( itr->name == __Symbol.second )
                        break;
                } 
                if( itr != __LocalVars.end() ){                             //找到了，表示是程序内部变量
                    index = itr - __LocalVars.begin();
                    __Code.push_back(OperationCommand(LOAD_LOCAL_VAR, 0, index));
				    getsym();
                }
                else{                                                       //内部也没找到，当函数使用
                    function();                                                                                                           
                }
		    }
			break;
		}
		else if(__Symbol.first == SYMBOL_INT){
		    __ConstVars.push_back(CVar(atoi(__Symbol.second.c_str())));           //整型常量压栈
		    __Code.push_back(OperationCommand(LOAD_CONST, 0, __ConstVars.size() - 1));
			getsym();
		}
		else if(__Symbol.first == SYMBOL_FLOAT){
			__ConstVars.push_back(CVar(atof(__Symbol.second.c_str())));
		    __Code.push_back(OperationCommand(LOAD_CONST, 0, __ConstVars.size() - 1));
			getsym();
		}
		else if(__Symbol.first == SYMBOL_STRING){
			__ConstVars.push_back(CVar(__Symbol.second.c_str()));
		    __Code.push_back(OperationCommand(LOAD_CONST, 0, __ConstVars.size() - 1));
			getsym();
		}
		else if(__Symbol.first == engine::SYMBOL_LBRACKET){
			getsym();
			expression();
			if (__Symbol.first == engine::SYMBOL_RBRACKET)
				getsym();
			else{
		        __ErrorMsgList.push_back("缺少右括号).");
		        return;
		    }
		}
	}
}

void CSLEngine::getsym()
{
    __Symbol = __Lex.getNextSymbol();
}

bool CSLEngine::execute(vector<CVar> & cdr)
{
    int StackTop = 0;                                   //栈顶
    
    OperationCommand * Oper;                            //操作指针
    
    int p = 0;                                          //当前程序代码的地址，其实是索引
    
    //int b = 1;
    
	do{
        Oper = & __Code[p];
        p += 1;
        
        switch( Oper->func ){
            case LOAD_VAR:{                             //将cdr里的值保存到栈顶
                StackTop += 1;
                __DataStack[StackTop] = cdr[Oper->op_switch];
                break;
            }
            case LOAD_LOCAL_VAR:{                       //将程序内部变量的值保存到栈顶
                StackTop += 1;
                __DataStack[StackTop] = __LocalVars[Oper->op_switch].value;
                break;
            }
            case LOAD_CONST:{                           //将常量加载到栈顶
                StackTop += 1;
                __DataStack[StackTop] = __ConstVars[Oper->op_switch];
                break;
            }
            case STOR:{                                 //将栈顶的内容保存到cdr中去
                cdr[Oper->op_switch] = __DataStack[StackTop];
                StackTop -= 1;
                break;
            }
            case STOR_LOCAL_VAR:{                       //将栈顶的内容保存到cdr中去
                __LocalVars[Oper->op_switch].value = __DataStack[StackTop];
                StackTop -= 1;
                break;
            }
            case CALL:{                                 //函数呼叫
                pFunctionHandle handle = __FunctionMap[Oper->op_switch].second;
                handle(StackTop, __DataStack);
                break;
            }
            case JUMP_Y:{                               //
				if((int)__DataStack[StackTop] != 0)
                    p = Oper->op_switch;
                break;
            }
            case JUMP:{
                p = Oper->op_switch;
                break;
            }
            case JUMP_N:{
                if((int)__DataStack[StackTop] == 0)
                    p = Oper->op_switch;
                break;
            }
            case OPER:{
                switch(Oper->op_switch){
                    case 0:                             //返回,return
			        	p = 0;
			        	__StackTopValue = &__DataStack[StackTop];
			        	break;
			        case 1:                             //前置-操作
			        	switch(__DataStack[StackTop].type()){
			        	    case VAR_TYPE_INT:{
			        	        __DataStack[StackTop] = -(int)__DataStack[StackTop];
			        	        break;
			                }
			        	    case VAR_TYPE_DOUBLE:{
			        	        __DataStack[StackTop] = -(double)__DataStack[StackTop];
			        	        break;
			        	    }
			            }
			        	break;
			        case 2:                             //加操作
			        	StackTop = StackTop - 1;
			        	switch(__DataStack[StackTop].type()){
			        	    case VAR_TYPE_INT:{
			        	        switch(__DataStack[StackTop+1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] += (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:       //将double数据转换为int再加
			        	                __DataStack[StackTop] += int((double)__DataStack[StackTop + 1]);
			        	                break;
			        	            case VAR_TYPE_STR:
			        	                __DataStack[StackTop] += (int)atoi(__DataStack[StackTop + 1].c_str());
			        	                break;
			        	            default:
			        	                throw "未知的数据类型，不能相加";
			        	        }
			        	        break;
			        	    }
			        	    case VAR_TYPE_STR:
			        	        __DataStack[StackTop] += __DataStack[StackTop + 1].c_str();
			        	        break;
			        	    case VAR_TYPE_DOUBLE:{
			        	        switch(__DataStack[StackTop+1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = double((double)__DataStack[StackTop] + (int)__DataStack[StackTop + 1]);
			        	                break;
			        	            case VAR_TYPE_DOUBLE:       //将double数据转换为int再加
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] + (double)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_STR:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] + (double)(atof(__DataStack[StackTop + 1].c_str()));
			        	                break;
			        	            default:
			        	                throw "未知的数据类型，不能相加";
			        	        }
			        	        break;
			        	    }
			            }
			        	break;
			        case 3:                             //减操作
			        	StackTop = StackTop - 1;        
			        	//__DataStack[StackTop] = (int)__DataStack[StackTop] - (int)__DataStack[StackTop + 1];
			        	switch(__DataStack[StackTop].type()){
			        	    case VAR_TYPE_INT:
			        	        switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] - (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] - (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有减操作";
			        	        }
			        	        break;
			        	    case VAR_TYPE_DOUBLE:
			        	        switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] - (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] - (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有减操作";
			        	        }
			        	        break;
			        	    default:
			        	        throw "只有整型与浮点数才有减操作";
			            }
			        	break;
			        case 4:                             //乘操作
			        	StackTop = StackTop - 1;
			        	switch(__DataStack[StackTop].type()){
			        	    case VAR_TYPE_INT:{
                                switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] * (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] * (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有乘操作";
			        	        }
			        	        break;
                            }
			        	    case VAR_TYPE_DOUBLE:
			        	        switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] * (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] * (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有乘操作";
			        	        }
			        	        break;
			        	    default:
			        	        throw "只有整型与浮点数才有乘操作";
			        	}
			        	break;
			        case 5:                             //除操作
			        	StackTop = StackTop - 1;
			        	switch(__DataStack[StackTop].type()){
			        	    case VAR_TYPE_INT:
			        	        switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] / (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (int)__DataStack[StackTop] / (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有乘操作";
			        	        }
			        	        break;
			        	    case VAR_TYPE_DOUBLE:
			        	        switch(__DataStack[StackTop + 1].type()){
			        	            case VAR_TYPE_INT:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] / (int)__DataStack[StackTop + 1];
			        	                break;
			        	            case VAR_TYPE_DOUBLE:
			        	                __DataStack[StackTop] = (double)__DataStack[StackTop] / (double)__DataStack[StackTop + 1];
			        	                break;
			        	            default:
			        	                throw "只有整型与浮点数才有乘操作";
			        	        }
			        	        break;
			        	    default:
			        	        throw "只有整型与浮点数才有除操作";
			        	}
			        	//__DataStack[StackTop] = (int)__DataStack[StackTop] / (int)__DataStack[StackTop + 1];
			        	break;
			        case 6:                             //取非操作
			        	if((int)__DataStack[StackTop] == 0)
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 8:                             //判断栈顶与次栈顶是否相等，结果保存到次栈顶，即==操作
			        	StackTop = StackTop - 1;
			        	if(__DataStack[StackTop] == __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 9:                             //判断栈顶与次栈顶是否相等，结果非值保存到次栈顶，即!=操作
			        	StackTop = StackTop - 1;
			        	if(__DataStack[StackTop] == __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 0;
			        	else
			        		__DataStack[StackTop] = 1;
			        	break;
			        case 10:                            //判断次栈顶是否小于栈顶内容，即<操作
			        	StackTop = StackTop - 1;
			        	if(__DataStack[StackTop] < __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 11:                            //判断次栈顶是否大于等于栈顶内容，即>=操作
			        	StackTop = StackTop - 1;
			        	if(__DataStack[StackTop] >= __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 12:                            //判断次栈顶是否大于栈顶内容，即>操作
			        	StackTop = StackTop - 1;
						if(__DataStack[StackTop] > __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 13:                            //判断次栈顶是否小于等于栈顶内容，即<=操作
			        	StackTop = StackTop - 1;
			        	if(__DataStack[StackTop] <= __DataStack[StackTop + 1])
			        		__DataStack[StackTop] = 1;
			        	else
			        		__DataStack[StackTop] = 0;
			        	break;
			        case 14:                            //与操作
			            StackTop = StackTop - 1;
			            __DataStack[StackTop] = (int)((int)__DataStack[StackTop]&(int)__DataStack[StackTop + 1]);
			            break;
			        case 15:                            //或操作
			            StackTop = StackTop - 1;
			            __DataStack[StackTop] = (int)((int)__DataStack[StackTop]|(int)__DataStack[StackTop + 1]);
			            break;
                }
            }
            break;
        }
    } while( p != 0 );
    return true;
}

