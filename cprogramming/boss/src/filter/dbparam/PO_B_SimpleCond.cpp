#include "PO_B_SimpleCond.h"

bool PO_B_SimpleCond::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_B_SimpleCond::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_condId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_yesOrNo=tmpstr[0];
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_judgeObjectId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_judgeMethod=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_valueType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_valueMethod=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	strncpy(recordStruct.m_valueParam, tmpstr.c_str(),MIN_STR_LENGTH );
	recordStruct.m_valueParam[MIN_STR_LENGTH] = '\0';

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_minValue=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_maxValue=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	
	recordStruct.m_enumValue.clear();
	StringUtil::split(tmpstr,",",recordStruct.m_enumValue,true);

	return true;
}

PO_B_SimpleCond::PO_B_SimpleCond(const PO_B_SimpleCond& right)
{
	*this=right;
}

void PO_B_SimpleCond::operator=(const PO_B_SimpleCond& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_B_SimpleCond &left,const PO_B_SimpleCond& right)
{
	return left.recordStruct.m_condId==right.recordStruct.m_condId;
}

int operator<(const PO_B_SimpleCond &left,const PO_B_SimpleCond& right)
{
	return (left.recordStruct.m_condId < right.recordStruct.m_condId);
}

ostream& operator<<(ostream& os,const PO_B_SimpleCond& po)
{
	bool isFirst = true;
	os<<po.recordStruct.m_condId<<" , "<<
			po.recordStruct.m_yesOrNo<<" , "<<
			po.recordStruct.m_judgeObjectId<<" , "<<
			po.recordStruct.m_judgeMethod<<" , "<<
			po.recordStruct.m_valueType<<" , "<<
			po.recordStruct.m_valueParam<<" , "<<
			po.recordStruct.m_minValue<<" , "<<
			po.recordStruct.m_maxValue<<" , ";
			for(int i = 0; i < po.recordStruct.m_enumValue.size(); i ++)
        {
             if (isFirst)
                {
                        os << po.recordStruct.m_enumValue.at(i);
                        isFirst = false;
                }
                else
                {
                        os << ";" << po.recordStruct.m_enumValue.at(i);
                }    
        }
	return os;
}


PO_Property PO_B_SimpleCond::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_B_SimpleCond",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select COND_ID, YES_OR_NO, JUDGE_OBJECT_ID, JUDGE_METHOD, VALUE_TYPE, VALUE_METHOD, VALUE_PARAM, MIN_VALUE, MAX_VALUE, ENUM_VALUE from TD_B_SIMPLECOND",
		10,"select count(*) from TD_B_SIMPLECOND"};

