#include "PO_Item.h"


bool PO_Item::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_Item::convert(const StringVector &argname)
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
	m_record.m_itemType = tmpstr[0];	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	
	m_record.m_reverseItemVector.clear();
	if(strlen(tmpstr.c_str())>0)
	{
		char *p = strtok( (char *) tmpstr.c_str(), ",");
		int reverseItem;
		while(p!=NULL)
		{
			reverseItem = atoi(p);
			if (reverseItem!=0)
				m_record.m_reverseItemVector.push_back(reverseItem);
			p = strtok(NULL,",");
		}
	}
		
	return true;
}


PO_Item::PO_Item(const PO_Item &right)
{
	*this = right;
}


void PO_Item::operator = (const PO_Item &right)
{
	m_record = right.m_record;
}


int operator == (const PO_Item &left,const PO_Item& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_Item &left,const PO_Item& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_Item& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_Item::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_Item","","","",
    "SELECT ITEM_ID,ITEM_TYPE,DEL_ITEM_SET FROM TD_B_ITEM",3,
    "SELECT count(*) FROM TD_B_ITEM"};
