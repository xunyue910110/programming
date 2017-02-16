#ifndef _FILERESOURCEREPOSITORYADAPTER_H_
#define _FILERESOURCEREPOSITORYADAPTER_H_

#include "flowline/adapter/ResourceRepositoryAdapter.h"
#include "fileadapter/FileRepositoryParameter.h"
#include "flowline/FlowLineDao.h"
#include "flowline/scripts.h"

//#define _FILE_ADAPTER_REPOSITORY_DEFINITION_ID_  "repositoryDefinitionId" 
#define _FILE_TEMP_FOLDER_KEY_  "flowline.fileadapter.TempFolder" 
#define _FILE_ERR_FOLDER_KEY_   "flowline.fileadapter.ErrorFolder" 

namespace fileadapter
{

class FileResourceRepositoryAdapter : public flowline::adapter::ResourceRepositoryAdapter, public fileadapter::FileRepositoryParameter
{
public:
	FileResourceRepositoryAdapter();
    
	virtual ~FileResourceRepositoryAdapter();
    
    void init(FlowLineDao& dao){     
         
         this->initExtendedAttribute(this->getMetadata()); 
         this->initExtendedAttribute(this->getTarget());        
         //m_definition_id = atoi(this->getTarget()->getExtendedAttribute(_FILE_ADAPTER_REPOSITORY_DEFINITION_ID_).c_str());
         
    }
    //初始化了所有参数      
    //void configure(const string & configuration){
    //     this->parseConfigure(configuration);
    //     this->initParameters();
    //}
    //Common
    virtual PResource recover(const string& resourceName);
    PResource createFileResource(const string& resourceName,const string& tempname = "");
    
    //Input
    virtual void list(Context& ctx, vector<PResource>& result);
    virtual void afterTargetCommitted(Context& ctx, PResource source);

    //Output
    virtual PResource create(Context& ctx);
    virtual void prepareCommit(Context& ctx, PResource target);
    virtual void commit(Context& ctx, PResource target);
    virtual void rollback(Context& ctx, PResource target);

    //内部用函数
    //string conviteFileName(const string& macrostr,Context& ctx);
    
    //void setRepositoryDefinitionId(int id){
    //    m_definition_id = id;
    //}     
    //  
    //int getRepositoryDefinitionId(){
    //    return m_definition_id;
    //}
 private :   
     //FileDefinition
     //int  m_definition_id;
     RowJSContainer m_jsContainer;
     
};

};

#endif //_FILERESOURCEREPOSITORYADAPTER_H_
