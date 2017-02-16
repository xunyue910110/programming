
#include "ParamManager.h"
#include "ParamContainerNext.h"
#include "ParamContainerChild.h"

#include <fstream>
#ifdef _DEBUG_
#define dumpline cout<<"ORA-"<<dbinterface.errorCode()<<":"<<dbinterface.errorMessage()<<";dbparamErrMsg=["<<m_errorCode<<"]"<<m_errorMessage.c_str()<<";Position="<<__FILE__<<":"<<__LINE__<<" errno="<<errno<<"["<<strerror(errno)<<"]"<<endl;
#else
#define dumpline
#endif

//##ModelId=3B8F7F0A01BE
bool ParamManager::initialization()
{
	m_errorCode=0;
	m_errorMessage="";
	connected=0;
	dbinterface.setFieldLength(1000);

	if(!addParamObject())
	{
		return false;
	}

	lastUpdateTimeStamp="";
	updateTimeStamp="";
	updateIntervalTime=DEFAULTINTERVAL;
	lastUpdateTime=0;
	getMode=GET_DB_TIME|GET_FIRST;

	needUpdate();
	
	if(m_errorCode)
	{
		return false;
	}

	if(!getData())
	{
		return false;
	}
	return true;
}

bool ParamManager::addParamObject()
{
	m_errorCode=0;
	m_errorMessage="";
	return true;
}

//##ModelId=3B8F7F3101BA
bool ParamManager::end()
{
	m_errorCode=0;
	m_errorMessage="";
	for(m_paramContainerListItr=m_paramContainerList.begin();
			m_paramContainerListItr!=m_paramContainerList.end();
			++m_paramContainerListItr)
	{
		(*m_paramContainerListItr)->clean();
	}
	m_paramContainerList.clear();
	return true;
}

//##ModelId=3B8F54A0032E
bool ParamManager::needUpdate()
{
	StringVector recordSet;
	StringVector errorSet;
	PO_Property property;
	int nTotal,nSuccess,nError;
	FILE *f;
	char srcfile[500];
	time_t nowtime;
	struct stat filestat;
	ofstream df;

	m_errorCode=0;
	m_errorMessage="";
	for(m_paramContainerListItr=m_paramContainerList.begin();
			m_paramContainerListItr!=m_paramContainerList.end();
			m_paramContainerListItr++)
	{
		(*m_paramContainerListItr)->getProperty(property);

// add by chenm 2003-7-9 for test,为了不建参数表,直接从文件中读参数
#ifndef FOR_TEST 
		if(property.dataSource==FROM_DATABASE)
		{
			if(property.updateCondition==BY_FLAG)
			{
				if( !connected )
				{
					if(dbinterface.connect(m_userName.c_str(),
							m_passWord.c_str(),m_servName.c_str()))
					{
						m_errorCode=RC_CONN_FAIL;
						m_errorMessage=RM_CONN_FAIL;
                        dumpline;
						break;
					}
					connected=1;
				}
				
				recordSet.clear();
				if(dbinterface.dataBind(recordSet,errorSet))
				{
					m_errorCode=RC_BIND_FAIL;
					m_errorMessage=RM_BIND_FAIL;
                    dumpline;
					break;
				}
				do
				{
					if(dbinterface.executeSql(SQLUPDATE_1,
							nTotal,nSuccess,nError))
					{
						m_errorCode=RC_EXEC_FAIL;
						m_errorMessage=RM_EXEC_FAIL;
                        dumpline;
						break;
					}
				}while(nTotal>=DEFAULTARRAYLINE);
				if(m_errorCode)break;
				if(nTotal)
				{
					if(*recordSet.begin()!=lastUpdateTimeStamp)
					{
						getMode|=GET_DB_FLAG;
						updateTimeStamp=*recordSet.begin();
					}
				}
			}
			if(property.updateCondition==BY_TIME)
			{
				time(&nowtime);
				if(nowtime-lastUpdateTime>=updateIntervalTime)
						getMode|=GET_DB_TIME;
			}
		}
#endif
		if(property.dataSource==FROM_FILE)
		{
			sprintf(srcfile,"%s/%s/%s_%d",truepath(property.filePath.c_str()),
					m_appName.c_str(),property.fileName.c_str(),m_channelNo);
			f=fopen(srcfile,"r");
			if(f!=NULL)
			{
				fclose(f);
				getMode|=GET_FILE_FLAG;
			}
		}
	}
	if(m_errorCode)
	{
		if(connected)
		{
			dbinterface.disconnect();
			connected=0;
		}
		return false;
	}
	
	if(getMode)
	{
		return true;
	}
	else
	{ 
		return false;
	}
}

//##ModelId=3B8F54B40282
bool ParamManager::getData()
{
	StringVector recordSet;
	StringVector errorSet;
	PO_Property property;
	int nTotal,nSuccess,nError;
	FILE *f=NULL;
	char srcfile[500];
	char bakfile[500];
	char record[5000];
	char field[500];
	char *begin;
	char *end;
	int fn;
	int rn;
	int point;
	int ret;
	long recordCount=0;
	time_t nowtime;
	FILE *pPipe;
	char syscmd[1000];
	char logsql[1000]="";
	char logtime[15]="";
	time_t lNowTime;
	struct tm *stNowTime;

	m_errorCode=0;
	m_errorMessage="";
	for(m_paramContainerListItr=m_paramContainerList.begin();
			m_paramContainerListItr!=m_paramContainerList.end();
			m_paramContainerListItr++)
	{
		(*m_paramContainerListItr)->rewind();
		(*m_paramContainerListItr)->getProperty(property);
        //added by shandy 20060712
        if(getMode&GET_FIRST)
            (*m_paramContainerListItr)->setLocalProperty(&property);
// add by chenm 2003-7-9 for test,为了不建参数表,直接从文件中读参数
#ifndef FOR_TEST 
		if(property.dataSource==FROM_DATABASE)
		{
			if( (property.updateCondition==BY_FLAG) &&
				(!(getMode&GET_DB_FLAG)) )
			{		
				continue;
			}
			if( (property.updateCondition==BY_TIME) &&
					(!(getMode&GET_DB_TIME)) )
			{		
				continue;
			}
#ifdef _DEBUG_
                if(property.dataSource==FROM_DATABASE)
                    cout<<"get DB   Param Object => "<<property.fileName<<endl;
                else
                    cout<<"get FILE Param Object => "<<property.fileName<<endl;                
#endif
			if( !connected )
			{
				
				if(dbinterface.connect(m_userName.c_str(),
						m_passWord.c_str(),m_servName.c_str()))
				{
					m_errorCode=RC_CONN_FAIL;
					m_errorMessage=RM_CONN_FAIL;
                    dumpline;
					break;
				}
				connected=1;
#ifdef _DEBUG_
    cout<<"DB connect OK"<<endl;
#endif					
			}
			recordSet.clear();
			if(dbinterface.dataBind(recordSet,errorSet))
			{
				m_errorCode=RC_BIND_FAIL;
				m_errorMessage=RM_BIND_FAIL;
                dumpline;
				break;
			}
			if(getMode&GET_FIRST)
			{
				if(dbinterface.executeSql(property.countSql.c_str(),
						nTotal,nSuccess,nError))
				{
					m_errorCode=RC_EXEC_FAIL;
					m_errorMessage=RM_EXEC_FAIL+":"+property.countSql;
                    dumpline;
					break;
				}
				recordCount=atol(recordSet[0].c_str());

				if(!(*m_paramContainerListItr)->prepareSize(recordCount*1.1))
				{
					m_errorCode=RC_PREP_FAIL;
					m_errorMessage=RM_PREP_FAIL;
                    dumpline;
					break;
				}
			}
			recordCount=0;

			do
			{
				if(dbinterface.executeSql(property.sqlStatement.c_str(),
						nTotal,nSuccess,nError))
				{
					m_errorCode=RC_EXEC_FAIL;
					m_errorMessage=RM_EXEC_FAIL+":"+property.sqlStatement;
                    dumpline;
					break;
				}

				if(!(*m_paramContainerListItr)->dataIn(recordSet))
				{
					m_errorCode=RC_DATI_FAIL;
					m_errorMessage=property.fileName;
                    dumpline;
					break;
				}
				recordCount+=nTotal;
			}while(nTotal>=DEFAULTARRAYLINE);
			if(m_errorCode)break;
			
			//(*m_paramContainerListItr)->buildNextRelation();//zhaogc add
		}


		if(property.dataSource==FROM_FILE)
#endif
		{
// add by chenm 2003-7-9 for test,为了不建参数表,直接从文件中读参数
#ifdef FOR_TEST
	        property.filePath = FILEPATH_1;
#endif
			sprintf(srcfile,"%s/%s/%s_%d",truepath(property.filePath.c_str()),
					m_appName.c_str(),property.fileName.c_str(),m_channelNo);
			sprintf(bakfile,"%s/%s/%s_%d.bak",truepath(property.filePath.c_str()),
					m_appName.c_str(),property.fileName.c_str(),m_channelNo);
			ret=access(srcfile,F_OK);
            //modified by shandy 20060712
            //if(ret&&(!(getMode&GET_FIRST)))continue;
            if(ret&&(!(getMode&GET_FIRST)))
            {
                if(getMode&GET_FILE_TIME)//是否需要根据文件时间来判断
                {
                    //srcfile文件不存在时，若.bak时间和上次保留的时间一致的才可以跳过更新
			        time_t fileNewTime=getFileModifyTime(bakfile);
                    //cout<<"fileNewTime="<<fileNewTime<<" property.fileModifyTime="<<(*m_paramContainerListItr)->getLocalProperty()->fileModifyTime<<endl;
                    if(ret&&fileNewTime == (*m_paramContainerListItr)->getLocalProperty()->fileModifyTime)
                        continue;
                }
                else
                    continue;
            }
#ifdef _DEBUG_
                if(property.dataSource==FROM_DATABASE)
                    cout<<"get DB   Param Object => "<<property.fileName<<endl;
                else
                    cout<<"get FILE Param Object => "<<property.fileName<<endl;
#endif
			if(ret==0)
			{
				sprintf(syscmd,"mv %s %s",srcfile,bakfile);
				if(system(syscmd))
				{
					m_errorCode=RC_SYSC_FAIL;
					m_errorMessage=RM_SYSC_FAIL+":"+syscmd;
                    dumpline;
					break;
				}
			}
			f=fopen(bakfile,"r");
			if(f==NULL)continue;

			if(getMode&GET_FIRST)
			{
				sprintf(syscmd,"wc -l %s",bakfile);
				if((pPipe=popen(syscmd,"r"))!=NULL)
				{
					if(fgets(syscmd,20,pPipe)!=NULL)
					{
						recordCount=atol(syscmd);
					}
					pclose(pPipe);
				}
				else
				{
					m_errorCode=RC_PIPE_FAIL;
					m_errorMessage=RM_PIPE_FAIL+":"+syscmd;
                    dumpline;
					break;
				}
			
				if(!(*m_paramContainerListItr)->prepareSize(recordCount*1.1))
				{
					m_errorCode=RC_PREP_FAIL;
					m_errorMessage=RM_PREP_FAIL;
                    dumpline;
					break;
				}
			}

			rn=0;
			recordSet.resize(DEFAULTARRAYLINE*property.fieldNum);
			point=0;
			while(1)
			{

				if(fgets(record,5000,f)==NULL)break;
				rn++;
				fn=0;
				begin=record;

				while((end=strchr(begin,','))!=NULL)
				{
					strncpy(field,begin,end-begin);
					field[end-begin]=0;
					begin=end+1;
					recordSet[point]=field;
					point++;
					fn++;
				}
				if(strlen(begin)>0)
				{
					if(begin[strlen(begin)-1]=='\n')
							begin[strlen(begin)-1]=0;
					strcpy(field,begin);
					recordSet[point]=field;
					point++;
					fn++;
				}
				if(fn!=property.fieldNum)
				{
					point-=fn;
					rn--;
				}
				if(point>=recordSet.size())
				{
					if(!(*m_paramContainerListItr)->dataIn(recordSet))
					{
						m_errorCode=RC_DATI_FAIL;
						m_errorMessage=property.fileName;
                        dumpline;
						break;
					}
					point=0;
				}
			}
			if(m_errorCode)break;
			if(point>0)
			{
				recordSet.resize(point);
				if(!(*m_paramContainerListItr)->dataIn(recordSet))
				{
					m_errorCode=RC_DATI_FAIL;
					m_errorMessage=property.fileName;
                    dumpline;
					break;
				}
			}
			fclose(f);
			recordCount=rn;
		}
		(*m_paramContainerListItr)->resize(recordCount);
		(*m_paramContainerListItr)->sort();
		//added by shandy 20060712
        (*m_paramContainerListItr)->getLocalProperty()->fileModifyTime=getFileModifyTime(bakfile);

		//-----write update log---------------
		if((property.updateCondition==BY_FLAG)&&
				(getMode&GET_DB_FLAG))
		{
			strncpy(logtime,updateTimeStamp.c_str(),sizeof(logtime)-1);
		}
		if((property.updateCondition==BY_TIME)&&
				(getMode&GET_DB_TIME))
		{
			time(&lNowTime);
			stNowTime=localtime(&lNowTime);
			strftime(logtime,15,"%Y%m%d%H%M%S",stNowTime);
		}
	}


	getMode=0;
	
	if(m_errorCode)	{
		if(connected)
		{
			dbinterface.disconnect();
			connected=0;
		}
		if(f)fclose(f);
		if(property.dataSource==FROM_FILE)
		{
			if(access(bakfile,F_OK))
			{
				sprintf(syscmd,"mv %s %s",bakfile,srcfile);
				system(syscmd);
			}
		}
    //	cout << "get "<< property.fileName<<" failure!" <<endl;
		return false; 
	}

	lastUpdateTimeStamp=updateTimeStamp;
	time(&nowtime);
	lastUpdateTime=nowtime;
	
    // sprintf(srcfile,"%s/%s/param_%d.dump",FILEPATH_1,m_appName.c_str(),m_channelNo);
	// ofstream df;
	// df.open(srcfile);
	// dump(df);
	// df.close();

	return true;
}


//##ModelId=3B8F54F600ED
bool ParamManager::update(const int mode)
{
	m_errorCode=0;
	m_errorMessage="";
    if(mode >= 0)
        getMode=getMode|mode;
        
    return getData();
}

bool ParamManager::setUpdateInterval(const int second)
{
	m_errorCode=0;
	m_errorMessage="";
	updateIntervalTime=second;
	return true;
}


int ParamManager::errorCode()
{
	return m_errorCode;
}

string ParamManager::errorMessage()
{
	return m_errorMessage;
}

void ParamManager::setHostName(const string hostName)
{
	m_hostName=hostName;
}

void ParamManager::setAppName(const string appName)
{
	m_appName=appName;
}

void ParamManager::setChannelNo(const int channelNo)
{
	m_channelNo=channelNo;
}

bool ParamManager::dump(ostream &os)
{
	PO_Property property;

	m_errorCode=0;
	m_errorMessage="";
	for(m_paramContainerListItr=m_paramContainerList.begin();
			m_paramContainerListItr!=m_paramContainerList.end();
			m_paramContainerListItr++)
	{
		(*m_paramContainerListItr)->getProperty(property);
		os<<"=============================="<<endl;
		os<<"dataSource: "<<property.dataSource<<endl;
		os<<"updateCondition: "<<property.updateCondition<<endl;
		os<<"filePath: "<<property.filePath<<endl;
		os<<"fileName: "<<property.fileName<<endl;
		os<<"userName: "<<m_userName<<endl;
		os<<"passWord: "<<m_passWord<<endl;
		os<<"servName: "<<m_servName<<endl;
		os<<"sqlStatement: "<<property.sqlStatement<<endl;
		os<<"fieldNum: "<<property.fieldNum<<endl;
		os<<"------------------------------"<<endl;
		(*m_paramContainerListItr)->dump(os);
	}

	return true;
}





char * ParamManager::truepath(const char *path)
{
	//static char truepath[500];
	char env[100];
	char *c;

	if(*path=='~')
	{
		strcpy(truepathBuffer,getenv("HOME"));
		strcat(truepathBuffer,path+1);
	}
	else if(*path=='$')
	{
		c=(char *)strchr(path,'/');
		if(c==NULL)
		{
			strcpy(truepathBuffer,getenv(path+1));
		}
		else
		{
			strncpy(env,path+1,c-path);
			env[c-path-1]=0;
			strcpy(truepathBuffer,getenv(env));
			strcat(truepathBuffer,c);
		}
	}
	else
	{
		strcpy(truepathBuffer,path);
	}

	return truepathBuffer;
}

// 从应用框架中得到数据库联接信息
void ParamManager::setDbConnInfo(const string &s_userName,const string &s_passWord,
                                 const string &s_servName)
{
	m_userName = s_userName;   
	m_passWord = s_passWord;
	m_servName = s_servName;	
}

ParamManager::~ParamManager()
{
	if(connected)
	{
		dbinterface.disconnect();
		connected = 0;
	}	
}
time_t ParamManager::getFileModifyTime(const char* fname)
{
    struct stat fileAttr;
    if(lstat(fname,&fileAttr) < 0)
        return 0;
    return fileAttr.st_mtime;
}
time_t ParamManager::getUpdateMinTime()
{
    time_t minT,tmpT;
    PO_Property* propertyPtr;
    char srcfile[1024],bakfile[1024];

    time(&minT);
    for(m_paramContainerListItr=m_paramContainerList.begin();
			m_paramContainerListItr!=m_paramContainerList.end();
			m_paramContainerListItr++)
	{
	    propertyPtr = (*m_paramContainerListItr)->getLocalProperty();
	    if(propertyPtr->dataSource==FROM_FILE)
	    {
            sprintf(srcfile,"%s/%s/%s_%d\0",truepath(propertyPtr->filePath.c_str()),
                             m_appName.c_str(),propertyPtr->fileName.c_str(),m_channelNo);
            sprintf(bakfile,"%s/%s/%s_%d.bak\0",truepath(propertyPtr->filePath.c_str()),
                            m_appName.c_str(),propertyPtr->fileName.c_str(),m_channelNo);
            if(access(bakfile,F_OK) < 0)
            {
                if(access(srcfile,F_OK) < 0)
                {//资料不存在
                    continue;
                }
                else
                    tmpT = 0;
            }
            else
	            tmpT=propertyPtr->fileModifyTime;
	        if(tmpT < minT)
	            minT = tmpT;
	    }
	}
	return minT;
}

