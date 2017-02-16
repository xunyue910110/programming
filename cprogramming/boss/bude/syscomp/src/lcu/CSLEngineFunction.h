#ifndef CSLENGINEFUNCTION_HEADER_INCLUDED
#define CSLENGINEFUNCTION_HEADER_INCLUDED

#include <string.h>
#include <CDataList.h>
#include <CPair.h>
//#include <CStringFunc.h>
#include <CDateTime.h>
using std::min;
using std::cout;
using std::endl;
using std::stringstream;
using std::fstream;
	
//外部引用为substr(org_msisdn, 0, 2)
inline void engine_substr(int & t, CDataList & data_stack)
{
    int len = (int)data_stack[t];               //取要截取的长度
    int pos = (int)data_stack[t-1];             //取要截取的起始
    const char * src = data_stack[t - 2].c_str();       //取要截取的字符串
    char tmp[1024];
    if( len == 0 ){
        strcpy(tmp, src + pos);
    }
    else{
        //! 这里做安全转换，即要校验长度
        int src_len = strlen(src);
        if(src_len <= pos){
            tmp[0] = '\0';
        }
        else{
            int copy_len = min(len, (int)strlen(src +pos));
            memcpy(tmp, src + pos, copy_len);
            tmp[copy_len] = '\0';
        }
    }
    data_stack[t - 2] = tmp;
    t = t - 2;
}

inline void engine_strlen(int & t, CDataList & data_stack)
{
    data_stack[t] = (int)strlen(data_stack[t].c_str());
}

//外部引用为atoi(call_duration)
inline void engine_atoi(int & t, CDataList & data_stack)
{
    data_stack[t] = (int)atoi(data_stack[t].c_str());
}
//外部引用为atoi(call_duration)
inline void engine_atol(int & t, CDataList & data_stack)
{
    data_stack[t] = (int)atol(data_stack[t].c_str());
}
//外部引用为atof(call_duration)
inline void engine_atof(int & t, CDataList & data_stack)
{
    data_stack[t] = (double)atof(data_stack[t].c_str());
}

//外部引用为sysdate(format)
inline void engine_sysdatetime(int & t, CDataList & data_stack)
{   
    data_stack[t] = idt::now(data_stack[t].c_str());
    printf("[LCU]sysdatetime:%s",data_stack[t].c_str());
}
//外部引用为trim(org_msisdn)
inline void engine_trim(int & t, CDataList & data_stack)
   {   //  by keiven
   //char tmp[1024];
   //strcpy(tmp, data_stack[t].c_str());
   //isf::trim(tmp);      
   //data_stack[t] = tmp;
   
   char tmp[1024];
   strcpy(tmp, data_stack[t].c_str());
   char* String = tmp;
   char *Position = String;
   /*找到第一个不是空格的位置*/
   while(isspace(*Position)){
   	Position++;
   }
   /*如果为一空串则退出*/
   if (*Position == '\0')	{
   	*String = '\0';
   	return;
   }
   /*除去前面的空格*/
   while(*Position)	{
   	*String = *Position;
   	String++;
   	Position++;
   }
   /*除去后面的空格*/
   do	{
   	*String = '\0';
   	String -- ;
   } while(isspace(*String)); 
   data_stack[t] = tmp;  
}

//外部引用为datetimeadd(s_date, s_time, l_seconds)
inline void engine_datetimeadd(int & t, CDataList & data_stack)
{//  by keiven
    idt::datetimeadd(data_stack[t-2].c_str(), data_stack[t-1].c_str(),
       (int)data_stack[t]);
    data_stack[t - 2] = idt::getDateTime();
    t = t - 2;
}

//外部引用为datetimediff(datetime1, datetime2)
inline void engine_datetimediff(int & t, CDataList & data_stack)
{   //  by keiven
    data_stack[t-1] = (int)idt::datetimediff(data_stack[t].c_str(), data_stack[t-1].c_str());
    t = t - 1;
}
//////////////////////////////////////////////////过程体函数
//外部引用为call print(org_msisdn)
inline void engine_print(int & t, CDataList & data_stack)
{
    cout << data_stack[t];
    t--;
}

inline void engine_upper(int & t, CDataList & data_stack)
{
    char tmp[1024];
    strcpy(tmp, data_stack[t].c_str());
    char * p = tmp;
    while(*p){
        if( *p >= 'a' && *p <= 'z' ){
            *p = *p - ('a' - 'A'); 
        }
        p++;
    }
    data_stack[t] = tmp;
}

inline void engine_lower(int & t, CDataList & data_stack)
{
    char tmp[1024];
    strcpy(tmp, data_stack[t].c_str());
    char * p = tmp;
    while(*p){
        if( *p >= 'A' && *p <= 'Z' ){
            *p = *p + ('a' - 'A'); 
        }
        p++;
    }
    data_stack[t] = tmp;
}

#endif /* CSLENGINEFUNCTION_HEADER_INCLUDED */
