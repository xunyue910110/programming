#ifndef _DATASETMAPPINGADAPTER_H_
#define _DATASETMAPPINGADAPTER_H_

#include "flowline/adapter/DataProcessorAdapter.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/scripts.h"
#include "flowline/flbase.h"
#include <string>
#include <vector>


namespace flowline{
namespace dataprocessor {
	
using namespace flowline::adapter;
using namespace flowline;

//#define _MAPPING_ADAPTER_REPOSITORY_DEFINITION_ID_  "repositoryDefinitionId" 


class DataSetMappingAdapter : public DataProcessorAdapter
{
	
public:
	DataSetMappingAdapter(){
		//skipReferenceFalt = false;
	}
	
	virtual ~DataSetMappingAdapter(){
//	     if(m_rowjs.size() > 0){
//			this->releaseRowJavaScript(m_rowjs);
//		}
	}    
    
    //字段转换类型    
    enum ConvitType{
       NONE = 0,//没有字段变化       
       STR2INT,
       STR2INT64, 
       INT2INT64, //INT64不能转成INT32
       INT642STR,          
       STR2DOUBLE,         
       INT2STR,
       DOUBLE2STR,
       INT2DOUBLE,
       DOUBLE2INT,
       JSCONVIT  //由js处理 不管转换
    };
    
    //初始化得到目标DataSet的数据结构
    void init(FlowLineDao& dao);
    
    
    //映射处理;
    void process(Context & ctx, pair<string, PDataSet>& srcdata, map<string, PDataSet>& result);
       
    
    virtual void finish(Context& ctx, bool isAllFinished, map<string, PDataSet>& result );
    
    /**
     * 得到对象数组转换映射表
     * 根据目标字段名，从element中得到扩展属性，扩展属性保存的是"destField = src.f1";javaScript脚本
     * 如果没有找到，就从src里找名字相同的字段的序号，记录在pair的int中,ConvitType里存这他们的类型关系.
     * 扩展属性中找到的放到vector：rowjs中，并把下标加1,取负数存在pair的int中，,ConvitType=JSCONVIT.
     * 创建RowJavaScript时需要用到JSContainer
     * 使用了 this->m_dstfld,this->m_dsttype
     */
    void getMappingScript(vector< pair< int ,ConvitType> >& mappingidx ,
                    vector< RowJavaScript* >& rowjs , PDataSet src);
        
    //得到字段名数组
    void getFieldName(PDataSet data,vector<string>& fields);
    
    //TYPE
    void getFieldDataType(PDataSet data, vector< DataType >& datatype);
    
    //得到目标DataSet的结构
    //PDataSet getDestinationDataSet(const RepositoryDefinition& repdef);
    
    //释放js指针
    void releaseRowJavaScript(vector< RowJavaScript* >& rowjs);
    
    //得到转换类型
    ConvitType checkConvitType(dataset::DataType srctype,dataset::DataType dsttype);
    bool isFoundReference(map<string, DataSet::iterator>& result);
    /**
    * 跳掉参照不到的字段
    */
    //bool isSkipReferenceFalt(){return skipReferenceFalt;}
    /**
    * 跳掉参照不到的字段
    */
    //void setSkipReferenceFalt(bool skip){skipReferenceFalt = skip;}
private :
    //int  definitionId;
    //JSContainer container;
    vector< string > m_dstfld; //初始化就有了
    vector< DataType > m_dsttype;//初始化时候也有了
    
    //有就建一个没有就不建立 记得释放指针 
    //vector< RowJavaScript* >  m_rowjs;  
   /**
    * 按照顺序，第一个值的含义是存src的下标，第二个类型转换选择 
    * 0: 1 ,NONE  src[1]
    * 1: 2 ,STR2INT  src[2]
    * 2: 0 ,STR2DOUBLE  src[0]
    * 3: -1   js[(-1+1)*-1]  取得那个js[0]处理器  不管转换
    * 4: -2   js[(-2+1)*-1]  取得那个js[1]处理器  不管转换
    * 5: 3 ,DOUBLE2INT
    **/
    //vector< pair< int ,ConvitType> > m_mappingidx; //目标字段与src和m_rowjs对应下标,下标与dstfld一一对应 
    /**
    * 跳掉参照不到的字段 ,false填写空值
    */
    //bool skipReferenceFalt;
    map<  string ,  PDataSet >  mapdstds;
    
    //所有的js
    RowJSContainer m_jsContainer;
};
}
}

#endif //_DATASETMAPPINGADAPTER_H_
