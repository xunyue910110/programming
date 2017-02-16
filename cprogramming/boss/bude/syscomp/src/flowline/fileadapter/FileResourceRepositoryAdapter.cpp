#include "FileResourceRepositoryAdapter.h"

#include "fileadapter/FileResource.h"
#include "file/File.h"
#include "ftrans/FSystemAgent.h"
#include "fileadapter/MacroAdapter.h"
#include "base/StringUtil.h"
#include "base/supdebug.h"
#include "base/UUID.h"
#include "flowline/FlowLineException.h"

#include "base/ClassRegister.h"
#include "flowline/flbase.h"
#include "flowline/SystemInfo.h"
#include "log4cpp/Logger.h"
#include "base/Properties.h"
#include "fileadapter/CDateTime.h"
#include "base/PatternMatcher.h"
#include <string>

using namespace flowline;
using namespace flowline::adapter;


namespace fileadapter
{

REGISTER_CLASS("FileRepositoryAdapter", FileResourceRepositoryAdapter,Adapter);

FileResourceRepositoryAdapter::FileResourceRepositoryAdapter()
{
}

FileResourceRepositoryAdapter::~FileResourceRepositoryAdapter()
{
}
//Common
//重建一个 FileResource 

PResource FileResourceRepositoryAdapter::recover(const string& resourceName){
	return createFileResource(resourceName,"uuid.tmp");
}

PResource FileResourceRepositoryAdapter::createFileResource(const string& resourceName,const string& tempname){
    FileResource* res =  new FileResource(this->getDirectory(), resourceName );
   
	res->setAccessId(this->getAccessId());
	if(!this->getErrFolder().empty()){
	    res->setErrFolder(this->getErrFolder());
	}
	if(!this->getTempFolder().empty()){
	    res->setTempFolder(this->getTempFolder());
	}   
	if(this->getLimitType() == this->LIMIT_RECORD){
	    res->setMaxRecordCount(this->getLimitRecord());
	}	
	if(this->getSeparator()!= 0){
		res->setSplitChar(getSeparator());
	}	
	if(!this->getHeader().empty()){
		res->setHeader(getHeader());
	}		
	if(!this->getFooter().empty()){
		res->setFooter(getFooter());
	}	
	if(!tempname.empty()){
		UUID uid = UUID::generate();  
        string s1 = uid.ascii(); //s1 = uid.str();
        s1 += ".tmp";
	 	res->setTempName(s1); 	 
	}	
	res->setFileAppend(this->getFileAppend());	
    res->setMetadata(this->getMetadata());   
      
    //打印信息
    //res->printResourceInfo();
    
    return PResource(res);
}


//Input     
void FileResourceRepositoryAdapter::list(Context& ctx, vector<PResource>& result){
       FP_BEGIN(FILE_LIST);
        const Properties& prop = ctx.getEnvironment();
        string temp = prop.getProperty( string(_FILE_TEMP_FOLDER_KEY_));       
        if(!temp.empty()){            
        	this->setTempFolder(temp);
        }
        temp = prop.getProperty( string(_FILE_ERR_FOLDER_KEY_));
        if(!temp.empty()){
        	this->setErrFolder(temp);
        }   
        //string macrostr = getFileName();//带宏的文件名
        string filter;   
        string macrostr = getListRule();//用listRule
        if(macrostr.find("js:") == 0) {
           m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
           m_jsContainer.get(macrostr.substr(3)).execute(filter);
        } else {       
          MacroAdapter maco(ctx); 
          filter = flowline::resultMacrofilter(macrostr,maco);     
        }
                
        Host h = ctx.getFlowLineDao().getHost(this->getAccessId()); 
        int errCode;
        string errInfo;
        std::vector<string> ftplist;
        
        FSystemAgent*  ftp = FSystemAgent::getInstance( h.isLocalHost ? "FILE" : "FTP");
        ftp->initialize(); 
        ftp->setOption(MAXTRANSFILE,10000);
        LOG_TRACE_P3(ctx.getLogger(),"FTP connect Host:%s:%s User:%s",h.ip.c_str(),"21",h.username.c_str());
        int r = ftp->connect(h.ip, "21" , h.username, h.password);
        cout << "FTP getDirectory():" << this->getDirectory()  <<endl; 
        cout << "FTP filter:"<< macrostr <<"[" << filter <<"]"  <<endl;     
        cout << "FTP :" << r <<endl;
        //if(r == 0) r = ftp->changeDirectory(this->getDirectory());//zzc -delete -9-1
        //cout << "FTP :" << r <<endl;  //zzc -delete -9-1
        cout<<"this->getDirectory()"<<this->getDirectory()<<endl;
        if(r == 0) r = ftp->listDirectory(this->getDirectory(), ftplist , "");
        cout << "FTP :" << r <<endl;
        if( r < 0){
            ftp->getErrInfo(errCode,errInfo);
            LOG_TRACE_P2(ctx.getLogger(),"FTP exception:%d, %s",errCode, errInfo.c_str());
            ftp->disconnect();
            delete ftp;
            THROW_C(DefinitionException,errCode,errInfo);
        }
        cout <<"-------------LIST-------------"<<r <<endl;
        string name;
        int c = 0;
        //long size=0; 
        //zzc---add--20050822----
        int i;
        i = filter.find( "|" );
        std::vector<string>::iterator it = ftplist.begin();
        if(i<0){      
        
        	for(; it !=ftplist.end();it++){
            	name = *it;
            	//对*.tmp文件不进行处理,解决临时文件不能删除问题 Yuaneg 2005-9-19
            	unsigned  int loc = name.find( ".tmp",0);
            	if(loc != string::npos )
              {
            		continue;
            	}            	
            	//begin文件分拣Yuaneg 2005-8-22
            	if (!PatternMatcher::matches(filter.c_str(), name.c_str()))
            	{
            		continue;
            	}

            	PResource  res = this->createFileResource(name,"uuid.tmp");               
                        
            	result.push_back(res);            
            	c++;
        	}
        }
        else{
    		int j = 0;
        	std::vector<string>list;
        	std::vector<string>::iterator it_list;
        	list.clear();
        	 for ( int i = filter.find( "|" ); i != string::npos; i = filter.find( "|", i + 1 ) ) {
                   list.push_back(filter.substr(j,i-j));
                   j = i + 1 ;   
            }
            list.push_back(filter.substr(filter.find_last_of("|")+1));
            
            for(; it !=ftplist.end();it++){
            	name = *it;
            	//对*.tmp文件不进行处理,解决临时文件不能删除问题 Yuaneg 2005-9-19
            	unsigned  int loc = name.find(".tmp",0);
            	if(loc != string::npos )
              {
            		continue;
            	}            	
            	//begin文件分拣Yuaneg 2005-8-22
	            for(it_list = list.begin(); it_list != list.end(); it_list++){    
	            		if (!PatternMatcher::matches((*it_list).c_str(), name.c_str()))
	            		{ 
	            			continue;
	            		}
	            		else{
	            		  cout << "file_source:"<<name<<endl;
	           		  PResource  res = this->createFileResource(name,"uuid.tmp");                          
	            	           result.push_back(res);            
	            	           c++;
	            	       	   break;
	            		}
                	}  
        	}
        	list.clear();
    	}
      //zzc-------end----------------------------- 

        ftp->disconnect();
        delete ftp;  
        //THROW(DefinitionException,"目录不存在或不可读错误");
        cout << "------END-LIST-----"  << ":" << c << endl;  
        FP_END(FILE_LIST);
}

void FileResourceRepositoryAdapter::afterTargetCommitted(Context& ctx, PResource source){
    cout << "afterTargetCommitted()" << endl;
    FileResource* fileSource  =  dynamic_cast<FileResource*> (source.get());           
   
    if(fileSource->isCloseed()){
        fileSource->open(ctx);
    }
    cout << "FinishActionType:" <<  this->getFinishActionType() << endl;
    switch(this->getFinishActionType()){
        case MOVE:  
        {   
        	cout << "getBakDirectory():" << this->getBakDirectory() <<endl;
            fileSource->moveResource(this->getBakDirectory(),false);
            FL_TRACE("[flowline]源文件备份完毕");   
            break; 
        }  
        case DELETE:
        {
            fileSource->moveResource(this->getBakDirectory(),true);
            FL_TRACE("[flowline]源文件删除完毕");  
            break;
        } 
        case NONE: 
        default:           
            FL_TRACE("[flowline]源文件AfterCommit处理NONE"); 
    }
} 


//Output 作为输出，检查文件 是多对一还是覆盖
PResource FileResourceRepositoryAdapter::create(Context& ctx)
{      
	    const Properties& prop = ctx.getEnvironment();
        string temp = prop.getProperty( string(_FILE_TEMP_FOLDER_KEY_));
        if(!temp.empty()){
        	this->setTempFolder(temp);
        }    
        temp = prop.getProperty( string(_FILE_ERR_FOLDER_KEY_));
        if(!temp.empty()){
        	this->setErrFolder(temp);
        }    
            
        Node*  thisNode =  dynamic_cast< Node* >(getTarget());
        size_t psize = thisNode->getPreviousNodeCount();
        if(this->getFileAppend() == AUTO_APP){
        	if(psize > 1){
        		this->setFileAppend(APPEND);
        	}else{
        	    this->setFileAppend(COVER);
        	}       
        }
	    //只为创建对象用,物理文件暂时不存在
	   
        MacroAdapter maco(ctx);       
        char ch[10];
        sprintf(ch,"%d",getTarget()->getId());  
        
        string key = "file.";
        string datekey = CDateTime::now("YYYYMMDD");        
        key += datekey + ".";
        key += string(ch);
            
        maco.setTargetId(key);    
        
        string fname;
        string macrostr = this->getFileName();   //带宏的文件名
        if(macrostr.find("js:") == 0) {
           m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
           m_jsContainer.get(macrostr.substr(3)).execute(fname);
        } else {   
           fname= flowline::resultMacro(macrostr,maco); 
        }      	
        
      string fnametemp = "uuid.tmp";  //都取uuid            
	    PResource  res = this->createFileResource(fname,fnametemp);  
	    cout << "Create  FileResource:" << macrostr << endl;        
	       
	    return PResource(res);
}

void FileResourceRepositoryAdapter::prepareCommit(Context& ctx, PResource target)
{          
		cout << "prepareCommit" << endl; 
}

void FileResourceRepositoryAdapter::commit(Context& ctx, PResource target)
{  
		cout << "commit" << endl; 
		FileResource* ftrg = dynamic_cast< FileResource* >(target.get());
		ftrg->milestone(ctx);	
		cout << "renameTo: " << ftrg->getTempName() << " -> " << ftrg->getName()<<endl;    
}

void FileResourceRepositoryAdapter::rollback(Context& ctx, PResource target)
{
		cout << "rollback" << endl;
		
		FileResource* ftrg = dynamic_cast< FileResource* >(target.get());
		//if(ftrg->isCloseed()){
		//	ftrg->open(ctx);
		//}
		//zzc -change ,对于追加文件做回滚时，不删除目标文件9-3
		//if((ftrg->fileAppend==1){
		 if(ftrg->getFileAppend()==1){ 
		  FL_TRACE_P1("[flowline]追加文件保存目标文件:%s",ftrg->getName().c_str());      
		}
		else{
			if(ftrg->isCloseed()){
				ftrg->open(ctx);
			}
			ftrg->moveResource("",true);
			FL_TRACE_P1("[flowline]删除已经完成的目标文件:%s",ftrg->getName().c_str()); 
		}
}

//内部用函数
//string FileResourceRepositoryAdapter::conviteFileName(const string& macrostr,Context& ctx){
//        MacroAdapter maco(ctx);       
//        char ch[10];
//        sprintf(ch,"%d",getTarget()->getId());      
//        maco.setTargetId(string(ch));
//        
//        string fname = flowline::resultMacro(macrostr,maco); 
//       
//        return fname;
//}

};
