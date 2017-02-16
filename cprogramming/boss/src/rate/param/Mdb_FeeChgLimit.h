#ifndef MDB_FEECHGLIMIT_H_INCLUDED_20081014_
#define MDB_FEECHGLIMIT_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

class FeeChgLimit
{
  public:
    int          m_limitId;       //限定标识
    int          m_orderNo;       //序列号 
    long         m_minValue;      //最小金额
    long         m_maxValue;      //最大金额
    char         m_limitType;     //限定方式
    long         m_limitValue;    //限定额度
    long         m_limitRatio;    //限定比例
    char         m_fillTag;       //补足标识
    
  public:
  	FeeChgLimit& operator=(const FeeChgLimit& right)
  	{
  		m_limitId	   = right.m_limitId;         
			m_orderNo	   = right.m_orderNo;     
			m_minValue	 = right.m_minValue;    
			m_maxValue	 = right.m_maxValue;    
			m_limitType  = right.m_limitType;   
			m_limitValue = right.m_limitValue;  
			m_limitRatio = right.m_limitRatio;  
			m_fillTag    = right.m_fillTag;     

			return *this;
  	}
  	
 	friend 	ostream& operator<<(ostream& os,const FeeChgLimit &po);
 	
  friend bool operator < (const FeeChgLimit &r_left,const FeeChgLimit &r_right);
    
};
class Mdb_FeeChgLimit
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_FeeChgLimit(){}

    Mdb_FeeChgLimit(const Mdb_FeeChgLimit& right);

    void operator=(const Mdb_FeeChgLimit& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_FeeChgLimit& po);
    FeeChgLimit m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_FEECHGLIMIT_H_INCLUDED_20081014_
