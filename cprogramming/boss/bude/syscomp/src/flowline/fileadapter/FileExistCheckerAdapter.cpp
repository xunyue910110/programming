#include "FileResourceRepositoryAdapter.h"

#include "file/File.h"
#include "ftrans/FSystemAgent.h"
#include "fileadapter/MacroAdapter.h"
#include "flowline/FlowLineException.h"
#include "base/ClassRegister.h"
#include "flowline/flbase.h"
#include "log4cpp/Logger.h"
#include "flowline/adapter/CheckerAdapter.h"
#include "base/PatternMatcher.h"
#include <string>

using namespace flowline;
using namespace flowline::adapter;

namespace fileadapter
{
class FileExistCheckerAdapter: public  flowline::adapter::CheckerAdapter
{
private :
    string m_dir;
    string m_file_pattern;
    string m_parameters;
    const Host * m_Host;
    int m_accessId ;
public :
	FileExistCheckerAdapter(){		
	}
	void init(FlowLineDao& dao){
	    m_dir = getTarget()->getExtendedAttribute( "dir");	    
	    m_dir = (!m_dir.empty()&& m_dir.at(m_dir.size()-1)=='/')? m_dir :(m_dir + "/");	 
	       	
		m_file_pattern = getTarget()->getExtendedAttribute( "pattern");
		m_parameters = getTarget()->getExtendedAttribute( "parameters" );
		
		//初始化连接
		string id = getTarget()->getExtendedAttribute( "accessId" );
		m_accessId = atoi( id.c_str() );
        m_Host = &dao.getHost( m_accessId );      
	}
	
    bool check(Context & ctx){
    	init(ctx.getFlowLineDao());
    	ASSERT(m_Host!=0);
    	int errCode=0;
    	string errInfo;
    	FSystemAgent * ftp;    	
		ftp = FSystemAgent::getInstance( m_Host->isLocalHost ? "FILE" : "FTP");
		ftp->initialize();    
		ftp->setOption(MAXTRANSFILE,10000);		
		int r = ftp->connect(m_Host->ip, "21" , m_Host->username, m_Host->password);		
		LOG_TRACE_P3(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s", m_accessId ,m_Host->ip.c_str(),m_Host->username.c_str());     
		
		if( r < 0){
		    ftp->getErrInfo(errCode,errInfo); 
		    LOG_TRACE_P4(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s@%s", m_accessId ,m_Host->ip.c_str(),m_Host->username.c_str(),m_Host->password.c_str());     
			LOG_TRACE_P2(ctx.getLogger(),"FTP登录，连接失败：%d,%s",errCode,errInfo.c_str());
		    THROW_C(ExecutionException,errCode,errInfo);
		}
						
        MacroAdapter maco(ctx); 
        string filter = flowline::resultMacrofilter(m_file_pattern,maco);  
		cout <<"-------------LIST-------------"<<r <<endl;
		cout << "FTP Directory:" << this->m_dir  <<endl;
        cout << "FTP filter:"<< m_file_pattern <<"[" << filter <<"]"  <<endl;     
        vector<string> ftplist;
       // r = ftp->changeDirectory(this->m_dir);
       // cout<<"r value is "<<r<<endl;
        cout<<"this->m_dir value is "<<this->m_dir<<endl;
        
        //if(r == 0) r = ftp->listDirectory(this->m_dir, ftplist , filter);
        if(r == 0) r = ftp->listDirectory(this->m_dir, ftplist , "");//zzc---change--取所有文件
        if( r < 0){
            ftp->getErrInfo(errCode,errInfo);
            LOG_TRACE_P2(ctx.getLogger(),"FTP 取得文件列表异常:%d,%s",errCode,errInfo.c_str());
            ftp->disconnect();
            delete ftp;
            return checkNumber(ctx, m_parameters ,0);
            //THROW_C(DefinitionException,errCode,errInfo);
        }       
         cout <<"-------------LIST-------------"<<r <<endl;
        string name;
        int c = 0;
        //long size=0; 
        //zzc---add--20050822----
        vector<string> ftplist_f;
        vector<string>::iterator it_f;
        std::vector<string>::iterator it = ftplist.begin();
        int i;
        i = filter.find( "|" );
        if(i<0){      
        	for(; it !=ftplist.end();it++){
            	name = *it;
            	//begin文件分拣Yuaneg 2005-8-22
            	if (!PatternMatcher::matches(filter.c_str(), name.c_str()))
            	{
            		continue;
            	}
            	ftplist_f.push_back(name);
            	//cout << "List FileResource:" << name << endl;
            	   
            	//PResource  res = this->createFileResource(name,"uuid.tmp");                     
            	//result.push_back(res);            
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
            
            	//begin文件分拣Yuaneg 2005-8-22
           		 for(it_list = list.begin(); it_list != list.end(); it_list++){
            		if (!PatternMatcher::matches((*it_list).c_str(), name.c_str()))
            		{
            			//cout << "not List FileResource:" << name << endl;   
            			continue;
            		}
            		else{
            			//cout << "List FileResource:" << name << endl;
            			ftplist_f.push_back(name);
            			break;
            		
            		}
            	}   
           		//PResource  res = this->createFileResource(name,"uuid.tmp");                          
            	//result.push_back(res);            
            	c++;
        	}
        	list.clear();
    	}
    	if(ftplist_f.size()==0){
    		FL_TRACE("在所指定的目录中，取得文件列表异常");
    		return checkNumber(ctx, m_parameters ,0);
    		//LOG_TRACE(ctx.getLogger(),"文件存在检测不合格, 文件数量:0");
    		
    	}
    	else{
    		string name;
        	int c = 0;     
        	std::vector<string>::iterator it_f = ftplist_f.begin();
        	for(; it_f !=ftplist_f.end();it_f++){
            	name = *it_f;           
            	cout << "List FileResource:" << this->m_dir << name << endl;
            	c++;
        	}
    	}	
      //zzc-------end----------------------------- 
        
        /*
        string name;
        int c = 0;      
        std::vector<string>::iterator it = ftplist.begin();
        for(; it !=ftplist.end();it++){
            name = *it;           
            cout << "List FileResource:" << this->m_dir << name << endl;
            c++;
        }
        */
        ftp->disconnect();
        delete ftp;         
    	cout <<"-------------LIST-------------end--" <<endl;
    	
    	return checkNumber(ctx, m_parameters ,c);    	
    }
    
    bool checkNumber(Context& ctx,const string& param,int count){
       if(param.empty()){
          LOG_TRACE_P2(ctx.getLogger(),"文件检测参数信息为空，默认表示文件数大于0就为合格:%d,%s",count,param.c_str());
       }
       if(param=="0" && count == 0){
           return true;
       }
       if(param=="1" && count == 1){
           return true;
       }
       if(param=="n" && count > 1){
           return true;
       }
       if(param=="0..1" && count <=1){
       	   return true;
       }
       if(param=="1..n" && count >=1){
       	   return true;
       }
       if(param=="n..n" && count >1){
       	   return true;
       }
       if(param=="0..n"){
       	   return true;
       }
       std::cout<<"///////////////"<<std::endl;
       LOG_TRACE_P1(ctx.getLogger(),"文件存在检测不合格:%s, 文件数量: -1",param.c_str());
       return false;
    }
};

   REGISTER_CLASS("FileExistCheckerAdapter", FileExistCheckerAdapter, Adapter);
  
};
