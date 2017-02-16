// ############################################
// Source file : Parameter.cpp
// Version     : 2.0
// Language    : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20050609
// Copyright(C): 2005 by Yang Zhang, Linkage.
// ############################################

#include <stdio.h>

#include <unistd.h> 


#include "util/Util.h"
#include "Parameter.h"   
#include "QueryMessage.h"
#include "QueryUseExpression.h"

// 2005.6.9 重大修改，现在参数更新不影响参数查询。使用了参数切换的方法，
// 保证了查询、更新可以同时进行。限制：参数更新需要额外内存，总内存使用不能超过4G。
// 除非使用64位编译。

//##ModelId=3F72A06100B7
Parameter::Parameter()
{   
    m_lastUpdateTimeStamp 	= "";        
    m_lastUpdateTime        = 0;  
    
    m_needUpdata         	= false;
    m_parameterLog          = ParameterLog::getInstance();  	
    
    m_parameterBuffer		= 0;	
    m_parameterCdrPool		= 0;	
    m_dataSource=NULL;
}

//##ModelId=3F72A066024F
Parameter::~Parameter()
{  
  //1.
    int size = m_parameterIndex.size(); 
    for(int i=0;i<size;i++)
    {   
        if(m_parameterIndex[i]!=0)
            delete m_parameterIndex[i];
        if(m_parameterIndexTmp[i]!=0)   //added by zhanglong 2006-4-4 15:13
            delete m_parameterIndexTmp[i];    
    }
    //2.
    delete m_parameterCdrPool; 
    delete m_parameterBuffer;

    //add by gaojf 2006-6-7 14:17 
    if(m_dataSource!=NULL)
      delete m_dataSource; 
    m_dataSource = NULL;
}

//changed by gaojf 2006-6-7 14:29
//##ModelId=3F72904803D3
/*
void Parameter::setDataSource(DataSource *datasource)
{
  m_dataSource = datasource;
}
*/
void Parameter::setDataSource(DataSource *datasource,int sourceType)
{  
    //m_dataSource = datasource;
    
    if(sourceType == 0)
    {
      if(m_dataSource != NULL) delete m_dataSource;
      m_dataSource = new  DbDataSource(*((DbDataSource*)datasource));
    }else //sourceType == 1
    {
      if(m_dataSource != NULL) delete m_dataSource;
      m_dataSource = new  FileDataSource(*((FileDataSource*)datasource));
    }
}

//##ModelId=3F729F7E0129
void Parameter::sort()
{
}

//##ModelId=3F729F8E0349
bool Parameter::getParameter(vector <string> *record)
{  
    return true;
}

//##ModelId=3F74E5930151 param_1 有关哪张表,字段等 ;param_2 有关按什么排序
bool Parameter::initialize(const vector<string> param_1, const vector<string> param_2)
{    
    int i,j,size;
    int fieldId; 
    ParameterIndex *parameterIndex; 
    ParameterIndex *parameterIndexTmp;//added by zhanglong 2006-4-4 14:19    
    
    vector <string> key_1,key_2,fieldName;
   
    if(param_1.size()!=6)
        return false;
    
    // --------- 以下使用param_1初始化的语句的次序不能修改 -------------
    j        = 0;
    m_name     = lrtrim(param_1[j++].c_str());              		// 初始化name (如果是数据库，则为select语句；如果是文件，则为文件名称。)    
    split(";",param_1[j++].c_str(),false,fieldName);             	// 初始化fields_name
    m_parameterLog->setMessage(param_1[j],1);
    m_parameterLog->setMessage("\n",0);
    
    split(",",param_1[j++].c_str(),false,m_tableName);              // 初始化table_name         
    m_bufferUnit	= atoi(param_1[j++].c_str());					// 初始化buffer_unit  
    m_updateIntervalTime  = atoi(param_1[j++].c_str())*60;         	// 初始化updata_time,单位:秒 
    m_lastUpdateTimeStamp = param_1[j++];  
    // --------- 以上使用param_1初始化的语句的次序不能修改 -------------
        
    if( m_parameterLog->m_debug>=2) 
    {   
        m_parameterLog->setMessage("m_name="+m_name+"\n",0); 
        m_parameterLog->setMessage("m_fieldsName=",0);
        for(i=0;i<fieldName.size();i++)
        {
            m_parameterLog->setMessage(fieldName[i]+";",0);
        }
        m_parameterLog->setMessage("\n",0); 
    }  
//------------------------------------------------------------------------------ 

	// 初始化字段列表，例：imsi,1,%15s   
	//	字段列表			由三部分组成：1. 字段名 2. 0/1(不压缩/压缩) 3. format(见下一注释)
	//					例：imsi,1,%15s ; channel_no,0,%2d
	//	foramt			对齐方式(使用c语言printf函数的格式串)
	for(i=0;i<fieldName.size();i++) 
	{	
	 	split(",",fieldName[i].c_str(),false,key_1); 
	 	m_fieldsName.push_back(key_1[0]);
	 	m_format.push_back(key_1[2]);
	 	if( key_1[1] == "1" ) 
	 		m_isCompress.push_back(true);
	 	else
	 		m_isCompress.push_back(false);
	}
    // 初始化索引，例：index_0    = imsi,0 ; begin_datetime,0   # 0：升序，1:降序    
    for(i=0;i<param_2.size();i++) 
    {
        parameterIndex = new ParameterIndex;  
        parameterIndexTmp = new ParameterIndex;    
        
        parameterIndex->setIsCompress(&m_isCompress);
        parameterIndexTmp->setIsCompress(&m_isCompress);
        
        split(";",param_2[i].c_str(),false,key_1); //第一层key 如:    key_1[0]=    imsi,0
        size = key_1.size(); 
        if( m_parameterLog->m_debug>=2) {  
            m_parameterLog->setMessage("key = ",0);
        } 
        
        for(int j=0;j<size;j++) 
        {
            split(",",key_1[j].c_str(),false,key_2);//第二层key 如: key_2[0]=imsi ;key_2[1]=0

            //由字段名得到其在 表的配置项fields_name 中的坐标
            if( ( fieldId = getFieldID(key_2[0]) ) < 0 ) 
                return false; 
            //cout<<" key_2[0]="<<key_2[0]<<endl;
            //cout<<" key_2[1]="<<key_2[1]<<endl;
            //cout<<"fieldId="<<fieldId<<endl;
            parameterIndex->initialize(key_2[0],fieldId,key_2[1]); 
            parameterIndexTmp->initialize(key_2[0],fieldId,key_2[1]);         
            
            if( m_parameterLog->m_debug>=2) {  
                m_parameterLog->setMessage(key_2[0]+","+key_2[1]+";",0);
            }       
        }
          
        if( m_parameterLog->m_debug>=2) {  
            m_parameterLog->setMessage("\n",0);
        }
        m_parameterIndex.push_back( parameterIndex );  //有问题,应该在前面  ????? 
        m_parameterIndexTmp.push_back(parameterIndexTmp);//added by zhanglong 2006-4-4 15:08
    } 
    
    if( m_parameterLog->m_debug>=2) {   
        m_parameterLog->setMessage("\n",0);     
    }  
        
    return true;
}//initialize

//##ModelId=3F75173F0051
//由字段名得到其在 表的配置项fields_name 中的坐标
int Parameter::getFieldID(const string &name)
{     
    int i;
    
    for(i=0;i<m_fieldsName.size();i++)
    {
        if( m_fieldsName[i].compare(name)==0)
            return i;
    } 
    
    return -1;
}

                                                                               
//##ModelId=3F7542DF01E1 一张表读自已的参数
bool Parameter::getData()
{   
    int i;    
    long sz; 
    char tmp[80]; 
    time_t nowtime;
    int fields,size;     
    vector<string> lines,desLines;  
        
    m_parameterBufferTmp	= new ParameterBuffer; 
    m_parameterCdrPoolTmp	= new ParameterPool< ParameterCdr > ;   
    
    m_parameterBufferTmp->setBufferUnit( m_bufferUnit ); 	
     
     //如为文件,则打开并返回记录数
    if( (sz=m_dataSource->reset(m_name)) < 0 )  //如果是数据库,则为select语句；如果是文件,则为文件名称    
    { 
        return false;   
    } 
    m_count = sz;
    
    if(m_count == 0)  {   
        time(&nowtime);
        m_lastUpdateTime=nowtime;             // 改写最后更新时间 
        return true;   
    } 
    
    m_parameterCdrPoolTmp->resize(sz);   
    
    while( (fields=m_dataSource->getRecord(lines)) > 0 ) 
    { 
        desLines.clear();  
        for(i=0;i<fields;i++)
        {   
            format(tmp,lines[i].c_str(),m_format[i],m_isCompress[i]);  
            desLines.push_back(tmp);       
        }      
          
        if(!insert(desLines)) { //具体子类确认方法 添加到m_parameterCdr列表中
            if( m_parameterLog->m_debug>=0) { 
                m_parameterLog->setMessage("内存分配失败\n",1);  
            }
            return false;                
        }  
    }  
    if(fields<0) {
        if( m_parameterLog->m_debug>=0) { 
            m_parameterLog->setMessage("获取参数记录失败\n",1);  
        }    
        return false;
    }  
    
    if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("资料已经读入内存",1);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    }   
    

	if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("开始重建索引.......",1);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    }
    //得到索引
    if( !getDataIndex() ) 
    { 
        return false;  
    }
    if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("索引重建完成",1);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    }
    
    time(&nowtime);
    m_lastUpdateTime=nowtime;             // 改写最后更新时间  
    
    
    //ParameterInterface::m_systemState    = ParameterInterface::UPDATA_STATE ;//added by zhanglong 2006-4-2 22:34
    //changed by gaojf 2006-4-7 
    if(ParameterInterface::m_systemState == ParameterInterface::READY_STATE)
    {
    	ParameterInterface::m_systemState    = ParameterInterface::UPDATA_STATE ;
    }		
    //sleep(10);  
    //此处需要和访问进程握手，且增加超时机制 10分钟
    time_t beginWaitTime;
    beginWaitTime = nowtime;
   
    while(ParameterInterface::m_cli_FindNum > 0)
    {	//此时访问线程正在访问，需要等待访问线程该次查询完毕
    	//如果等待超时，则表示有访问线程出现异常
    	if(beginWaitTime < nowtime - 600) return false;
    	usleep(10);
    	time(&nowtime);
    }
    if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("开始切换资料和索引",1);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    }
    
    toSwitch();

	 if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("完成切换资料和索引",1);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    }
    //changed by gaojf 2006-4-7 
    if(ParameterInterface::m_systemState == ParameterInterface::UPDATA_STATE)
    {
    	ParameterInterface::m_systemState    = ParameterInterface::READY_STATE; 
    }		
	//ParameterInterface::m_systemState    = ParameterInterface::READY_STATE; //added by zhanglong 2006-4-2 22:35
    //cout<<"初始化成功!"<<endl;

    return true;     
}//getData 

//##ModelId=3FD3E372009B
bool Parameter::upData()
{   
    long index_size;
    
    if( !m_needUpdata )	// 不需要更新
        return true;         
                 
    m_needUpdata	= false; // 恢复服务
    
    getData();        
             
    return true;
}

void Parameter::toSwitch()	// 切换
{
	   //更新 数据及索引
    ParameterBuffer * parameterBuffer;
    ParameterPool< ParameterCdr > * parameterCdrPool;
        
    parameterBuffer 		= m_parameterBuffer;
    parameterCdrPool 		= m_parameterCdrPool;
                        	
    m_parameterBuffer		= m_parameterBufferTmp;
    m_parameterCdrPool		= m_parameterCdrPoolTmp;
    
    //切换索引 added by zhanglong 2006-4-4 14:31
	m_parameterIndex.swap(m_parameterIndexTmp);
	
	m_needUpdata 			= false; 
	 
	//added by zhanglong 2006-4-4 14:50
	long index_size = m_parameterIndexTmp.size();
	for(long j=0;j<index_size;j++) 
    {
    	m_parameterIndexTmp[j]->clear();   
    }
	
	m_parameterBufferTmp	= 0; 
	m_parameterCdrPoolTmp	= 0;
	
	if( parameterBuffer > 0) {      
    	parameterBuffer->clear();
    	delete parameterBuffer;
    	delete parameterCdrPool; 
    }

}


//##ModelId=3F78E81B00E9
bool Parameter::getDataIndex()
{
    int i,j;  
    long cdr_size,index_size;  
    
    cdr_size   = m_parameterCdrPoolTmp->size();
    
    index_size = m_parameterIndexTmp.size();//一张表可能有几个索引，每个索引可能有几个字段

    m_needUpdata 			= true; 	// 暂停服务      
    for(j=0;j<index_size;j++) 
    {
    	m_parameterIndexTmp[j]->clear();   
        m_parameterIndexTmp[j]->resize(cdr_size);    
    } 
    //(1)设置每条记录的 每种索引
    for(i=0;i<cdr_size;i++) 
    {
        for(j=0;j<index_size;j++)  
        {                                  
            m_parameterIndexTmp[j]->setKey( m_parameterCdrPoolTmp->at(i),&m_isCompress );               
        } 
    }  
    //(2) 每种索引排序
    for(j=0;j<index_size;j++) {
        m_parameterIndexTmp[j]->sort();    //这种索引全部排序
    }  
    if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage(m_parameterBufferTmp->getMessage(i),0);  // 输出缓存信息
        m_parameterLog->setMessage("\n",0); 
    } 
    return true;
}//getDataIndex

//##ModelId=3F8376380330
long Parameter::capacity()
{   
    return m_parameterCdrPool->capacity();
}

//##ModelId=3FB83CF402A4 fieldId 
//fieldId:一个query 中配置的字段中的第一个 在上面表中配置字段的 位置
int Parameter::find(const vector <int> &inFieldList, vector<const ParameterCdr *> &result)
{    
    int i,size;  
    char desTmp[100];  
    vector <Index> index;
    vector<string> desFields;
    QueryMessage *queryMessage; 
    ParameterIndex *parameterIndex=0;
    
    //while(m_needUpdata)//modified by zhanglong 2006-4-3 19:15
    //deleted by zhanglong 2006-5-16 16:27
    //while(ParameterInterface::m_systemState != ParameterInterface::READY_STATE)
    //{
    //	sleep(1);
    //}

    // 得到当前使用的索引对象的指针
    size = m_parameterIndex.size();
    for(i=0;i<size;i++) 
    {           
    	//inFieldList[0] 是这个query 中配置的字段中的第一个 在上面表中配置字段的位置
        if( m_parameterIndex[i]->getFieldID()==inFieldList[0] ) 
        {
            parameterIndex = m_parameterIndex[i];
            break;
        }
    }   
    
    if(parameterIndex==0)
    {
    	  cout<<" no parameterIndex find "<<endl;
        return 0;   
    }   
    
    queryMessage = 0;    
    result.clear();
    if( (queryMessage = (QueryMessage *)pthread_getspecific(rl_key)) == NULL ) 
    {
        if( m_parameterLog->m_debug>=0) {                    
            m_parameterLog->setMessage("不能通过rl_key得到线程变量\n",1);  
        }            
        return -1;    
    }
           
    //把 客户端传进来的 需查询的内容 传进去     
    size = parameterIndex->seach(index,queryMessage->m_queryUseExpression); 
     
    //先找到索引集合,index为输出参数
    if( ( size ) <= 0 )  {
        size = 0;
    } else {
    //再找到 记录集合
        for(i=0;i<size;i++)  
        {
            result.push_back(index[i].getCdr());
        }    
        
    } 
    
     
    return size;
}//find

//##ModelId=3FA08A02005D
char *Parameter::format(char *strDes, const char *strSrc, const string &format,const bool isCompress)
{   
    int a;
    double b;  
    char *fmt;
    char type;
    char tmp[1024];
    
    //      imsi,1,%15s			; begin_datetime,1,%s	; end_datetime,1,%s	
    
    fmt  = (char *)format.c_str();
    type = fmt[format.size()-1]; 
      
    if( type == 's' ) //字串 
    {
        sprintf(tmp,format.c_str(),strSrc);  
    } 
    else if( type=='f') //实数
    { 
        b = atof(strSrc);
        sprintf(tmp,format.c_str(),b);    
    } 
    else if( type=='d') 
    {
        a = atoi(strSrc);
        sprintf(tmp,format.c_str(),a);     
    } 
    else 
    {
        strcpy(tmp,strSrc);    
    } 
    
    if(isCompress) {
    	if( (a = EBCD_incode(strDes,tmp)) < 0 ) {
    		return NULL;
    	}
    } else {
    	strcpy(strDes,tmp); 
    }
    return strDes;    
}//format


//##ModelId=3FD7BCE901BC 置 本表的 m_needUpdata 标志
bool Parameter::needUpdata(const string &timeStamp, const vector<string> &tables)
{   
    int size,size1;
    time_t nowtime; 
        
    //1. 文件参数刷新判断
    if( m_name.compare(0,6,"select") != 0 ) {           
        if( m_dataSource->needUpdata(m_name) )  { 
            m_needUpdata = true;  
        } 
        return m_needUpdata;
    }    
    
    //2. 数据库参数刷新判断
    if( m_updateIntervalTime == 0 )    {              // 时间戳判断 
        if( timeStamp.size() > 0 && m_lastUpdateTimeStamp.compare(timeStamp) == 0  ) {
            return m_needUpdata;
        }
        m_lastUpdateTimeStamp = timeStamp;
    } else if( m_updateIntervalTime > 0 )    {      // 时间间隔判断
        time(&nowtime);  
        if( (nowtime - m_lastUpdateTime) < m_updateIntervalTime )  {
            return m_needUpdata;
        }                                           
    } else {
        return m_needUpdata;
    }
    
    //需要更新参数
    if( tables[0].compare("ALL") == 0 )  { 
        m_needUpdata = true; 
        return m_needUpdata;
    }  
    
    size  = tables.size(); 
    size1 = m_tableName.size(); //本类相关的表  
    for(int i=0;i<size;i++)
    {   
        for(int j=0;j<size1;j++)
        {
            if( tables[i].compare(m_tableName[j]) == 0 ) {  
                m_needUpdata = true; 
                break;
            } 
        }    
    } 
    return m_needUpdata;
}//needUpdata



//##ModelId=4028809302F9 得到本表的一些总体信息
int Parameter::getTable(string &result)
{  
	int men=0;
    char tmp[10];
    
    //tmp 放记录数
    sprintf(tmp,"%d, ",m_count);  
    if( m_count == 0 )
        strcat(tmp,"\n");
    
    //返回参数 result 内容 :(1)文件名或select语句 \n (2) 记录数 =记录数 \n (3) message \n 
    result = m_name + "\n";
    result += string("记录数 = ") + tmp ;
    if(m_count>0) {
        result +=m_parameterBuffer->getMessage(men);
    }
      
    return men;
}

//##ModelId=3F72A4B0023A
bool Parameter::insert(const vector<string> & field)
{   
	ParameterCdr parameterCdr,*p;
	
    if(field.size()<=0)
    	return true;
    
	m_parameterCdrPoolTmp->add(parameterCdr); 
	p=m_parameterCdrPoolTmp->at(m_parameterCdrPoolTmp->size()-1);	   
	p->setParameterBuffer(m_parameterBufferTmp);  
		
	if(!p->insert(field))
		return false;  	

	return true;	
}

bool Parameter::isCompressField(int fieldId)
{
	return m_isCompress[fieldId];
}

vector <bool> * Parameter::getCompressVector()
{
	return &m_isCompress;
}

void Parameter::setTableName(const string & tableName)
{
	m_tableName1 = tableName;
}

string Parameter::getTableName()
{
	return m_tableName1;
}

int Parameter::getCount()
{
	return m_parameterIndex[0]->getCount();
}

int Parameter::dump(string &result,const int &no)
{
	return m_parameterIndex[0]->dump(result,no);
} 

int Parameter::dump(string &result)
{
	return m_parameterIndex[0]->dump(result);
} 
