#ifndef PO_GprsSgsnAddr_H_200601017_
#define PO_GprsSgsnAddr_H_200601017_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_GprsSgsnAddr
{
    public:
        bool getProperty(PO_Property &property);
        
        bool convert(const StringVector &argname);
        
        PO_GprsSgsnAddr(){}
        
        PO_GprsSgsnAddr(const PO_GprsSgsnAddr& right);
        
        void operator=(const PO_GprsSgsnAddr& right);
        
        friend int operator==(const PO_GprsSgsnAddr &left,const PO_GprsSgsnAddr& right);
        
        friend int operator<(const PO_GprsSgsnAddr &left,const PO_GprsSgsnAddr& right);
        
        friend ostream& operator<<(ostream& os,const PO_GprsSgsnAddr& po);
        
        struct
        {
      		string          m_beginSgsn; 	  //起始SGSN地址16机制
      		string          m_endSgsn;			//结束SGSN地址16机制
      		string          m_beginSgsnOrg; //起始SGSN地址10机制点分法
      		string          m_endSgsnOrg;	  //结束SGSN地址10机制点分法
      		T_AREA_CODE     m_areaCode;     //归属区号
				  T_PROVINCE_CODE m_provCode;     //归属省代码
      		char            m_homeType;		  //归属类型
      		T_DATE_TIME     m_beginTime;    //生效时间
          T_DATE_TIME     m_endTime;      //失效时间
    		} m_record;
    private:
        static PO_Property m_property;
        static void decTohexAddr(const string &decAddr,string &hexAddr);
};

#endif //PO_GprsSgsnAddr_H_200601017_
