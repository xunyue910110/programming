
#ifndef CSEXORESSION_HEADER_INCLUDED
#define CSEXORESSION_HEADER_INCLUDED
                                    
#include <string>
#include <string.h>
#include "CVar.h"
#include "CSLEngine.h"
#include "CException.h"

/**
*对表达式就解封装 
* 
*/
class CSExpression
{
private:
   string result ;   //返回值字符串型
   string errorMessage ; //ERROR Message
   bool isError;  // 是否有异常  
protected:   
   const CVar * returnedCVar;  //返回值对象
public:
  
  CSExpression()
  {
    isError = false;
    // result = "";
    // errorMessage = NULL;    
  };
  ~CSExpression(){
    //if(returnedCVar!=0)
    //delete returnedCVar;    
  };
  
  const char * getResult(){
     return result.c_str();
  };
  const char * getErrorMessage(){
     errorMessage += " 表达式计算错误" ;
     return errorMessage.c_str();
  };
  bool isException(){
    return isError;  
  };
 /**
 * ExpressionCompile
 * @param stat_rule 公式 
 * @param isLogicExpression 是逻辑表达式吗
 */
  bool ExpressionCompile(const char * stat_rule, const bool isLogicExpression = true)
  {
     CSLEngine    formula;//公式计算 
     vector<CVar>  cdr;
     try{
          if(isLogicExpression){//默认为逻辑计算 
               formula.LogicExpressionCompile(stat_rule);
          }
          else{
               formula.MathExpressionCompile(stat_rule);
          }

          formula.execute(cdr);//执行 
          const CVar & rtn = formula.getExecuteResult();
          returnedCVar = & rtn;

          switch(rtn.type()){
            case VAR_TYPE_STR:{
                result = rtn.str();
                break;
            }
            case VAR_TYPE_INT:{
                //char tmp[16];
                //sprintf(tmp, "%d", (int)rtn);]
                //result = string(tmp);
                result = rtn.str();                
                break;
            }
            case VAR_TYPE_DOUBLE:{
                //char tmp[16];
                //sprintf(tmp, "%f", (double)rtn);
                //result = string(tmp);
                result = rtn.str();      
                break;
            }
           }
       }
       catch(common_exception &e){
              errorMessage = e.what().c_str();
              printf("error:%s",e.what().c_str());
              isError = true;
              //return false;
       }catch(exception &e ) {
              errorMessage = e.what();
              printf("error:%s",e.what());
              isError = true;
              //return false;
       } catch(...) {
              errorMessage = "未知异常";
              printf("error:%s","未知异常");
              isError = true;
              //return false;
       }
       isError = false;
       return !isError;    
  };   
};

#endif
