#include "PO_AreaCodeInterAsp.h"

bool PO_AreaCodeInterAsp::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AreaCodeInterAsp::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_aspCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	
	recordStruct.m_interhead.clear();
	StringUtil::split(tmpstr,";",recordStruct.m_interhead,true);

	return true;
}

PO_AreaCodeInterAsp::PO_AreaCodeInterAsp(const PO_AreaCodeInterAsp& right)
{
	*this=right;
}

void PO_AreaCodeInterAsp::operator=(const PO_AreaCodeInterAsp& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AreaCodeInterAsp &left,const PO_AreaCodeInterAsp &right)
{
	return left.recordStruct.m_aspCode==right.recordStruct.m_aspCode;
}

int operator<(const PO_AreaCodeInterAsp &left,const PO_AreaCodeInterAsp& right)
{
	return (left.recordStruct.m_aspCode<right.recordStruct.m_aspCode);
}

ostream& operator<<(ostream& os,const PO_AreaCodeInterAsp& po)
{
        bool isFirst = true;
        os<<po.recordStruct.m_aspCode<<" , "<<
                        po.recordStruct.m_beginTime<<" , "<<
                        po.recordStruct.m_endTime<<" , "<<
                        po.recordStruct.m_areaCode<<" , ";
        for(int i = 0; i < po.recordStruct.m_interhead.size(); i ++)
        {
                if (isFirst)
                {
                        os << po.recordStruct.m_interhead.at(i);
                        isFirst = false;
                }
                else
                {
                        os << ";" << po.recordStruct.m_interhead.at(i);
                }
        }
        return os;
}


PO_Property PO_AreaCodeInterAsp::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AreaCodeInterAsp",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select ASP_CODE, BEGIN_TIME, END_TIME, NATION_AREA_CODE, INTERHEAD from TD_IMSII",
		5,"select count(*) from TD_IMSII"};


