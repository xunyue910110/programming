#include "FileResourceRepositoryAdapter.h"

#include "file/File.h"
#include "ftrans/FSystemAgent.h"
#include "fileadapter/MacroAdapter.h"
#include "flowline/FlowLineException.h"
#include "base/ClassRegister.h"
#include "flowline/flbase.h"
#include "log4cpp/Logger.h"
#include "flowline/adapter/CheckerAdapter.h"

#include <string>

using namespace flowline;
using namespace flowline::adapter;

namespace fileadapter
{
class FileContentCheckerAdapter: public  flowline::adapter::CheckerAdapter
{
private :
    int m_accessId ;
    string m_dir;
    string m_file_pattern;
    string m_content;
    const Host * m_Host;
    
public :
	FileContentCheckerAdapter(){		
	}
	void init(FlowLineDao& dao){
	    m_dir = getTarget()->getExtendedAttribute( "dir");	    
	    m_dir = (!m_dir.empty()&& m_dir.at(m_dir.size()-1)=='/')? m_dir :(m_dir + "/");	 
	       	
		m_file_pattern = getTarget()->getExtendedAttribute( "pattern");
		m_content  = getTarget()->getExtendedAttribute( "content" );
		
		//初始化连接得到主机的ID
		string id = getTarget()->getExtendedAttribute( "accessId" );
		m_accessId = atoi( id.c_str() );
        m_Host = &dao.getHost( m_accessId );      
	}
	
    bool check(Context & ctx){
    	init(ctx.getFlowLineDao());  //初始化
    	ASSERT(m_Host!=0);
    	int errCode=0;
    	string errInfo;
    	FSystemAgent * ftp;    	
		//ftp = FSystemAgent::getInstance( m_Host->isLocalHost ? "FTP" : "FTP");
		ftp = FSystemAgent::getInstance("FTP");
		ftp->initialize();    
		ftp->setOption(MAXTRANSFILE,10000);		
		int r = ftp->connect(m_Host->ip, "21" , m_Host->username, m_Host->password);		
		LOG_TRACE_P3(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s", m_accessId ,m_Host->ip.c_str(),m_Host->username.c_str());     
		if( r < 0){
		    ftp->getErrInfo(errCode,errInfo); 
		    LOG_TRACE_P4(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s@%s", m_accessId ,m_Host->ip.c_str(),m_Host->username.c_str(),m_Host->password.c_str());     
			LOG_TRACE_P2(ctx.getLogger(),"FTP不能正常登录，连接失败：%d,%s",errCode,errInfo.c_str());
		    THROW_C(ExecutionException,errCode,errInfo);
		}
						
        MacroAdapter maco(ctx); 
        string filter = flowline::resultMacrofilter(m_file_pattern,maco);  
		//cout <<"-------------LIST-------------"<<r <<endl;
		cout << "FTP Directory(路径):" << this->m_dir  <<endl;
        cout << "FTP filter:"<< m_file_pattern <<"[" << filter <<"]"  <<endl;     
        vector<string> ftplist;
        int k = ftp->changeDirectory(this->m_dir);
        if(k == 0) r = ftp->listDirectory(this->m_dir, ftplist , filter);//将所有符合条件的文件放在vector<string> ftplist
        if( k < 0){
            ftp->getErrInfo(errCode,errInfo);
            LOG_TRACE_P2(ctx.getLogger(),"FTP 取得文件列表异常:%d,%s",errCode,errInfo.c_str());
            ftp->disconnect();
            delete ftp;
            return checkContent(ctx, m_content ,-1);//修改成-1可能连目录都不存在，不用抛异常，可以把这个当作文件不存在
        }       
        string name;
        int ii = 0;    
        std::vector<string>::iterator it = ftplist.begin();
        for(; it !=ftplist.end();it++){
            name = *it;           
            cout << "List FileResource : " << this->m_dir <<":   "<< name << endl;
            ii++;
        }
        ftp->disconnect();
        delete ftp;         
    //	cout <<"-------------LIST-------------end--" <<endl;
    	
    	return checkContent(ctx, m_content ,ii);    	
    }
    
    bool checkContent(Context& ctx,const string& content,int count){
   
       if(content.empty()){
          LOG_TRACE_P2(ctx.getLogger(),"文件检测参数信息为空，默认表示文件数大于0就为合格:%d,%s",count,content.c_str());
       }
       if(content == "content"){
    	cout<<"input is correct ok ......"<<endl;
    	return true;
       }
       if(content == "not"){
       	cout<<"文件内容不存在,异常"<< "/ "<<count<<endl;
       	return false;
        }
     
       LOG_TRACE_P2(ctx.getLogger(),"文件存在检测不合格:%s, 文件数量:%d",content.c_str(),int(count));
       return false;
    }
};

   REGISTER_CLASS("FileContentCheckerAdapter", FileContentCheckerAdapter, Adapter);
  
};

