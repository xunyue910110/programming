#ifndef _EXPRESSION_H_INCLUDE_20090813_
#define _EXPRESSION_H_INCLUDE_20090813_

#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <stack>
#include <queue>
#include "config-all.h"
#include "base/exceptions.h"

template <typename T>struct Type{static bool isPrefix(char ch){return 0;}};

template <> struct Type<double>{
    static bool isPrefix(char ch){
        return ch=='.' || isdigit(ch);
    }
};

template <> struct Type<int>{
    static bool isPrefix(char ch){
        return isdigit(ch);
    }
};

template <class T>
class Expression{
    string instr;
    queue<T> Q;
public:
    //用字符串str初始化表达式
    Expression(string str){instr=str;}

    //返回表达式的值
    T value(){return eval(postfix(instr));}

    static int prcd(char c1,char c2)
    {//当c1运算优先级高于c2时返回真,否则返回假
        if(c1=='^' &&c2=='^')return 0;
        if(c1=='(')return 0;
        if(c2==')')return 1;
        if(c2=='(')return 0;
        static char *str[]={"+-","*/%","^",NULL};
        int i1=0,i2=0;
        for(int i=0;str[i];i++){
            for(int j=0;str[i][j];j++){
                if(str[i][j]==c1)i1=i;
                if(str[i][j]==c2)i2=i;
            }
        }
        return (i1>=i2);//左结合时取等号，如果右结合将等号去掉即可
    }

    string postfix(string instr)
    {//返回中缀表达式instr的后缀表达式，其中的操作数用o代替，并存入队列Q 
        istringstream iss(instr);
        ostringstream oss;
        stack<char> S;
        for (char ch;iss>>ch;){
            if(Type<T>::isPrefix(ch)){
                iss.unget();
                T s;
                iss>>s;
                oss<<'o';
                Q.push(s);
            }
            else {
                while(!S.empty() && prcd(S.top(),ch)){
                    oss<<S.top();
                    S.pop();
                }
                if(ch!=')') S.push(ch);
                else S.pop();
            }
        }
        while(!S.empty()){
            oss<<S.top();
            S.pop();
        }
        return oss.str();
    }

    static T oper(char symb,T& ta,const T& tb)
    {
    	long ta1 = ta;
        switch(symb){
        case '+':return ta+=tb;
        case '-':return ta-=tb;
        case '*':return ta*=tb;
        case '/':return ta/=tb;
        case '^':return ta=(T)pow(ta,(double)tb);
        case '%':
        	ta1%=(long)tb;
        	return ta=ta1;
        default:
            THROW_C_P1(BusinessException, -1, "未定义的运算符:%c",symb);
            return -1;
        }
    }

    T eval(string poststr)
    {//返回后缀表达式poststr的值
        istringstream iss(poststr);
        stack<T> S;
        for (char ch;iss>>ch;){
            if(ch=='o'){
                T v=Q.front();
                Q.pop();
                S.push(v);
            }
            else{
                T tb=S.top();
                S.pop();
                oper(ch,S.top(),tb);
            }
        }
        T t=S.top();
        S.pop();
        return t;
    }
};

//int main()
//{
//    string e1 = "1+234+5*(3+4)";
//    cout<<e1<<" = "<<Expression<int>(e1).value()<<endl;
//
//    string e2 = "1+2.34+5*(3+4)"; 
//    cout<<e2<<" = "<<Expression<double>(e2).value()<<endl;   
//}

#endif
