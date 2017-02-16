#include "DataSetMappingAdapter.h"

#include "flowline/adapter/DataProcessorAdapter.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "base/supdebug.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/Element.h"
#include "flowline/flbase.h"

#include <string>
#include <vector>
#include <map>
namespace flowline
{
namespace dataprocessor
{


void DataSetMappingAdapter::init(FlowLineDao& dao)
{       
	    cout << "DataSetMappingAdapter.init()" <<endl;  
		//definitionId = atoi(this->getTarget()->getExtendedAttribute(_MAPPING_ADAPTER_REPOSITORY_DEFINITION_ID_).c_str());
		//ASSERT(definitionId != 0);
		//const RepositoryDefinition&  repdefi = dao.getRepositoryDefinition(definitionId);
		//PDataSet dest = getDestinationDataSet(repdefi);

       	const Metadata* dstmetadata = this->getMetadata(""); 
		if(dstmetadata == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}                         
		PDataSet dest = dstmetadata->createDataSet();          
        this->getFieldName(dest,this->m_dstfld);
            
        this->getFieldDataType(dest,this->m_dsttype);
};          

 /**
 * 得到对象数组转换映射表
 * 根据目标字段名，从element中得到扩展属性，扩展属性保存的是"destField = src.f1";javaScript脚本
 * 如果没有找到，就从src里找名字相同的字段的序号，记录在pair的int中,ConvitType里存这他们的类型关系.
 * 扩展属性中找到的放到vector：rowjs中，并把下标加1,取负数存在pair的int中，,ConvitType=JSCONVIT.
 * 创建RowJavaScript时需要用到JSContainer
 * 使用了 this->m_dstfld,this->m_dsttype
 *
 * 按照顺序，第一个值的含义是存src的下标，第二个类型转换选择 
 * 0: 1 ,NONE  src[1]
 * 1: 2 ,STR2INT  src[2]
 * 2: 0 ,STR2DOUBLE  src[0]
 * 3: -1   js[(-1+1)*-1]  取得那个js[0]处理器  不管转换
 * 4: -2   js[(-2+1)*-1]  取得那个js[1]处理器  不管转换
 * 5: 3 ,DOUBLE2INT
 */
void DataSetMappingAdapter::getMappingScript(vector< pair< int ,ConvitType> >& mappingidx , vector< RowJavaScript* >& rowjs , PDataSet src){
//		cout << "DataSetMappingAdapter::getMappingScript:mappingidx.size:" << mappingidx.size()<<endl;
//		ASSERT(element!=0);
        Element* element = getTarget();
		ASSERT(src!=0);
		//releaseRowJavaScript(rowjs);//释放会带来不可知的问题		
		
		int dstsize = this->m_dstfld.size();
		string attr;
		ConvitType cvttype = NONE;
    for(int i = 0 ;i < dstsize;i++ ){
    	attr = element->getExtendedAttribute(this->m_dstfld[i]); 
    	  int srcidx = -1;
        if(!attr.empty()){
        	srcidx = src->getFieldIndex(attr.substr(attr.find('.')+1).c_str());
        	if(srcidx == -1) {            	
        		rowjs.push_back(&m_jsContainer.get(attr.c_str()));
    				mappingidx.push_back(pair<int ,ConvitType> ((0 - rowjs.size()),JSCONVIT));//用负数表示 -1 对应 rowjs[0];不管转换
    				continue;
    			}
        } else {
        	//配置里没有找到，就从源头上找
           srcidx = src->getFieldIndex(m_dstfld[i].c_str());          
           if(srcidx == -1) {
           	  THROW_P1(DefinitionException, "执行转换目标字段找不到源字段或脚本:%s", this->m_dstfld[i].c_str());
           }  
        }  
         
    	  cvttype = checkConvitType(src->getField(srcidx).getDataType(),this->m_dsttype[i]);                 
        mappingidx.push_back(pair< int ,ConvitType> (srcidx,cvttype));                 
            
    }
}    
    

  //得到字段名数组
void DataSetMappingAdapter::getFieldName(PDataSet data,vector<string>& fields){
    fields.clear();
    int fcount = data->fieldCount();
    for(int i=0; i<fcount; i++) {
        fields.push_back(data->getField(i).getFieldName());
    }   
}
//TYPE
void DataSetMappingAdapter::getFieldDataType(PDataSet data, vector< DataType >& datatype){ 
    datatype.clear();  
	int fcount = data->fieldCount();
    for(  int i = 0;i < fcount ;i++){
        datatype.push_back(data->getField(i).getDataType());
    }   
}


void DataSetMappingAdapter::process( Context & ctx, pair<string, PDataSet>& srcdata, map<string, PDataSet>& resultDataSet ){
		FP_BEGIN(MAPPING);
		LOG_TRACE_P1(ctx.getLogger(),"DataSetMappingAdapter:process:%s","");	
		//后一个名字是什么不知道，""会自动查找到
		const Metadata* dstmetadata = this->getMetadata(); 
		if(dstmetadata == 0){
			THROW_P1(DefinitionException, "执行转换目标Metadata没有找到:%s", "");       	
		}
		PDataSet dstds = dstmetadata->createDataSet(); 
		int fmet= dstmetadata->fields().size();    
	        int fcount = dstds->fieldCount();
	   
	    //cout << "[FLOWLINE]目标Dataset字段size:"<< fmet<<"/"<< fcount <<"/"<<dstmetadata->getId()<<endl;
		string srcname = srcdata.first;
		PDataSet srcds = srcdata.second; 
		map<string, DataSet::iterator> result;
			
				
		DataSet::iterator it = srcds->begin(); //源头的叠代指针 
		vector< RowJavaScript* > rowjs;  
        vector< pair< int ,ConvitType> > mappingidx;
        
		this->getMappingScript(mappingidx, rowjs, srcds);   
		//把指针传给Script    
		m_jsContainer.setGlobalObject(srcname.c_str(), RowJavaScript::rowClass(), &it);  
        m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
                
		DataSet::iterator itdst;//目标的叠代指针
		int destfldcount = this->m_dstfld.size();//与m_mappingidx长度相同
		
		int tmpint = 0;
		double tmpdouble = 0.0;
		const char * tmpchar;
		ConvitType cvttype = NONE;
	        char tchar1[10];
		char tchar2[20]; 
        	ostringstream oos;	
		for(; it!= srcds->end(); it++){
       	          itdst = dstds->pushBack();//添加一行
			for(int i=0; i < destfldcount; i++){
			     int jsidx = mappingidx[i].first;//左边是下标，右边是转换类型
			     
			     if(jsidx < 0){	//采用javaScript处理     	
			        jsidx = (jsidx+1) * -1; //取得序号
			        rowjs[jsidx]->execute((*itdst)[i]);	
			        
				 } else  {// >=0直接复制
					cvttype = mappingidx[i].second; 
					switch(cvttype){
                        case  NONE:
					        (*itdst)[i] = (*it)[jsidx];	
					        break;
					    case  STR2INT:
					        {
					        	tmpchar = (*it)[jsidx].c_str();	
					        	(*itdst)[i] = atoi(tmpchar);
					        }
						    break;
					    case STR2DOUBLE:
						    {
						        tmpchar = (*it)[jsidx].c_str();	
						        (*itdst)[i] = atof(tmpchar);
						    }
						    break;
					    case INT2STR:
						    {   tmpint = (*it)[jsidx];	
						    //    char tchar1[10];
						        sprintf(tchar1,"%d",tmpint);
						        (*itdst)[i] = tchar1;
						    }
						    break;
					    case DOUBLE2STR:
					         {   
					         	tmpdouble = (*it)[jsidx];	
						  //      char tchar2[20];
						        sprintf(tchar2,"%f",tmpdouble);
						        (*itdst)[i] = tchar2;
						    }
						     break;
					    case INT2DOUBLE:
					        {   
					         	tmpint = (*it)[jsidx];							      
						        (*itdst)[i] = (double)tmpint;
						    }
						    break;
					    case DOUBLE2INT:
					        {   
					         	tmpdouble = (*it)[jsidx];							      
						        (*itdst)[i] = (int)tmpdouble;
						    }
						     break;
						//---------------------
						 case  STR2INT64:
					        {
					        	tmpchar = (*it)[jsidx].c_str();	
					        	(*itdst)[i] = StringUtil::toValue<int64_t>(tmpchar);
					        }
						    break;
					    case INT2INT64:
						    {
						        tmpint = (*it)[jsidx];
						        (*itdst)[i] = tmpint;
						    }
						    break;
					    case INT642STR:
					    	 
						    {   
						//	ostringstream oos;
						      //Yuaneg 2005-8-29 清空
						      oos.str("");
						    	oos << (*it)[jsidx];							        
						        (*itdst)[i] = oos.str().c_str();
						    }
						    break;
						//---------------------
					    case JSCONVIT:
					    default:
					        THROW_P1(DefinitionException, "执行转换目标字段不是找源字段应该是脚本:%s", "JSCONVIT");
       
					}//switch
			     }//else			
			}//for       
       } //for
       resultDataSet.insert(pair<string , PDataSet> (string(),dstds));
       LOG_TRACE_P1(ctx.getLogger(),"DataSetMappingAdapter:result:%d",dstds->size());
       FP_END(MAPPING);
}

//完成处理
void DataSetMappingAdapter::finish( Context& ctx, bool isAllFinished, map<string, PDataSet>& result ){
   
    FL_TRACE("DataSetMappingAdapter::finish()");
}
 
//  //释放js指针
//void  DataSetMappingAdapter::releaseRowJavaScript(vector< RowJavaScript* >& rowjs){
//      cout << "// TODO 释放RowJavaScript*指针" << endl;
//      vector< RowJavaScript* >::iterator  it = rowjs.begin();
//       for( ;it!=rowjs.end();it++){
//          RowJavaScript* pjs= *it;//内容是一个指针
//          delete pjs;
//          *it = 0;
//       }
//       rowjs.clear();
//    }
    
//得到转换类型
DataSetMappingAdapter::ConvitType DataSetMappingAdapter::checkConvitType(dataset::DataType srctype,dataset::DataType dsttype){       
   if(srctype == dsttype) return NONE;
   switch(srctype){
        case dataset::FTSTRING:
            return (dsttype == dataset::FTINT)? STR2INT: (dsttype == dataset::FTDOUBLE)? STR2DOUBLE :STR2INT64 ;           
        case dataset::FTDOUBLE :
            return (dsttype == dataset::FTSTRING)? DOUBLE2STR : DOUBLE2INT;
        case dataset::FTINT :
            return (dsttype == dataset::FTSTRING)?INT2STR:(dsttype == dataset::FTDOUBLE)? INT2DOUBLE : INT2INT64 ;
        case dataset::FTINT64 :
            return (dsttype == dataset::FTSTRING)?INT642STR:NONE;//INT64不能转成INT32
        default :         	
        	break ; 
   }  
   return NONE;	  
}  
//是否查到参照
bool DataSetMappingAdapter::isFoundReference(map<string, DataSet::iterator>& result){
	if(result.size()==0)return true;
	map<string, DataSet::iterator>::iterator it =  result.begin();
    DataSet::iterator dait;
	for(;it!=result.end();it++){
		 dait = (*it).second;
		 if(dait == (*dait).getDataSet()->end()){
		 	 cout << "有一条记录没有参照到数据" <<endl;
		     return false;
		 }	
	}		
	return true;
}

REGISTER_CLASS("DataSetMappingAdapter", DataSetMappingAdapter, Adapter);
}
}
