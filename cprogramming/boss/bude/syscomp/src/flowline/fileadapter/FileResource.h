#ifndef _FILERESOURCE_H_
#define _FILERESOURCE_H_

#include "flowline/Resource.h"
#include "flowline/Context.h"
#include "flowline/DataSet.h"
#include "file/File.h"
#include "ftrans/FSystemAgent.h"
#include <iostream>
#include <fstream>
using namespace flowline;


namespace fileadapter
{
	
//标准日期时间格式 与ORACLE同
#define  STD_DATATIME_FORMAT  "YYYYMMDDHH24MISS"

class FileResource : public flowline::Resource
{
	
public:
	FileResource(const string& dir, const string& name) : ftp( 0 ), m_count( -1 ) ,m_max_count( 0 ),streamfile(0){		
		m_name = name;
		m_dir = (!dir.empty() && dir.at(dir.size()-1)=='/')? dir :(dir + "/") ;
        isclosed = true;
        accessId = -1;
        tempfolder = "./";
        errfolder = "./";
        splitchar = ',';        
        //m_max_count = 10000000;
        m_max_count = 1000000000;//zzc --- add file record datas 
        m_count = 0;
        iState = 0;
        metadata = 0;//指针为空
        bloksize = 0;
        fileAppend = 0;
        m_fileno = 1;
	};
    
    virtual ~FileResource();
 // int listDirectory(string & m_dir , vector<string> &ftplist,string & name);//zzc 
protected : 
    //TYPE与Length分开放
    void getDataTypeArray(PDataSet data, vector< int >& datatype) const;
    //TYPE与Length分开放，int 和 double的长度不是字符长度
    void getDataLengthArray(PDataSet data, vector< int >& vlen);
    
    inline void put2DataSet(DataSet::iterator it,  int i, int type,const char* str );
    /**
    * 读取数据集合
    * @param  istream   输入流 已经格式化
    * @param  dataset   输出数据集合
    * @param  max_count 读最大行数
    * @param  split     行内字段分割符号
    * @param  delimiter 行结尾符号
    */
    inline  int readDataSet(FILE* fin,PDataSet dataset,  int max_count,char split);
   
    // 写到文件流中,在外部已经确认好了,文件不会超大 
    inline  int writeDataSet(FILE* fout,PDataSet dataset,char split);

    /****/
    virtual void moveFileForMetadataException(Context& ctx);
    
    inline const char* dateFormated(int fieldidx,const char* str,bool isWrite);
public :
    /**
     * 知道是NULL, 表示读完毕
     */
    virtual PDataSet read(Context& ctx);
    /**
     * 输出到DataSet
     */
	virtual void write(Context& ctx, PDataSet data); //0 , 1 满
    
    /*
     * 取得处理的记录数, 如果此资源没有经过读写可以直接返回-1
     */
    virtual int getRecordCount() const{
        return m_count;
    }
    virtual int getMaxRecordCount() const{
        return  m_max_count;
    }
      
    /**
     * 初始化的时候并不开资源,只有要超作的时候才打开他.
     */
    virtual void open(Context& ctx);
    /**
     * 关闭资源,一但关闭不允许再操作资源
     */
    virtual void close();
    /**
     * 是否已经被关闭了
     */
    virtual bool isCloseed(){
       return isclosed;
    }
    /**
     * 将资源拷贝到另一个资源上
     */     
    virtual void copyTo(FileResource& dst,Context& ctx);
    
    /**
     * 把临时文件换成正式文件名 marker
     */
    virtual void milestone(Context& ctx);
    
    /**
     * 删除掉资源,并关闭
     */
    virtual void moveResource(const string& path = "" , bool isdele = false);
    /**
     * 得到有结构的DataSet
     */
    PDataSet getMetadataDataSet(Context& ctx);   
    
    void setMetadata(const Metadata* meta){
        metadata = meta;
    }
    
    void setMaxRecordCount(int count){
        m_max_count = count;
    }
    /**
     * 文件名称,不含路径的真实名称 
     */
    const string& getName() const{
        return m_name;
    }
	const string & getTempName() const {
        return m_tempname;
    }

	void setTempName(const string& tmpname){
        m_tempname = tmpname;
    }
    
    const string& getDirectory(){
        return m_dir;
    }
           
    void setAccessId(int id){
        accessId = id;
    }
    int getAccessId(){
        return accessId;
    }
    void setTempFolder(const string& temp){        
        tempfolder = (!temp.empty() && temp.at(temp.size()-1)=='/')? temp :(temp + "/") ;
    }
    
    const string& getTempFolder() const{      
       return tempfolder;
    }
    void setErrFolder(const string& temp){
        errfolder = (!temp.empty() && temp.at(temp.size()-1)=='/')? temp :(temp + "/") ;
    }
    const string& getErrFolder() const{
        return errfolder;
    }
    void setSplitChar(char schar ){
        splitchar = schar;
    }
    char getSplitChar(){
        return splitchar;
    }
	int getFileAppend(){
        return fileAppend; 
    }    
    void setFileAppend(int append){
        fileAppend = append;
    }
    void setBlokSize(size_t size){
        bloksize = size;	
    }
    size_t getBlokSize(){
        return bloksize; 	
    }
    void setHeader(const string& temp){
        m_header = temp;
    }
    const string& getHeader() const{
        return m_header;
    } 
    void setFooter(const string& temp){
        m_footer = temp;
    }
    const string& getFooter() const{
        return m_footer;
    }     
    void printResourceInfo(){
        cout << "-----------FileResourceInfo----------" << endl;
        cout << "-- accessId:" << accessId << endl;
        cout << "-- m_dir:" << m_dir << endl;        
        cout << "-- m_name:" << m_name << endl;
        cout << "-- m_tempname:" << m_tempname << endl;       
        cout << "-- tempfolder:" << tempfolder << endl;
        cout << "-- errfolder:" << errfolder << endl;
        cout << "-- splitchar:" << splitchar << endl;
        cout << "-- bloksize:" << bloksize << endl;
        cout << "-- m_count:" << m_count << endl;
        cout << "-- m_max_count:" << m_max_count << endl;
        cout << "-- iState:" << iState << endl;
        cout << "-- fileAppend:" << fileAppend << endl;        
        cout << "-----------FileResourceInfo----------" << endl;
    }
    string getDefinition(Context ctx){
    	 Definition& def = ctx.getDefinition();    	
    	 return def.getName();
    }
private :
	 
	  size_t bloksize ;
      string errfolder;//源头文件名//moveResource(就行了);
      string m_dir;
      string tempfolder;//临时工作目录可写   
      string m_tempname;
      string m_header;
      string m_footer;
	  string m_name;
	  char splitchar; //分割符    
      int accessId;      
      int m_count;
      int m_max_count;
      FSystemAgent * ftp;
      const Metadata* metadata;  
      //int m_definition_id;
      bool isclosed;//记录的是ftp的状态
      int iState;//记录读写文件状态
      //没有读过 = true;不能按照读过多少条来判断
      enum {NONE = 0,
      	BEFORE_READ,
      	BEFORE_WRITE,
      	ON_READ,
      	ON_WRITE,
      	ALL_CLOSE
      };
      int fileAppend;
      enum {NONETYPE = 0,METADATA_DATETIME = -1 };
      Host h;//zzc-add-9-19
      FILE* streamfile;
      int m_fileno; //记录文件序号(分割文件用)
};
		
};

#endif //_FILERESOURCE_H_


