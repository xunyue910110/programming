#ifndef _FILE_REPOSITORY_PARAMETER_H_
#define _FILE_REPOSITORY_PARAMETER_H_

#include "config-all.h"
#include "flowline/adapter/ResourceRepositoryAdapter.h"
#include "flowline/Resource.h"
#include "flowline/Context.h"
#include "flowline/FlowLineException.h"
#include "flowline/flbase.h"
#include "flowline/adapter/Adapter.h"
#include "flowline/Element.h"

#include "fileadapter/Util.h"

#include <string>
#include <vector>
#include <map>

USING_NAMESPACE(std);
using namespace flowline;
using namespace flowline::adapter;

namespace fileadapter{

class FileRepositoryParameter
{

public:
	FileRepositoryParameter():separator(','),fileAppend(0){
	}
	virtual ~FileRepositoryParameter(){};

protected:
    //子类需要调用一下才能有数据
    inline void parseConfigure(const string& conf){
        flowline::parseParameter(conf,m_paremeter);
    }
    //外部configure参数
    map <string,string> m_paremeter;

public :
   
    virtual  void initParameters(){}; //设置对象内的属性,也需要调用才行的
   
    inline string getParameter(const string& key) const{
         return flowline::getParameter(key , m_paremeter);
    }
    
    inline string getParameter(const string& key,const Element* element) const{
        if(element){
           return element->getExtendedAttribute(key);
        }
        return string();
    }

	//本类的方法
	void setDirectory(const string & dirpath){
		m_dir = dirpath;
	}
	
	void setFileNameTemp(const string & name){
		m_filenameTemplate0 = name;
	}
	
	void setFileName(const string & name){
		m_filenameTemplate1 = name;
	}
	
	void setBakDirectory(const string & dirpath){
		m_bakdir = dirpath;
	}
	
	const string & getDirectory() const{
		return m_dir;
	}
	
	const string & getFileNameTemp() const{
		return m_filenameTemplate0;
	}
	
	const string & getFileName() const {
		return m_filenameTemplate1;
	}
	
	const string & getBakDirectory() const {
		return m_bakdir;
	}

    private:
        //宏管理器
        //  DspMacroToolkit m_marcohandle;
        //string configuration;
        string m_dir;
        string m_filenameTemplate0;//临时 T${YYYYMMDD}.${SEQ}.tmp
        string m_filenameTemplate1;//正式 T${YYYYMMDD}.${SEQ}.txt
        //string 主机,用户,密码

        string m_bakdir; //备份目录

    private :
        //size_t  lm_count; //LM_COUNT;
        //size_t  batchCount; //处理批量;

        string header;
        string footer;
        int limitType;
        size_t limitRecord;
       
    public :
        enum {LIMIT_NONE,LIMIT_RECORD,LIMIT_SIZE};
        enum {NONE,MOVE,DELETE};

        void setLimitType(int type){
            limitType = type;
        }

        int getLimitType(){
            return limitType;
        }

        void setLimitRecord(size_t limit){
           limitRecord = limit;
        }

        size_t getLimitRecord(){
           return limitRecord;
        }

        void setFinishActionType(int type){
            finishaction = type;
        }

        int getFinishActionType(){
            return finishaction;
        }
		
        void setHeader(const string& strheader){
            header = strheader;
        }
        void setFooter(const string& strfooter){
            footer = strfooter;
        }
        const string& getHeader()const{
           return header;
        }
        const string& getFooter()const{
           return footer;
        }

    public :
        enum {SEPARATOR, WIDTH, LEFT, RIGHT};

        void setSeparatorType(int type){
           septype = type;
        }
        void setSeparator(char cseparator){
           separator = cseparator;
        }
        void setFillSymbol(char ch){
           fillSymbol = ch;
        }
        void setAlignType(int type){
           alignType = type;
        }
        int getSeparatorType(){
           return  septype;
        }
        char getSeparator(){
           return separator;
        }
        char getFillSymbol(){
          return fillSymbol;
        }
        int getAlignType(){
          return alignType;
        }
        void  setCycleType(char type){
           cycleType = type;
        }
        char getCycleType(){
           return cycleType;
        }
		void setAccessId(int id){
		    accessId = id;
		}
		virtual int getAccessId(){
		    return accessId;
		}
		void setTempFolder(const string& temp){
		    tempfolder = temp;
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
        int getFileAppend(){
        	return fileAppend; 
        }    
        void setFileAppend(int append){
        	fileAppend = append;
        }
        const string& getListRule(){
        	return listRule;
        }
        void setListRule(const string& rule){
           // listRule = listRule;
        	listRule = rule;//zzc
        }        
        
    private :
        string errfolder;

        int accessId; //accessId
        string tempfolder;
        int finishaction ;
        int septype;
        char separator;
        char fillSymbol;
        int alignType; //left/right
        char cycleType; //R:实时;D:日；W:周；M:月；S:季度；Y:年
        /**
           ％..％：表示当前日期，格式参考YYYYMMDDHH24MISS；
           '..'：表示对日期的操作，'D-1','M-1','Y-1'分别表示对当前日期减去一天、一月、一年；
           &..&：表示序列号的长度，3:1,3:a，分别表示序列号号的规则为001、002...和00a、00b...
           #..#：表示需要按数据切割文件，并以文件命名字段宏参数表中的映射值作为本处替换值，成为文件名的一部分
        */
        //日期(修正值).序列号.源文件名(sfn).切割
        //T%YYYYMMDD%'D-1'&3:1&#ep#.txt  T20050304.txt
        int fileAppend; //是覆盖还是叠加还是自动多对一叠加
        
        string listRule;//文件列出规则

public:
   
    enum{COVER,APPEND,AUTO_APP};//是覆盖还是叠加还是自动多对一叠加
    
	virtual void initExtendedAttribute(const Element* element){
        ASSERT(element!=0);
        string pk;//关键字
        string ps;//值
            
        pk = "dir";
        ps = getParameter(pk , element);       
        if( ps!="") this->setDirectory(ps);
        
        pk = "file";        
        ps = getParameter(pk , element);       
        if( ps!="") this->setFileName(ps);
        
        pk = "filetemp";
        ps = getParameter(pk , element);
        if( ps!="") this->setFileNameTemp(ps);
        
        pk = "bakdir";
        ps = getParameter(pk , element);
        if( ps!="") this->setBakDirectory(ps);
                
       // pk = "lm_count";
       // ps = getParameter(pk , element);
       // if( ps!="") this->setLimitCount(atoi(ps.c_str()));
        
       //pk = "bloksize";
       //ps = getParameter(pk , element);
       //if( ps!="") this->setBatchCount(atoi(ps.c_str()));
    
        pk = "header";
        ps = getParameter(pk , element);
        if( ps!="") this->setHeader(ps);
        
        pk = "footer";
        ps = getParameter(pk , element);
        if( ps!="") this->setFooter(ps);
        
        pk = "access";
        ps = getParameter(pk , element);       
        if( ps!="") this->setAccessId(atoi(ps.c_str()));
                
        int  tp;
        char pc;       
        //enum {SEPARATOR, WIDTH, LEFT, RIGHT};
        pk = "SeparatorType";
        ps = getParameter(pk , element);  
        if( ps!="") {
            tp = atoi(ps.c_str());
            if(tp < 4) this->setSeparatorType(tp);
        }
                
       //enum {NONE,MOVE,DELETE};        
        pk = "FinishActionType";
        ps = getParameter(pk , element);               
        if( ps!="") {
            tp = atoi(ps.c_str());
            if(tp < 3) this->setFinishActionType(tp);
        }
        
        pk = "SeparatorChar";
        ps = getParameter(pk , element);        
        if( ps!="") {
            //pc = ps[0];
            pc = atoi(ps.c_str());
            this->setSeparator(pc);
        } 
        
        pk = "FillSymbol";
        ps = getParameter(pk , element);        
        if( ps!="") {
            //pc = ps[0];
            pc = atoi(ps.c_str());
            this->setFillSymbol(pc);
        }
               
        pk = "AlignType";
        ps = getParameter(pk , element);        
        if( ps!="") {
            tp = atoi(ps.c_str());
            if(tp < 4) this->setAlignType(tp);
        }
        
        pk = "CycleType";
        ps = getParameter(pk , element);        
        if( ps!="") {
            pc = ps[0];
            this->setCycleType(pc);
        }
        
        pk = "LimitType";
        ps = getParameter(pk , element);        
        if( ps!="") {
            tp = atoi(ps.c_str());
            if(tp < 3) this->setLimitType(tp);
        }
        
        pk = "LimitRecord";
        ps = getParameter(pk , element);
        if( ps!="") this->setLimitRecord(atoi(ps.c_str()));
        
        pk = "ErrFolder";
        ps = getParameter(pk , element);
        if( ps!="") this->setErrFolder(ps);
        
        pk = "listRule";
        ps = getParameter(pk , element);
        cout<<"-----zhouzc------listRule is : "<<ps<<endl;
        if( ps!="") this->setListRule(ps);
        
        pk = "append";
        ps = getParameter(pk , element);        
        if( ps!="") {
            tp = atoi(ps.c_str());
            if(tp < 4) this->setFileAppend(tp);
        }
	}

 };

};

#endif //_FILEREPOSITORYPARAMETER_H_
