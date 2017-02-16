#ifndef PO_GprsApnniType_H_200601007_
#define PO_GprsApnniType_H_200601007_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_GprsApnniType
{
    public:
        bool getProperty(PO_Property &property);
        bool convert(const StringVector &argname);
        PO_GprsApnniType(){}
        PO_GprsApnniType(const PO_GprsApnniType& right);
        void operator=(const PO_GprsApnniType& right);
        friend int operator==(const PO_GprsApnniType &left,const PO_GprsApnniType& right);
        friend int operator<(const PO_GprsApnniType &left,const PO_GprsApnniType& right);
        friend ostream& operator<<(ostream& os,const PO_GprsApnniType& po);
        
        struct
        {
						T_GPRS_APNGROUP  m_apnGroup;    //APNNI分组
						T_GPRS_APNTYPE   m_apnType;     //APNNI大类
						int							 m_priority;    //优先级：批价顺序，大着优先
    		} m_record;

    private:
        static PO_Property m_property;
};



#endif //PO_GprsApnniType_H_200601007_
