#include "FileResource.h"
#include <string>

#include "flowline/DataSet.h"
#include "flowline/Resource.h"
#include "base/supdebug.h"
#include "flowline/SystemInfo.h"
#include "flowline/FlowLineException.h"
#include "flowline/flbase.h"
#include "log4cpp/Logger.h"
#include "base/StringUtil.h"

#include "file/File.h"
#include "ftrans/FSystemAgent.h"
#include <fstream>
#include "dataset/DataSet.h"
#include <errno.h>
#include <unistd.h>

USING_NAMESPACE(std);
using namespace log4cpp;
using namespace flowline;
//using namespace dataset;
namespace fileadapter
{

FileResource::~FileResource()
{
   this->close();
   
}

// 初始化的时候并不开资源,只有要超作的时候才打开他.
void FileResource::open(Context& ctx){
    //如果已经打开就先关闭掉否则ftp 会core dump.
    if(isclosed == false)
    {
        this->close();        
    }
    int errCode;
    string errInfo;  
    //Host h = ctx.getFlowLineDao().getHost(accessId);   
     this->h = ctx.getFlowLineDao().getHost(accessId); 
    ftp = FSystemAgent::getInstance( this->h.isLocalHost ? "FILE" : "FTP");//zzc 
    //ftp = FSystemAgent::getInstance( h.isLocalHost ? "FTP" : "FTP");//不管他是不是本地，都用FTP
    ftp->initialize();    
    int r = ftp->connect(this->h.ip, "21" , this->h.username, this->h.password);
   
    LOG_TRACE_P3(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s", getAccessId() ,this->h.ip.c_str(),this->h.username.c_str());     
    
    if( r < 0){
        ftp->getErrInfo(errCode,errInfo); 
        LOG_TRACE_P4(ctx.getLogger() , "FTP 连接主机:%d,IP:%s,%s@%s", getAccessId() ,this->h.ip.c_str(),this->h.username.c_str(),this->h.password.c_str());     
        LOG_TRACE_P2(ctx.getLogger(),"FTP登录，连接失败：%d,%s",errCode,errInfo.c_str());
        
        THROW_C(ExecutionException,errCode,errInfo);
    }
    
    //r = ftp->changeDirectory(this->m_dir);
    /*
    if( r < 0){
        ftp->getErrInfo(errCode,errInfo);
        LOG_TRACE_P2(ctx.getLogger(),"FTP exception:%d,%s",errCode,errInfo.c_str());
//        THROW_C(DefinitionException,errCode,errInfo);
    }
    */
    size_t t = ctx.getDefinition().getBlockSize();
    this->setBlokSize(t);  
    
    this->isclosed = false;
    m_count = 0;    
}

//关资源
void FileResource::close()   
{   
    if (isclosed == false){
        if( ftp ){
            ftp->disconnect();
            delete ftp;
            ftp = 0;
        }  
        this->isclosed = true;  
    }
    //关闭写流之前写尾记录 
    if (iState == ON_WRITE && this->streamfile && !m_footer.empty()){
	     fwrite(m_footer.c_str(), m_footer.size(), 1, streamfile);
	  }	    
    //其它地方都不允许关闭流
    if(this->streamfile){
        fclose(this->streamfile);//说明这里已经用过流了
        streamfile = 0;
        iState = ALL_CLOSE;
        
    }
    if(iState < ON_READ){
        iState = NONE;//没有开过流
    } else {
        iState = ALL_CLOSE;//开过的现在都关了
    }
}

/**
 * 如果是NULL, 表示读完毕
 */
PDataSet FileResource::read(Context& ctx){
   	FP_BEGIN(FILE_READ);
    if( isclosed == true)
    {
        this->open(ctx);
    }
    string tmpdir = this->getTempFolder();
    LOG_TRACE_P2(ctx.getLogger() , "FileResource 开始读:%s,%s", tmpdir.c_str(),"");           
   
    //printResourceInfo();
    if(iState <= BEFORE_READ )
    { 	//没有读过而且没有开过
        //第一次需要把文件放到工作目录下
        if(m_tempname.empty()) {m_tempname = m_name;}        
        ASSERT( ftp!= 0);
        int errCode = ftp->getFile(m_dir + m_name, tmpdir + m_tempname); 
        if (errCode < 0)
        {
					File tmpFile(tmpdir + m_tempname);
	   			tmpFile.deleteFile();
          string errInfo; 
          ftp->getErrInfo(errCode,errInfo);
	   			close();
          LOG_TRACE_P2(ctx.getLogger() , "FTP exception:%d,%s", errCode ,errInfo.c_str());
          THROW_C(ExecutionException,errCode,errInfo);
        }
        iState = BEFORE_READ;      
        string s = tmpdir + m_tempname;
        this->streamfile = fopen(s.c_str(), "r"); 
        LOG_TRACE_P1(ctx.getLogger() ,  "打开文件:%s",s.c_str());  
    
    } 
    else 
    	if(iState > ON_READ)
    	{   
        LOG_TRACE(ctx.getLogger() ,  "读完了还要读");  
        return PDataSet() ; //读完了还要读
    	} 

    iState = ON_READ;
    PDataSet dataset = getMetadataDataSet(ctx);
    try 
    {        
        //最多提供的减去已经提供的
        FP_BEGIN(READ_DATA);
        int r = readDataSet(streamfile,dataset,this->getBlokSize()?this->getBlokSize():m_max_count,this->getSplitChar());    
        FP_END(READ_DATA);
        if(r >= 1) 
        {
            m_count += r;
            LOG_TRACE_P2(ctx.getLogger() , "读完返回:%d,%d", r ,m_count); 
            FP_END(FILE_READ);           
            return dataset;
        }
        else 
        {            
            close();
            File tmpFile(tmpdir + m_tempname);
            tmpFile.deleteFile();
            //删除临时文件
            LOG_TRACE_P1(ctx.getLogger() , "删除临时文件:%s", m_tempname.c_str());
            //cout << "Delete read temp file :" << tmpdir + m_tempname << endl;
            return PDataSet();
        }
     }
     catch (ExecutionException& e)
     {
         LOG_TRACE_P2(ctx.getLogger(),"Resource Read Format Exception:%d,%s",e.getCode(),e.toString().c_str()); 
         this->moveFileForMetadataException(ctx);
     }
    //没有读到表示已经读完成.
    iState = ALL_CLOSE;
    LOG_TRACE_P2(ctx.getLogger() , "未读就返回:%d,%d", 0 ,m_count);  
     
    close();
    File tmpFile(tmpdir + m_tempname);
    tmpFile.deleteFile();
    //删除临时文件
    LOG_TRACE_P1(ctx.getLogger() , "删除临时文件:%s", m_tempname.c_str());
    //cout << "Delete read temp file :" <<  tmpdir + m_tempname << endl; 
    return PDataSet();   
}

//写data到文件中，格式已经确定了
void FileResource::write(Context& ctx, PDataSet data) 
{    
	
    FL_TRACE_P1("准备写入文件:%d",data->rowCount());
    //由于m_max_count 目前功能没有失效,
   	/*
   	if(data->rowCount() + m_count > m_max_count) 
   	{
        iState = ALL_CLOSE;
        close();
        FL_TRACE_P1("写满了不再写了:%s","");
        return ;//1满了
    }*/
    //根据文件大小分割 只对非追加文
    long dstlen; 
    File file_name(this->tempfolder,this->m_tempname);
    dstlen=file_name.length();
   	if (this->getFileAppend() == 0 &&(data->rowCount() * data->getRowSize()+ dstlen >= 2147483648) && (this->streamfile != NULL) && this->m_name.length() == 24) 
   	{   
   		  //Yuaneg 2005-09-7
   		  //当文件超过最大输出记录时,重新生成文件
   		    string sa = this->m_name;
		    	string sa_after;
		      string sa_before;
		      sa_after = sa.substr(18);
		    	sa_before = sa.substr(0,14); 
		    	char tchar[5];
		    	std::vector<string> ftplist;
		    	//关闭文件流准备移动文件
	        //写尾记录
	        if(!m_footer.empty()){
	        	fwrite(m_footer.c_str(), m_footer.size(), 1, streamfile);
	      	}		    	
		    	fclose(this->streamfile); 
          this->streamfile = 0;
          //产生移动的文件
          sprintf(tchar,"%04d",this->m_fileno);
          string ss = sa_before + tchar + sa_after;	    				  	
          if(isclosed){ 
				       open(ctx);
				   } 
  				int bb = ftp->listDirectory(m_dir,ftplist,ss);
  				if( bb < 0){
  				   string errInfo;
  				   int errCode = 0;
				     ftp->getErrInfo(errCode,errInfo);
				     THROW_C(DefinitionException,errCode,errInfo);
				  }
  			  if(ftplist.size()!=0)
  			  {
  			  	 //删除已经存在的文件
  			  	 File filed(m_dir,ss);
  			  	 filed.deleteFile();
  			  }
				  int errCode=0;
				  errCode = ftp->getFile(this->tempfolder + this->m_tempname,m_dir + ss);
				  if( errCode < 0){
						string errInfo; 
						ftp->getErrInfo(errCode,errInfo);
            File file(this->tempfolder,this->m_tempname); 
            file.deleteFile();						
						THROW_C(DefinitionException,errCode,errInfo);
          }
				  //cout<<"ts_name"<<ts_name<<endl;
				  file_name.deleteFile();
				  //------------------
				  //重新生生新的流
				  this->m_fileno++;
          iState = BEFORE_WRITE;
          FL_TRACE("成功分割文件");
    }    
    if(iState <= BEFORE_WRITE)
    {
        if(m_tempname.empty()) m_tempname = m_name; 
        string cfile(tempfolder);       
        cfile += m_tempname;          
        
        if(streamfile == NULL)
        {           
           
            FL_TRACE_P1("打开文件准备写:%s",cfile.c_str());
            /*
            File tmpfile(cfile);            
            if(!tmpfile.exists())
            {
                tmpfile.createNewFile();
            } 
            */
                                 
            this->streamfile = fopen(cfile.c_str(), "a+");//,ios_base::app); //垒加不冲掉以前的 ，IBM上不允许app
            //this->streamfile.seekp(0,ios_base::end);
            //写头记录
            if (streamfile){
			        if(!m_header.empty()){
			        	m_header = m_header+'\n';
			        	fwrite(m_header.c_str(), m_header.size(), 1, streamfile);
			      	}              	
          	}
         }     
         if(!streamfile)
         {
            FL_TRACE_P1("打开文件失败:%s",cfile.c_str());
            THROW_C_P1(ExecutionException,-1,"打开文件失败:%s",cfile.c_str());
         }
         iState = ON_WRITE;
    }
    else 
    	if(iState > ON_WRITE) 
    	{
        //cout << "//表示已经写玩了,就不再写了" <<endl;
        FL_TRACE_P1("已经写玩了,关闭文件:%s","");
    	  close();   
        return ;
    	}
    
    int r = writeDataSet(streamfile,data,splitchar);
    FL_TRACE_P1("写入条数:%d",r);     
    if(r > 0)
    {
        m_count += r;
    } 
    else 
    {
    	  FL_TRACE_P1("没有写的内容,关闭文件:%s","");
    	  close();
        iState = ALL_CLOSE;//一条都没有写出去,就不再写了
    } 
    return ;//return 0; //0 , 1 满
}
//转化Metadata到DataSet格式
PDataSet FileResource::getMetadataDataSet(Context& ctx){ 
	 ASSERT( metadata != 0 );
	 PDataSet dataset = metadata->createDataSet();

     if(this->bloksize > 0){
        dataset->reserve(bloksize);
     }     
     return dataset;
}

//TYPE与Length分开放
void FileResource::getDataTypeArray(PDataSet data, vector< int > & datatype) const{   
	DataType dt;
    for(  int i = 0;i < data->fieldCount();i++){
			dt = data->getField(i).getDataType();
		    if(dt == dataset::FTSTRING && data->getField(i).getTag() == flowline::TAG::DATATIME){
				datatype.push_back(METADATA_DATETIME);//表示DATATIME型
		    } else {
		    	datatype.push_back(dt);
            } 
    }   
}
//TYPE与Length分开放，int 和 double的长度不是字符长度
void FileResource::getDataLengthArray(PDataSet data, vector< int >& vlen){      
    for(  int i = 0;i < data->fieldCount();i++){   
        vlen.push_back(data->getField(i).getLength());
    }   
}

inline void FileResource::put2DataSet(DataSet::iterator it,  int i, int type,const char* str ) {
    switch(type){    	
        case FTINT    : (*it)[i] = atoi(str); 
            break;  
        //case FTINT64  : (*it)[i] = atoll(str);
        case FTINT64  : (*it)[i] = StringUtil::toValue<int64_t>(str);
            break;
        case FTDOUBLE : (*it)[i] = atof(str); 
            break;
        case FTSTRING : (*it)[i] = str;
            break;	
        case METADATA_DATETIME: (*it)[i] = dateFormated(i,str,true);
    		break;    
        default:            
            (*it)[i] = str;
    }
}
//是读还是写
inline const char* FileResource::dateFormated(int fieldidx,const char* str,bool isWrite){
	ASSERT( metadata != 0 );
	//const vector<Field>& vf = metadata->fields();
	//if(vf[fieldidx].type != flowline::Field::T_DATETIME) return "";
	//string format = vf[fieldidx].format;
	//STD_DATATIME_FORMAT
	
	//TODO 时间日期的读写
	
	
	return str;
	
}

/**
* 读取数据集合
* @param  istream   输入流 已经格式化
* @param  dataset   输出数据集合
* @param  max_count 读最大行数
* @param  split     行内字段分割符号
* @param  delimiter 行结尾符号
*/
inline int FileResource::readDataSet(FILE* fp,PDataSet dataset,  int blok_count,char split)
{   
    if(!fp) {
    	cout << "文件未打开，不可以读" <<endl;
    	return -1;
    }
    vector<int> type;
    getDataTypeArray(dataset,type);     
    
    int fieldcount = dataset->fieldCount();    
    DataSet::iterator it;
    char linedata[4096];
    int r_count = 0;
    while(true) 
    {       
    	  if(r_count >= blok_count) break;    	  
    	  if(!fgets(linedata, sizeof(linedata),  fp)) {
    	  	break;
    	  }        
        //cout <<"读到一行内容:"<<linedata << endl;
        r_count ++;
        it = dataset->pushBack();
        FP_BEGIN(SSL);
        char* p = linedata;
        char* fielddata = p;
        int i = 0;
        for(int end=0; !end && i < fieldcount;) {
        	if(*p == split || (end = (*p == '\0' || *p == '\n'))) {
        	  *p = '\0';
        	  
        	  put2DataSet(it,i,type[i], fielddata);   
        	  //cout <<"fieldata:"<<i<< ",............."<<fielddata<<endl;
        	  p++;      	          	  
        	  fielddata = p;
			      i++;
        	} else {
        		p++;
        	}
        }
        FP_END(SSL); 
        if(i != fieldcount){ 
        	//	printf("文件名:%s 文件列:%d 定义列:%d",m_name.c_str(),i,fieldcount);            
            THROW_C(ExecutionException,i,"文件数据与格式定义不符");
        }
    } 
    
    FL_TRACE_P1("文件读完毕内容:%d",r_count);
    //cout << *dataset << endl;
    return (r_count > 0)? r_count :-1 ;
    
//    int r_count = 0;
//    int fieldcount = dataset->fieldCount();
//    string  fielddata;  
//    DataSet::iterator it;
//    int i;
//    while(!fin.eof() && !fin.fail() ) 
//    {       
//    	if(r_count >= blok_count) break;
//    	string  linedata;
//        getline(fin, linedata, delimiter );
//        if(linedata.empty()) break;
//        //cout <<"读到一行内容:"<<linedata << endl;
//        r_count ++;
//        i = 0;
//        istringstream ssl(linedata);
//        it = dataset->pushBack();
//        
//        FP_BEGIN(SSL);
//        while(!ssl.eof() && i < fieldcount){
//            getline( ssl ,fielddata , split);      
//            put2DataSet(it,i,type[i], fielddata.c_str());
//            i++;    
//        } 
//        FP_END(SSL);
//        if(i != fieldcount){           
//            THROW_C(ExecutionException,i,"文件数据与格式定义不符");
//        }
//    } 
//    FL_TRACE_P1("文件读完毕内容:%d",r_count);
//    //cout << *dataset << endl;
//    return (r_count > 0)? r_count :-1 ;
}

// 写到文件流中,在外部已经确认好了,文件不会超大 
inline  int FileResource::writeDataSet(FILE* fout,PDataSet dataset,char split)
{
    if(!fout){
    	cout << "文件没有打开不可以写"<<endl;
    	return -1;
    }
     int r_count = 0;
    int fieldcount = dataset->fieldCount();
    
    DataSet::iterator it = dataset->begin();    
    ostringstream oos; 	
    while ( it!= dataset->end())
    {      
    	  oos.str("");  
        for(  int i=0; i< fieldcount;i++){
           oos << (*it)[i];
           if (i < fieldcount-1)
           oos << split;
        }       
        oos << endl;
        fwrite(oos.str().c_str(), oos.str().size(), 1, fout);
        it++;
        r_count ++;
    }  
     
    return (r_count <= 0)? -1 : r_count;
}

//int FileResource::listDirectory(string & m_dir , vector<string> &ftplist,string & name){
 //       int errCode ;
//	errCode = ftp->listDirectory(m_dir, ftplist ,"");
//        cout<<"ftplist"<<ftplist.size()<<endl;
//	
//}
//把临时文件换成正式文件名 marker 
void FileResource::milestone(Context& ctx){
		FP_BEGIN(milestone);
    if(isclosed)
    { 
       open(ctx);
    } 
    int errCode = 0;
 
    File fileR(tempfolder,m_tempname);
    
    //cout << "FileR:" << tempfolder << m_tempname << endl;
    
    if(this->getFileAppend() == 1)
    {
    		string appendfilename = m_tempname + ".append";  
        cout<<"fffff:"<<fileR.length()<<"ddd"<<tempfolder+m_tempname<<endl;
    		LOG_TRACE_P2(ctx.getLogger() , "需要Append文件,把前一个文件读下来 FTP ftp->getFile:%s-->%s",this->m_name.c_str() ,appendfilename.c_str()); 	
    		std::vector<string> ftplist;
    	  	errCode = ftp->listDirectory(m_dir, ftplist , this->m_name);  /* 检查最终目标文件是否存在?(.AVL) */
    		//cout << "check:" << m_dir << this->m_name << endl;
    		//用file重试一把
    		if(ftplist.size()==0){
		    	 File filetry(m_dir,this->m_name); 
		    	 if (filetry.exists()) 
		    		  ftplist.push_back(this->m_name);
    		} 
    		if(ftplist.size()==0){
    			int i_ = 0 ;
            	while(i_<=100){
	        		ftp->disconnect();
	        		//FSystemAgent::getInstance( this->h.isLocalHost ? "FILE" : "FTP");//zzc 
	        		//ftp->initialize(); 
	        		ftp->connect(this->h.ip, "21" , this->h.username, this->h.password);
	        		errCode = ftp->listDirectory(m_dir, ftplist , this->m_name); 
            	  	if (ftplist.size()==0){ 	
            	  		i_++;
                	}
                	else{
                	    break;
                	}
            	}    
    		   
    		}
    	  if(ftplist.size()==0){
    	    	LOG_TRACE_P2(ctx.getLogger() , "设置为Append文件,但目标文件还没有产生:%s/%s",this->m_dir.c_str(),m_name.c_str()); 	
    		}
    		else{
	    		 errCode = ftp->getFile(m_dir + this->m_name,tempfolder + appendfilename); /*取下最终目标文件，命名为/temp/.append */   		
	    		 if(errCode == 0 )
	    		 {    		
			    			File appendfile(tempfolder,appendfilename); 
			    			LOG_TRACE_P1(ctx.getLogger() , "文件Append前已有文件大小:%d",appendfile.length()); 	
			    			if(appendfile.length()==0)
			    			{
			    			//原文件为空
			    			} 
		    	//----zzc--add--for --BI---9-6--------------------------
	    			  	if(appendfile.length()+fileR.length()>=2147483648){
	    			  	  string sa = this->m_name;
		    			  string sa_after;
		    			  string sa_before;
		    			  sa_after = sa.substr(18);
		    			  sa_before = sa.substr(0,14); 
		    			  char tchar[4];
	    				  File file_name(m_dir,this->m_name);
	    				  int i = 1;
	    				  for( ;;i++){
                                sprintf(tchar,"%d",i);
                                string ss = sa_before + "000" + tchar + sa_after;	    				  	
	    				      	int bb = ftp->listDirectory(m_dir, ftplist , ss);
	    				      	if( bb < 0){
	    				      		string errInfo;
						            ftp->getErrInfo(errCode,errInfo);
						            THROW_C(DefinitionException,errCode,errInfo);
						        }
	    				      	if(ftplist.size()==0)break;    
	    				  }
	    				  string ts_name;
	    				  ts_name.clear();
	    				  if(i<=9){
	    				  	 ts_name = sa_before + "000" + tchar + sa_after;
	    				  }
	    				  else{
	    				  	ts_name = sa_before + "00" + tchar + sa_after;
	    				  }
    					  ftp->getFile(m_dir + this->m_name,m_dir + ts_name);
    					  if( errCode < 0){
									 string errInfo; 
									 ftp->getErrInfo(errCode,errInfo);
									 THROW_C(DefinitionException,errCode,errInfo);
                }       
    					  LOG_TRACE_P1(ctx.getLogger() , "文件追加超过规定值，重命名为:%s", ts_name.c_str());
    					  //cout<<"ts_name"<<ts_name<<endl;
    					  file_name.deleteFile();
    					  appendfile.deleteFile();
	    					  
	    				}	
		    	//--------zzc----end-------------------------------
		    	
		    			else 
		    			{	    		  
		    		   	LOG_TRACE_P1(ctx.getLogger() , "文件Append时tmp文件大小:%d",fileR.length()); 
		    		   	if(fileR.length()==0)
		    		   	{ 
		    		   	  appendfile.renameTo(fileR);		    		   		 
		    		   	}
		    		   	else
		    		   	{
                 		 int a = appendfile.concateFile2(fileR);	//--zzc---change---- 	    		   
 		    		     //int a = fileR.concateFile2(appendfile);
                  		if (a ==-1)
                 		 {
			      			cout<<"can not concateFile"<<endl;
			    			//cout << "------------error no = " << errno << endl;
			      		}	
			      		fileR.deleteFile();  
			      		appendfile.renameTo(fileR);  	

			              /*
					      appendfile.deleteFile();		    		            
				    	  File atmp(tempfolder,appendfilename+".tmp");       
					      int a = atmp.concateFile(appendfile,fileR);
						  if (a ==-1){
					      	cout<<"can not concateFile"<<endl;		
					    	ASSERT(a == 1);
					      }
					      if(a==0){
					    	appendfile.deleteFile();
					    	//fileR.deleteFile();
					    	int b =atmp.renameTo(fileR);
					    	if(b ==-1){
					    	  cout<<"renameTO defult"<<endl;
					    	  ASSERT(b == 1);
					    	}
					       }	
				    		*/
		    		   		} 
		    		      		   
		    			}
		    		LOG_TRACE_P1(ctx.getLogger() , "文件Append后文件大小:%d",fileR.length()); 	
	    		} 
	    		else 
	    		{
	    		    LOG_TRACE_P2(ctx.getLogger() , "设置为Append文件,但目标文件还没有产生:%s/%s",this->m_dir.c_str(),m_name.c_str()); 	
                            string errInfo;         
	    		    THROW_C(DefinitionException,errCode,errInfo);
	    		}
	    	}
    }   
    
    if(iState > NONE){//是DataSet写操作的里程碑
    	ASSERT( ftp != 0);
    	string tmf = tempfolder + m_tempname;
      cout << "tmpf:" << tmf << endl;
      	
     	string dst_tmp = m_dir + m_tempname;  
    	
    	LOG_TRACE_P2(ctx.getLogger() , "把文件从本地写好的文件上传到正式路径下的临时文件名 \n FTP ftp->putFile:%s-->%s",tmf.c_str() ,dst_tmp.c_str());
    	
    	errCode = ftp->putFile( tmf ,dst_tmp ); 
      if(errCode ==0)
      {        	      	
            errCode = fileR.deleteFile();          
            cout << "deleteFile:FileR" << fileR.getPath() << endl; 
            LOG_TRACE_P2(ctx.getLogger(),"删除本地临时文件errCode:%d,tmpname:%s",errCode,tmf.c_str());
			}
    } 
    if(errCode==0)
    {    	    	
	    errCode = ftp->rename(m_dir + this->m_tempname,m_dir + this->m_name);
	    cout << "把临时文件换成正式文件名 ftp->rename:"<< m_dir + this->m_tempname<<" --> "<< m_dir + this->m_name<<endl;
	    //Yuaneg 2005-9-5  删除rename不成功的临时文件
	    fileR.deleteFile();
	    //rename不成功的话删除临时文件
	    if (errCode < 0){
         File fileD(m_dir,m_tempname);
         fileD.deleteFile();
      }
    }
    if( errCode < 0)
    {
        int errCode;
        string errInfo;
        
        ftp->getErrInfo(errCode,errInfo);
        LOG_TRACE_P4(ctx.getLogger(),"FTP exception:%d,%s\n%d-%s",errCode,errInfo.c_str(), errno, strerror(errno));
        THROW_C(DefinitionException,errCode,errInfo);
    }
    FP_END(milestone);
    
}
//移除文件
void FileResource::moveResource(const string & path, bool isdele){
     if(isclosed == false){
     	int errCode;
        string errInfo; 
        if(isdele){
            //当前目录已经转过了           
            int r = ftp->delFile(m_dir + this->m_name);
            if( r < 0){
            	int i_ = 0 ;
            	while(i_<=100){
	        		ftp->disconnect();
	        		//FSystemAgent::getInstance( this->h.isLocalHost ? "FILE" : "FTP");//zzc 
	        		//ftp->initialize(); 
	        		ftp->connect(this->h.ip, "21" , this->h.username, this->h.password);
	        		//2005-9-14 Yuaneg 删除文件失败重试
            	 	int r = ftp->delFile(m_dir + this->m_name);
                    
            	  	if (r < 0){ 	
            	  		i_++;
                	}
                	else{
                	    break;
                	}
            	}    
            	
            	if (i_ == 100){
	                ftp->getErrInfo(errCode,errInfo);
	                g_logger.alert(Grade::WARNING,1033,"[%s]删除文件exception:%s",(m_dir + this->m_name).c_str(),errInfo.c_str());
	                //LOG_TRACE_P2(ctx.getLogger() , "FTP exception:%d,%s", errCode ,errInfo.c_str());
		            THROW_C(DefinitionException,errCode,errInfo);
	            } 
            }
        } else if(!path.empty()){
            string newname(path);
            if(newname.at(newname.size()-1)=='/'){
                newname += this->m_name;
            }else {
                newname += "/" + this->m_name;
            }
            int r = ftp->rename(m_dir + this->m_name,newname);
            if( r < 0){
            	  //2005-9-14 Yuaneg 删除文件失败重试
            	  int r = ftp->rename(m_dir + this->m_name,newname);
            	  if (r <0 ){
		                ftp->getErrInfo(errCode,errInfo);
		                //LOG_TRACE_P2(ctx.getLogger() , "FTP exception:%d,%s", errCode ,errInfo.c_str());
		                g_logger.alert(Grade::WARNING,1033,"[%s]移除文件exception:%s",(m_dir + this->m_name).c_str(),errInfo.c_str());
		                THROW_C(DefinitionException,errCode,errInfo);
                }
            }
        }  else {
        	  g_logger.alert(Grade::WARNING, 1033,"[%s]备份目录设置错误exception:%s",(m_dir + this->m_name).c_str(),errInfo.c_str());
            THROW_C(DefinitionException,-1,"备份目录设置错误");
        }
     }   
}

//把格式错误的文件移走到坏文件目录中
void FileResource::moveFileForMetadataException(Context& ctx){
    string newname = getErrFolder();   
    
    string folder = getDefinition(ctx);
    
    File file(newname,folder); 
    if(!file.exists()) file.mkdir();
    newname +=folder;
    newname +="/";
    if(isclosed == false){
        if(!newname.empty()){           
            newname += this->m_name;
            //int errCode = ftp->rename(tmpdir + m_tempname,newname);//格式错误把读下来的工作目录下的文件移走
            FL_TRACE_P1("文件格式错误把远程的文件移到错误目录:%s",newname.c_str());
            int errCode = ftp->getFile(m_dir + this->m_name,newname);//格式错误把远程的文件移走 rename
            if( errCode < 0){
                string errInfo; 
                ftp->getErrInfo(errCode,errInfo);
                //LOG_TRACE_P2(ctx.getLogger() , "FTP exception:%d,%s", errCode ,errInfo.c_str());
                THROW_C(DefinitionException,errCode,errInfo);
            }
        }  else {
            THROW_C(DefinitionException,-1,"转移错误文件目录设置错误");
        }
     }  
}

// 将文件资源拷贝到另一个资源上
void FileResource::copyTo(FileResource& dst,Context& ctx)
{
    FileResource& src = *this;
    //FTP - FTP 0
    //FTP - 本地 1 
    //本地 - FTP 2
    //本地 - 本地 3    
    int s = ctx.getFlowLineDao().getHost(accessId).isLocalHost;
    int d = ctx.getFlowLineDao().getHost(dst.getAccessId()).isLocalHost;  
    int mode = s*2 + d ;
    //Yuaneg 20051101 都是本地文件时用link
    if ((s+d)==2){
    	mode=3;
    }else{
    	mode=0;
    }
    string srcdir = (!src.m_dir.empty() && src.m_dir.at(src.m_dir.size()-1)=='/')? src.m_dir :(src.m_dir + "/") ;
    string dstdir = (!dst.m_dir.empty() && dst.m_dir.at(dst.m_dir.size()-1)=='/')? dst.m_dir :(dst.m_dir + "/") ;
    string srcname = src.m_name;
    string dstname = dst.m_tempname;
    string tmpdir = (!tempfolder.empty() && tempfolder.at(tempfolder.size()-1)=='/')? tempfolder :(tempfolder + "/") ;
    src.open(ctx);
    dst.open(ctx);
    Host srch= ctx.getFlowLineDao().getHost(src.getAccessId());
    Host dsth= ctx.getFlowLineDao().getHost(dst.getAccessId());
    LOG_TRACE_P4(ctx.getLogger() , "FTP 连接来源主机:%d,IP:%s,%s@%s", src.getAccessId() ,srch.ip.c_str(),srch.username.c_str(),srch.password.c_str()); 
    LOG_TRACE_P4(ctx.getLogger() , "FTP 连接目标主机:%d,IP:%s,%s@%s", dst.getAccessId() ,dsth.ip.c_str(),dsth.username.c_str(),dsth.password.c_str()); 
    int r=0;
    string srcfn = srcdir + srcname;
    string dstfn = dstdir + dstname;
    switch(mode){
        case 0://FTP - FTP
        {
          cout << "//FTP - FTP " << r <<endl; 
          //Yuaneg 20050905 增加判断源文件与目标文件大小是否一致,如不一致则抛出异常
          unsigned long srclen=0;
          if (src.ftp->getFileInfo(srcfn,srclen)!=0){ 
          	 srclen=0;   	
          }
          r = src.ftp->getFile(srcfn, tmpdir + srcname); 
          if(r == 0){         
              r = dst.ftp->putFile(tmpdir + srcname, dstfn);
              if (r<0){
              		string errInfo; 
                  ftp->getErrInfo(r , errInfo);
              	  g_logger.alert(Grade::WARNING,1033,"[%s]copy文件异常文件[%s]%s到%s",ctx.getDefinition().getName().c_str(),errInfo.c_str(),srcfn.c_str(),dstfn.c_str());
              	  File file(tmpdir,srcname);
              	  file.deleteFile();
              	  THROW_C(ExecutionException,-1,"copy文件异常");
              }
              unsigned long dstclen=0;
              if (dst.ftp->getFileInfo(dstfn,dstclen)!=0) dstclen=0;              
              if((srclen == dstclen)){
	              File file(tmpdir,srcname); 
	              LOG_TRACE_P2(ctx.getLogger() , "删除临时文件:%s/%s", tmpdir.c_str() ,srcname.c_str()); 
	              file.deleteFile();
	            }
	            else{
	               string errInfo; 
                ftp->getErrInfo(r , errInfo);
                //删除临时文件
                File file(tmpdir,srcname); 
                file.deleteFile();
                g_logger.alert(Grade::WARNING,1033,"[%s]copy源文件与目标文件大小不一致%s到%s",ctx.getDefinition().getName().c_str(),srcfn.c_str(),dstfn.c_str());
                LOG_TRACE_P2(ctx.getLogger() , "源文件与目标文件大小不一致:%d,%s", r ,errInfo.c_str());
                //删除没有成功的目标文件
                if (r==0){
                	File filed(dstdir,dstname)	; 
                	filed.deleteFile();              
                	THROW_C(ExecutionException,-1,"源文件与目标文件大小不一致");
                }
	            }
          } else {
             cout << "//FTP - FTP 失败：" <<endl;      
          }        
          break; 
        }    
        case 1://FTP - 本地    
          cout << "//FTP - 本地 " << r <<endl;     
          r = src.ftp->getFile(srcfn, dstfn);          
          break;       
        case 2://本地 - FTP
          cout << "//本地 - FTP " << r <<endl; 
          r = dst.ftp->putFile(srcfn, dstfn); 
          break;      
        case 3:        
        default: 
          cout << "//本地 - 本地 " << r <<srcfn << "-->"<< dstfn <<endl;  
          if (::link(srcfn.c_str(),dstfn.c_str()) != 0){
          		string errInfo; 
          		ftp->getErrInfo(r , errInfo);
         			g_logger.alert(Grade::WARNING,1033,"[%s]link文件异常文件[%s]%s到%s",ctx.getDefinition().getName().c_str(),errInfo.c_str(),srcfn.c_str(),dstfn.c_str());
          		THROW_C(ExecutionException,-1,"link文件异常");         		
          }
          //r = src.ftp->getFile(srcfn,dstfn);   
          break; 
     }
     if( r != 0){       
        string errInfo; 
        ftp->getErrInfo(r , errInfo);
				g_logger.alert(Grade::WARNING,1033,"[%s]FTP文件传输exception:%s",ctx.getDefinition().getName().c_str(),errInfo.c_str());
        LOG_TRACE_P2(ctx.getLogger() , "FTP文件传输exception:%d,%s", r ,errInfo.c_str());
        THROW_C(ExecutionException, r, errInfo);
     }
        
    src.close();
    dst.close();   
}

};


