#include "PO_VpnRelFeePolicy.h"

bool PO_VpnRelFeePolicy::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}

bool PO_VpnRelFeePolicy::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_vpnIdA = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_vpnIdB = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_feePolicyId = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_beginTime = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endTime = tmpstr;
	
	return true;
}

PO_VpnRelFeePolicy::PO_VpnRelFeePolicy(const PO_VpnRelFeePolicy& right)
{
	*this = right;
}

void PO_VpnRelFeePolicy::operator = (const PO_VpnRelFeePolicy& right)
{
	m_record = right.m_record;
}

int operator == (const PO_VpnRelFeePolicy &left,const PO_VpnRelFeePolicy& right)
{
    return (left.m_record.m_vpnIdA == right.m_record.m_vpnIdA);
}

int operator < (const PO_VpnRelFeePolicy &left,const PO_VpnRelFeePolicy& right)
{
    return (left.m_record.m_vpnIdA < right.m_record.m_vpnIdA);
}

ostream& operator << (ostream& os,const PO_VpnRelFeePolicy& po)
{
	os<< po.m_record.m_vpnIdA << " , " <<
	     po.m_record.m_vpnIdB << " , " <<
	     po.m_record.m_feePolicyId << " , " <<
	     po.m_record.m_beginTime << " , " <<
	     po.m_record.m_endTime;
	return os;
}

PO_Property PO_VpnRelFeePolicy::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_VpnRelFeePolicy","","","",
    "SELECT VPNID_A,VPNID_B,FEEPOLICY_ID,BEGIN_TIME,END_TIME FROM TD_B_VPNRELFEEPOLICY",
    5,"SELECT COUNT(*) FROM TD_B_VPNRELFEEPOLICY"};
