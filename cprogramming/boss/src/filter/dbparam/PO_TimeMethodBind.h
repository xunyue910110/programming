#ifndef PO_TIMEMETHODBIND_H_
#define PO_TIMEMETHODBIND_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_TimeMethodBind
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_TimeMethodBind(){}
	
    PO_TimeMethodBind(const PO_TimeMethodBind& right);
	
    void operator=(const PO_TimeMethodBind& right);
	
    friend int operator==(const PO_TimeMethodBind &left,
		const PO_TimeMethodBind& right);
	
    friend int operator<(const PO_TimeMethodBind &left,
		const PO_TimeMethodBind& right);
	
	friend ostream& operator<<(ostream& os,const PO_TimeMethodBind& po);
	
    struct
    {
		T_ATTRI_VAL m_sourceType; //数据源
		T_FLAG      m_method;     //0:判断时间有效范围精确到秒，正常情况; 1:不判时间有效范围; 2:判断时间有效范围精确到当月，3:判断时间有效范围到上个月（话单上个月月份 >= 资料结束月份）
		//add bph，增加客户资料的前后偏移量
		int         m_preoffset;  //前偏移量
		int         m_postoffset; //后偏移量
		//end of add bph，增加客户资料的前后偏移量
    }recordStruct;
	
	private:
		static PO_Property m_property;
};
#endif /* PO_TIMEMETHODBIND_H_ */