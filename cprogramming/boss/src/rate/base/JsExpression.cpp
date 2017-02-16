#include "JsExpression.h"
#include <algorithm>

bool operator < (const Variable &left,const Variable &right)
{
    if (left.m_varType < right.m_varType)
    {
        return true;
    }
    if (left.m_varType > right.m_varType)
    {
        return false;
    }
    return left.m_varAttrid < right.m_varAttrid;
}

bool operator == (const Variable &left,const Variable &right)
{
    return ((left.m_varType == right.m_varType) && (left.m_varAttrid == right.m_varAttrid));
}       

void JsExpression::dump()
{
	cout<< " jsStatement : " << m_jsStatement << endl;
	for (int i=0; i < m_variables.size(); ++i)
	{
	    cout<< " Variable[" << i << "] : " << m_variables[i].m_varName << " , " << m_variables[i].m_varAttrid << " , " << m_variables[i].m_varType << endl;
	}
}

bool JsExpression::parseExpr(const string &r_expr)
{
    size_t bPos,ePos=0;
    string des,key;
    string expr = r_expr;
    int position=0;
    Variable var;
    char varName[4];
    vector<Variable>::iterator itr;

    m_variables.clear();
    //替换参数
    while(true)
    {
    	//查找第一个?
    	bPos = expr.find_first_of("?");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 1;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
    	    var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "inBuf.getValue('";
        des += varName;
        des += "')";
    }
    expr = des;
    des = "";
    //替换对象
    while(true)
    {
    	//查找第一个$
    	bPos = expr.find_first_of("$");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 2;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
            var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "inBuf.getValue('";
        des += varName;
        des += "')";
    }
    expr = des;
    des = "";
    //替换用户属性
    while(true)
    {
    	//查找第一个#
    	bPos = expr.find_first_of("#");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 3;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
            var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "inBuf.getValue('";
        des += varName;
        des += "')";
    }
    //Added By Sunam 2009/11/6 21:44:40 目前最多充许使用10个变量
    if (position >= 10)
    {
        return false;
    }
    m_jsStatement = "var result = " + des;
    m_jsStatement += "; outBuf.pushValue('result',result);";
    return true;
}

//added by hany at 2011/3/31 16:45:46
bool JsExpression::ParseSpecialExpr(const string &r_expr)
{
    size_t bPos,ePos,gPos=0;
    string des,key,offset;
    string expr = r_expr;
    int position=0;
    Variable var;
    char varName[4];
    vector<Variable>::iterator itr;

    m_variables.clear();
    //替换参数
    while(true)
    {
    	//查找第一个?
    	bPos = expr.find_first_of("?");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 1;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
    	    var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "inBuf.getValue('";
        des += varName;
        des += "')";
    }
    expr = des;
    des = "";
    //替换对象
    while(true)
    {
    	//查找第一个$
    	bPos = expr.find_first_of("$");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
      {
    	  key = expr.substr(bPos+1);
    	  offset = "0";
    	  //删除从开始到ePos
    	  expr = "";
      }
      else
      {
      	if(expr[ePos] == '~')
      	{
      		key = expr.substr(bPos+1,ePos-bPos-1);
      		gPos = expr.find_first_not_of("0123456789",ePos+2);
      		if (gPos == string::npos)
    	    {
    	      offset = expr.substr(ePos+1,gPos-ePos-1);
    	      expr = "";
    	    }
    	    else
    	    {
    	      offset = expr.substr(ePos+1,gPos-ePos-1);
    	      expr.erase(0,gPos);
    	    }
      	}
      	else
      	{
      		key = expr.substr(bPos+1,ePos-bPos-1);
      		offset = "0";
      	}
      }
    	var.m_varAttrid = atol(key.c_str());
      var.m_varType = 2;
      var.m_offsetValue = atol(offset.c_str());
      itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
      memset(varName,0,4);
      if (itr == m_variables.end() || !(*itr == var))
      {
    	  sprintf(varName,"V%d",position++);
          var.m_varName = varName;
          m_variables.insert(itr,var);
      }
      else
      {
          sprintf(varName,"%s",itr->m_varName.c_str());
      }
      des += "inBuf.getValue('";
      des += varName;
      des += "')";
    }
    expr = des;
    des = "";
    //替换用户属性
    while(true)
    {
    	//查找第一个#
    	bPos = expr.find_first_of("#");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 3;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
            var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "inBuf.getValue('";
        des += varName;
        des += "')";
    }
    //Added By Sunam 2009/11/6 21:44:40 目前最多充许使用10个变量
    if (position >= 10)
    {
        return false;
    }
    m_jsStatement = "var result = " + des;
    m_jsStatement += "; outBuf.pushValue('result',result);";
    return true;	
}

bool JsExpression::parseJScript(const string &r_expr)
{
    size_t bPos,ePos=0;
    string des,key;
    string expr = r_expr;
    int position=0;
    Variable var;
    char varName[4];
    vector<Variable>::iterator itr;

    m_variables.clear();
    //替换参数
    while(true)
    {
    	//查找第一个?
    	bPos = expr.find_first_of("?");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 1;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
    	    var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "'";
        des += varName;
        des += "'";
    }
    expr = des;
    des = "";
    //替换对象
    while(true)
    {
    	//查找第一个$
    	bPos = expr.find_first_of("$");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 2;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
            var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "'";
        des += varName;
        des += "'";
    }
    expr = des;
    des = "";
    //替换用户属性
    while(true)
    {
    	//查找第一个#
    	bPos = expr.find_first_of("#");
    	//未找到结束
    	if (bPos == string::npos)
    	{
    		des += expr;
    		break;
    	}
    	//第一个字符至第一个$原样保留
    	des += expr.substr(0,bPos);
    	//从bPos开始查找第一个不是数字
    	ePos = expr.find_first_not_of("0123456789",bPos+1);
    	//叁数
    	if (ePos == string::npos)
        {
    	    key = expr.substr(bPos+1);
    	    //删除从开始到ePos
    	    expr = "";
        }
        else
        {
            key = expr.substr(bPos+1,ePos-bPos-1);
            //删除从开始到ePos
    	    expr.erase(0,ePos);
        }
    	var.m_varAttrid = atol(key.c_str());
        var.m_varType = 3;
        itr = ::lower_bound(m_variables.begin(),m_variables.end(),var);
        memset(varName,0,4);
        if (itr == m_variables.end() || !(*itr == var))
        {
    	    sprintf(varName,"V%d",position++);
            var.m_varName = varName;
            m_variables.insert(itr,var);
        }
        else
        {
            sprintf(varName,"%s",itr->m_varName.c_str());
        }
        des += "'";
        des += varName;
        des += "'";
    }
    m_jsStatement = des;
    //Added By Sunam 2009/11/6 21:44:40 目前最多充许使用10个变量
    if (position >= 10)
    {
        return false;
    }
    return true;
}

//int main(int argc, char *argv[])
//{
//  Expression expr;
//  cout << " Expr = ?5-100*?1*?2+?3+?4/33-55+$2000 " << endl;
//  expr.parseExpr("?5-100*?1*?2+?3+?4/33-55+$2000");
//  expr.dump();
//  return 0;
//}
