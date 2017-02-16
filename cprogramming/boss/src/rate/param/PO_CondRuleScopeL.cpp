#include "PO_CondRuleScopeL.h"


bool PO_CondRuleScopeL::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_CondRuleScopeL::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_yesOrNo = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	
	//added by hany at 2011/3/31 16:28:46
	strncpy(m_record.m_judgeValue, tmpstr.c_str(),MIN_STR_LENGTH );
	
	//Modified By Sunam 2009/8/14 9:27:40
	//m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str());
	//第一位字符用来表示JUDGE_OBJECT_TYPE，取值如下：
	//@:直接数，表示JUDGE_OBJECT_ID为一个实际数值与min_value,max_value,enum_value进行比较
	if (tmpstr[0] == '@')
	{
		m_record.m_judgeObjType = '5';
		m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str()+1);
	}
	//?:参数ID，表示JUDGE_OBJECT_ID为一个参数ID，其取值由资费实例确定。
	else if (tmpstr[0] == '?')
	{
		m_record.m_judgeObjType = '6';
		m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str()+1);
	}
	//#:参数ID，表示JUDGE_OBJECT_ID为一个用户属性参数ID，其取值由用户属性参数确定。
	else if (tmpstr[0] == '#')
	{
		m_record.m_judgeObjType = '7';
		m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str()+1);
	}
	//added by hany at 2011/3/18 18:46:58
	else if (tmpstr[0] == 'H')
	{
		m_record.m_judgeObjType = '8';
		m_record.m_judgeObj = 0;
	}
	//*:对象，含义与原来相同。默认为对象
	else
	{
		m_record.m_judgeObjType = '1';
		if (tmpstr[0] == '*')
		{
			m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str()+1);
		}
		else
		{
			m_record.m_judgeObj = (ObjectBase *)atoi(tmpstr.c_str());
		}
	}

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_diviedTag = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_judgeMode = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_valueMethed = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_valueType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_valueParam, tmpstr.c_str(),MIN_STR_LENGTH );
	m_record.m_valueParam[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_minValue, tmpstr.c_str(),MIN_STR_LENGTH );
	m_record.m_minValue[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_maxValue, tmpstr.c_str(),MIN_STR_LENGTH );	
	m_record.m_maxValue[MIN_STR_LENGTH] = '\0';
	
	m_record.m_minObj = 0;
	m_record.m_maxObj = 0;
	m_record.m_condType = '0';
			
	return true;
}


PO_CondRuleScopeL::PO_CondRuleScopeL(const PO_CondRuleScopeL& right)
{
	*this = right;
}


void PO_CondRuleScopeL::operator = (const PO_CondRuleScopeL& right)
{
	m_record = right.m_record;
}


int operator == (const PO_CondRuleScopeL &left,const PO_CondRuleScopeL& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_CondRuleScopeL &left,const PO_CondRuleScopeL& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_CondRuleScopeL& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_CondRuleScopeL::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_CondRuleScopeL","","","",
    "SELECT COND_ID,YES_OR_NO,JUDGE_OBJECT_ID,DIVIED_TAG,JUDGE_METHOD,"
    "VALUE_METHOD,VALUE_TYPE,VALUE_PARAM,MIN_VALUE,MAX_VALUE "
    "FROM TD_B_SIMPLECOND WHERE JUDGE_METHOD = '2'",
    10,"SELECT count(*) FROM TD_B_SIMPLECOND WHERE JUDGE_METHOD = '2'"};
