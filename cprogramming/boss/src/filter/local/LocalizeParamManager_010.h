#ifndef _LOCALIZEPARAMMANAGER_010_H_
#define _LOCALIZEPARAMMANAGER_010_H_

#include "PMHelperall.h"
#include "CdrProperty.h"
//////////////////////////////////////////////////////////////////////////
class PO_SpServCodeSpname
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_SpServCodeSpname(){}
	
    PO_SpServCodeSpname(const PO_SpServCodeSpname& right);
	
    void operator=(const PO_SpServCodeSpname& right);
	
    friend int operator==(const PO_SpServCodeSpname &left,
		const PO_SpServCodeSpname& right);
	
    friend int operator<(const PO_SpServCodeSpname &left,
		const PO_SpServCodeSpname& right);
	
	friend ostream& operator<<(ostream& os,const PO_SpServCodeSpname& po);
	
    SpProperty  recordStruct;
	
private:
	static PO_Property m_property;
};


//////////////////////////////////////////////////////////////////////////
class PO_ServiceCodeSpname
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_ServiceCodeSpname(){}
	
    PO_ServiceCodeSpname(const PO_ServiceCodeSpname& right);
	
    void operator=(const PO_ServiceCodeSpname& right);
	
    friend int operator==(const PO_ServiceCodeSpname &left,
		const PO_ServiceCodeSpname& right);
	
    friend int operator<(const PO_ServiceCodeSpname &left,
		const PO_ServiceCodeSpname& right);
	
	friend ostream& operator<<(ostream& os,const PO_ServiceCodeSpname& po);
	
    SpProperty  recordStruct;
	
private:
	static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_SpBillRule
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_SpBillRule(){}
	
    PO_SpBillRule(const PO_SpBillRule& right);
	
    void operator=(const PO_SpBillRule& right);
	
    friend int operator==(const PO_SpBillRule &left,
		const PO_SpBillRule& right);
	
    friend int operator<(const PO_SpBillRule &left,
		const PO_SpBillRule& right);
	
	friend ostream& operator<<(ostream& os,const PO_SpBillRule& po);
	
    struct
    {
		T_ATTRI_VAL   m_source;
		T_ATTRI_VAL   m_spcode;
		T_ATTRI_VAL   m_sp_srvtype;
		T_ATTRI_VAL   m_billflag;
		T_DATE_TIME   beginTime;
		T_DATE_TIME   endTime;
    }recordStruct;
	
	private:
		static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_RegularFixCalled
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_RegularFixCalled(){}
	
    PO_RegularFixCalled(const PO_RegularFixCalled& right);
	
    void operator=(const PO_RegularFixCalled& right);
	
    friend int operator==(const PO_RegularFixCalled &left,
		const PO_RegularFixCalled& right);
	
    friend int operator<(const PO_RegularFixCalled &left,
		const PO_RegularFixCalled& right);
	
	friend ostream& operator<<(ostream& os,const PO_RegularFixCalled& po);
	
    struct
    {
		T_ATTRI_VAL   m_msc;
		T_ATTRI_VAL   m_calledareacode;
		T_ATTRI_VAL   m_calledprefix;
		T_DATE_TIME   beginTime;
		T_DATE_TIME   endTime;
    }recordStruct;
	
	private:
		static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_NmFixTrashPtp
{
public:
    bool getProperty(PO_Property &property);
	
    bool convert(const StringVector &argname);
	
    PO_NmFixTrashPtp(){}
	
    PO_NmFixTrashPtp(const PO_NmFixTrashPtp& right);
	
    void operator=(const PO_NmFixTrashPtp& right);
	
    friend int operator==(const PO_NmFixTrashPtp &left,
		const PO_NmFixTrashPtp& right);
	
    friend int operator<(const PO_NmFixTrashPtp &left,
		const PO_NmFixTrashPtp& right);
	
	friend ostream& operator<<(ostream& os,const PO_NmFixTrashPtp& po);
	
    struct
    {
		T_ATTRI_VAL   m_msc;
		T_ATTRI_VAL   m_phone;
		int           m_groupId;
		T_DATE_TIME   beginTime;
		T_DATE_TIME   endTime;
    }recordStruct;
	
	private:
		static PO_Property m_property;
};


// Begin add by xiezq 20101207 for 集团BSSV4.1 一卡双号需求
// 一卡双号MSISDN
class PO_MsisdnHk
{
public:
    bool getProperty(PO_Property& property);

    bool convert(const StringVector& argname);

    PO_MsisdnHk(){}

    PO_MsisdnHk(const PO_MsisdnHk& rhs);

    void operator=(const PO_MsisdnHk& rhs);

    friend int operator==(const PO_MsisdnHk& lhs, const PO_MsisdnHk& rhs);

    friend int operator<(const PO_MsisdnHk& lhs, const PO_MsisdnHk& rhs);

	friend ostream& operator<<(ostream& os, const PO_MsisdnHk& po);
	
    struct
    {
    	T_ATTRI_VAL begin_phone;
    	T_ATTRI_VAL end_phone;
    	T_ATTRI_VAL usertype;
	    T_DATE_TIME beginDate;
	    T_DATE_TIME endDate;
    }recordStruct;
    
private:
	static PO_Property m_property;
};

// 一卡双号IMSI
class PO_ImsiHk
{
public:
    bool getProperty(PO_Property& property);
    
    bool convert(const StringVector& argname);
    
    PO_ImsiHk(){}
    
    PO_ImsiHk(const PO_ImsiHk& rhs);
    
    void operator=(const PO_ImsiHk& rhs);
    
    friend int operator==(const PO_ImsiHk& lhs, const PO_ImsiHk& rhs);
	
    friend int operator<(const PO_ImsiHk& lhs, const PO_ImsiHk& rhs); 
	
	friend ostream& operator<<(ostream& os, const PO_ImsiHk& po);
	
    struct
    {
    	T_ATTRI_VAL begin_imsi;
    	T_ATTRI_VAL end_imsi;
    	T_ATTRI_VAL usertype;
	    T_DATE_TIME beginDate;
	    T_DATE_TIME endDate;
    }recordStruct;
    
private:
	static PO_Property m_property;
};
// End add by xiezq 20101207 for 集团BSSV4.1 一卡双号需求


#endif /* _LOCALIZEPARAMMANAGER_010_H_ */
