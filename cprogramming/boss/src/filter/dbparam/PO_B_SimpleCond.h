#ifndef _PO_B_SIMPLECOND_H_
#define _PO_B_SIMPLECOND_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
#include "base/StringUtil.h"

const int JUDGE_RANGE        = 0; //()
const int JUDGE_RRANGE       = 1; //(]
const int JUDGE_LRANGE       = 2; //[)
const int JUDGE_DRANGE       = 3; //[]
const int JUDGE_IN           = 4; //IN

const int MAX_STR_LENGTH     = 200;//最大参数长度
const int MIN_STR_LENGTH     = 50; //最小参数长度


class PO_B_SimpleCond
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_B_SimpleCond(){}

    PO_B_SimpleCond(const PO_B_SimpleCond& right);

    void operator=(const PO_B_SimpleCond& right);

    friend int operator==(const PO_B_SimpleCond& left,
    		const PO_B_SimpleCond& right);

    friend int operator<(const PO_B_SimpleCond& left,
    		const PO_B_SimpleCond& right);

	friend ostream& operator<<(ostream& os,const PO_B_SimpleCond& po);

    struct
    {
    	int					  	 m_condId;         //条件标识ID
    	T_FLAG  		  	 m_yesOrNo;        //取反标志 0:正常 1:取反
    	int   					 m_judgeObjectId;  //判断对象标识
    	T_FLAG   				 m_judgeMethod;    //判断方式 0:() 1:(] 2:[) 3:[] 4:IN
    	T_FLAG           m_valueType;      //数值类型 0:数值 1:字符
    	T_ATTRI_VAL   	 m_minValue;       //最小基准值
    	T_ATTRI_VAL      m_maxValue;       //最大基准值
    	vector<string>   m_enumValue;      //枚举基准值
    	T_FLAG           m_valueMethod;    //取值方法 0:普通 1:取模 2:SUBSTR 3:STRCAT
    	char  m_valueParam[MIN_STR_LENGTH+1];    //取值参数
    	
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* _PO_B_SIMPLECOND_H_ */
