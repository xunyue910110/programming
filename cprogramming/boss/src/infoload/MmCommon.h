#ifndef _MmCommon_H_
#define _MmCommon_H_


#include "config-all.h"
#include "CException.h"
#include "base/Types.h"
#include "mdb/MdbConstDef.h"
#include "base/StringUtil.h"


USING_NAMESPACE(std)


//(1.1)数据库相关常量属性定义
//数据库字符串值的最大长度
const unsigned int  MAX_MMSTRVALUE_LEN = 4096+1;
//数据库表列名称的最大长度
const unsigned int	MAX_MMCOLUMNNAME_LEN = 50;


/**
* MmParam:内存数据库的参数结构。
*/
struct MmParam
{
	//该字段是否是对应的索引构成字段 0:不是 1:是
	unsigned int m_indextag;
	//字段值类型:VAR_TYPE_INT2,VAR_TYPE_INT,VAR_TYPE_DATE,VAR_TYPE_LONG,VAR_TYPE_REAL,VAR_TYPE_NUMSTR(无用),VAR_TYPE_VSTR,VAR_TYPE_UNKNOW(无用)
	COLUMN_VAR_TYPE	m_type;
	//字段值
	union			
	{
		short	m_shortValue;
		int		m_intValue;
		long	m_longValue;
		float	m_floatValue;
		char	m_dateValue[15];
		char	m_strValue[MAX_MMSTRVALUE_LEN];
	};
};


#endif

