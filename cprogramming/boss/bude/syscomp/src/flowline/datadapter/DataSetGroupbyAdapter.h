#ifndef _DATASEGROUPBYGADAPTER_H_
#define _DATASEGROUPBYGADAPTER_H_

#include "flowline/adapter/DataProcessorAdapter.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/flbase.h"
#include <string>
#include <vector>

namespace flowline{
namespace dataprocessor {
	
using namespace flowline::adapter;
using namespace flowline;



class DataSetGroupbyAdapter : public DataProcessorAdapter
{
	
public:
	DataSetGroupbyAdapter()
	{
	
	}
	
virtual ~DataSetGroupbyAdapter()
	{
	  
	}    
    
   enum TransType{
       NONE = 0,//没有字段变化       
       STR2INT, 
       STR2INT64,
       INT2INT64,
       INT642STR,
       STR2DOUBLE,
       INT2STR,
       DOUBLE2STR,
       INT2DOUBLE,
       DOUBLE2INT,
       ISINT,//10
       ISDOUBLE,//11
       ISINT64 //12
       
    //   JSCONVIT  //由js处理 不管转换
    };
    
    //得到转换类型
    TransType checkTransType(dataset::DataType srctype,dataset::DataType dsttype);
  
    //初始化得到目标DataSet的数据结构
    void init(FlowLineDao& dao);
    
    
    //groupby处理
    void process(Context & ctx, pair<string, PDataSet>& srcdata, map<string, PDataSet>& result);
       
    //完成处理
    virtual void finish(Context& ctx, bool isAllFinished, map<string, PDataSet>& result );
    
    //得到字段名数组
    void getFieldName(PDataSet data,vector<string>& fields);
    
    //TYPE
    void getFieldDataType(PDataSet data, vector< DataType >& datatype);
         
    
    //void  printVector( vector<dataset::Data>& datas);
      

// 得到对象数组转换映射表
    void getgroupbyScript(Element* element, vector< pair< int ,TransType> >& groupidx , PDataSet src);
    
private :
    vector< string > m_dstfld; //初始化就有了
    vector< DataType > m_dsttype;//初始化时候也有了
    vector< pair< int ,TransType> >m_groupbyidx;
    vector <int> groupsrc; 
    multimap<string, PDataSet> res;
    //PDataSet dstds ;
    vector <PDataSet> ASS;
};  
}
}
#endif 


