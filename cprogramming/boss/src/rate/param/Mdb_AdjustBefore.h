#ifndef MDB_ADJUSTBEFORE_H_INCLUDED_20081014_
#define MDB_ADJUSTBEFORE_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

//定义帐前调整信息. 
class AdjustBefore                                      
{
  public:
    long  	    m_adjustInsId;      //帐单标识  
    T_SERVER_ID m_id;               //三户标识
    char        m_idType;           //标识类型
    int         m_effectObjectId;   //作用对象标识
    char    		m_actMode;	        //作用方式    
    char    		m_effectMode;       //帐前调整方式  
    char    		m_addupMode;        //累计方式
    int         m_addupObjectId;    //累计对象标识	                        
    char    		m_effectValueType;  //作用值类型        
    long     		m_effectValue;      //作用值              
    int         m_referObjId;       //参考对象标识            
    long    		m_referFee;         //参考值              
    int     		m_startCycId;       //起始帐期标识        
    int     		m_endCycId;         //终止帐期标识
    long  			m_limitFee;         //限定金额	             
    char        m_recvTag;          //作废标志	
    int     		m_adjustReasonId;   //调整原因标识 
   
   AdjustBefore & operator=(const AdjustBefore& right)
   {
   		m_adjustInsId  			=	right.m_adjustInsId;          
			m_id                =	right.m_id;              
			m_idType            =	right.m_idType;          
			m_effectObjectId    =	right.m_effectObjectId;  
			m_actMode 	        =	right.m_actMode;	        
			m_effectMode        =	right.m_effectMode;      
			m_addupMode         =	right.m_addupMode;       
			m_addupObjectId     =	right.m_addupObjectId;   
			m_effectValueType   =	right.m_effectValueType; 
			m_effectValue       =	right.m_effectValue; 
			m_referObjId        = right.m_referObjId;    
			m_referFee          =	right.m_referFee;        
			m_startCycId        =	right.m_startCycId;      
			m_endCycId          =	right.m_endCycId;        
			m_limitFee          =	right.m_limitFee;        
			m_recvTag           =	right.m_recvTag;         
			m_adjustReasonId    =	right.m_adjustReasonId;  

   		return *this;
   }    
   
   friend bool operator < (const AdjustBefore& left,const AdjustBefore& right) 
   {
   		return 	 (left.m_adjustInsId < right.m_adjustInsId);
   }  
   
    friend bool operator == (const AdjustBefore& left,const AdjustBefore& right) 
   {
   		return 	 (left.m_adjustInsId == right.m_adjustInsId);
   }  
};

class Mdb_AdjustBefore
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_AdjustBefore(){}

    Mdb_AdjustBefore(const Mdb_AdjustBefore& right);

    void operator=(const Mdb_AdjustBefore& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_AdjustBefore& po);
    AdjustBefore m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_ADJUSTBEFORE_H_INCLUDED_20081014_
