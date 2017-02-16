#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "MdbDmlSql.h"
#include "common.h"
#include "MutexShmMgr.h"
#include "IndexHash.h"

int loadHelp();
bool Connected()
{
	return m_db != NULL;
}
bool isNumber(const string& r_src)
{
	for (int i = 0;i<r_src.length(); i++)
	{
		if (r_src[i]<'0' || r_src[i]>'9')
			return false;
	}
	return true;
}
long getTableRowCount(const string& r_tableName)
{
    Table* table;
    try
    {
        table = m_db->findTableByName(r_tableName.c_str());
    }
    catch(Mdb_Exception &e)
    {
        cout << "获取"<<r_tableName<<"记录总数失败:" << endl;
        e.toString();
        return -1;
    }
    return table->m_tableDesc->m_recordNum;	
}
bool isValidMDBName(const string&r_dbName,bool isCreate=false)
{
    string t_dbLockFile;
    if (isCreate)
    {
    	t_dbLockFile+=getenv(MDB_HOMEPATH.c_str());
    	t_dbLockFile+="/config/";
    	t_dbLockFile+=r_dbName;
    	t_dbLockFile+=".conf";
    }
    else
    {
    	t_dbLockFile+=getenv(MDB_HOMEPATH.c_str());
    	t_dbLockFile+="/ctl/";
    	t_dbLockFile+=r_dbName;
    	t_dbLockFile+=".ctl";
    }
    if (access(t_dbLockFile.c_str(),R_OK) !=0)
    {
    	return false;
    }
    return true;	
}
int Parse (const vector<string>& vec, ErrParam& errParam)
{
    vector<string> vecstr = vec;
    
    //匹配map命令
    map<string, SingleFunc>::iterator itr = mapCmdFunc.find(vecstr[0]);

    if(itr!=mapCmdFunc.end())
    {
        //20080523添加，判断是否有连接，没有直接返回
        if((vecstr[0]!="connect")&&(vecstr[0]!="disconnect")
            &&(vecstr[0]!="help")&&(vecstr[0]!="?")
            &&(vecstr[0]!="exit")&&(vecstr[0]!="quit")
            &&(vecstr[0]!="start")
            &&(vecstr[0]!="create")&&(vecstr[1]!="database") )
        {
            if((m_db==NULL)||(m_dbName==""))
            {
                cout << "当前没有连接数据库，请先connect数据库" << endl;
                return 0;
            }
        }
        //zhaogc add
        if((vecstr[0]=="drop")&&(vecstr[1]=="database"))
        {
            if((m_db==NULL)||(m_dbName==""))
            {
                cout << "当前没有连接数据库，请先connect数据库" << endl;
                return 0;
            }        	
        }
        // add end

        //每次执行之前更新全局信息
        if((m_db!=NULL)&&(m_memMgr!=NULL))
        {    
            m_memMgr->updateMgrInfo();
        }
        //执行函数操作
        try
        {
            itr->second(vecstr, errParam); 
        }
        catch(Mdb_Exception &e)
        {
            e.toString();
            return -1;
        }
        
        return 0;
    }   
    else
    {
        cout << "没有以" << vecstr[0] << "开头的命令" << endl;
    }  
        
    return -1;
}    


int dealCmd(const char* str) 
{
    char* commandStr;
    
    char* ch = (char*) str;
    int g = 0;
    //计算str的长度 
    while((*ch)!='\0')
    {
        ch++;
        g++;
    }    
    if(g!=0)
    {
        commandStr = new char[g+1];
    }    
    else
    {
        return -1;   
    }    
    
    memset(commandStr, 0, g+1);
    strcpy(commandStr, str);
    //对命令字符串的简单处理(去分号、空格等)
    
    //add by yih@
    //处理前先去除语句前后的空格
    strcpy(commandStr,trim(string(commandStr)).c_str());
    
    //将最后一个分号替换成'\0'
    if(commandStr[strlen(commandStr)-1]==';')
    {
        commandStr[strlen(commandStr)-1]='\0';
    }    
    //转换大小写modify by xuf 20100309
    //for(int k=0;k<strlen(commandStr);k++)
    //{
    //    commandStr[k]=tolower(commandStr[k]);
    //}    
    
    //cout << "|" << commandStr << "|" << endl;
    //cout << strlen(commandStr) << endl;
    
    string scmd = commandStr;
    delete []commandStr;
    
    vector<string> vecstr;
    vector<string> vectmp;
    string tempstr;
    
    scan(scmd, vectmp);
    
    // 对vecstr进行特殊处理 ，应用select中 count （*）情况
    for (int i = 0; i < vectmp.size(); i++)
    {
        if (vectmp[i] == "count")
        {
            tempstr = vectmp[i] + vectmp[++i];
            vecstr.push_back(tempstr);
        }
        else
        {
            tempstr = vectmp[i];
            vecstr.push_back(tempstr);
        }
    }
    
    ErrParam errParam;

    errParam.m_errInfo = "";
    errParam.m_errNo = 0;
    errParam.m_errPos = 0;
    
    if(Parse(vecstr, errParam)>=0)
    {
        //for(vector<string>::iterator itr = vecstr.begin();
        //    itr!=vecstr.end(); itr++)
        //{
        //     cout << trim(*itr," ") << " ";
        //}
        //cout << endl;
        if(errParam.m_errNo != 0)
        {
            //cout << "处理后的输入是:";
            //cout << "vecstr.size()=" << vecstr.size() << endl;
            for(vector<string>::iterator itr = vecstr.begin();
                itr!=vecstr.end(); itr++)
            {
                 cout << trim(*itr," ") << " ";
            }
            cout << endl;
            int cols = 0;
            for(int i=0; i<=vecstr.size()-1; i++)
            {
                for(int j=0; j<(vecstr[i].length()); j++)
                {
                    if(i==errParam.m_errPos)
                    {    
                        cout << "^";
                    }
                    else
                    {
                        cout << " ";
                    }    
                }    
                if(i==errParam.m_errPos)
                {    
                    break;
                }
                else
                {
                    cols += vecstr[i].length();
                }    
                cout << " "; cols += 1;
            }  
            //如果是因为参数个数不足，需要在命令行后面再添加命令 
            if(errParam.m_errPos+1>vecstr.size())
            {
                cout << "^^^" << endl;
            }    
            cout << endl;
            //cout << "第" << errParam.m_errPos+1 << "个标记,第" << cols+1 << "列有问题" << endl
            //     << "ErrNo: " << errParam.m_errNo << endl
            //     << "ErrInfo: " << errParam.m_errInfo << endl;
            cout << cols+1 << "列," << "标记" << errParam.m_errPos+1 << ",:" << errParam.m_errInfo << endl;
        }      
    }    
    
    return 0;
}    




/*
经过封装的辅助函数
*/
//检查是否有该表空间是否存在
bool isTableSpaceExists(const char* r_spaceName)
{
    SpaceInfo r_spaceInfo;
    try
    {
        if(!m_memMgr->getSpaceInfo(r_spaceName, r_spaceInfo))
        {
            return false;
        } 
    }
    catch(Mdb_Exception &e)
    {
        e.toString();
        return false;
    }
    return true;
}

//检查是否有该表存在
bool isTableExists(const char* tableName)
{
    //cout << tableName << endl;
    //vector<TableDef> r_tableDefList;
    TableDesc* r_pTableDesc;
    try
    {
        /*if(!m_memMgr->getTableDefList(r_tableDefList))
        {
            cout << "return false" << endl;
            return false;    
        }
        cout << "m_memMgr->getTableDefList(r_tableDefList)" << endl;
        for(vector<TableDef>::iterator itr=r_tableDefList.begin();
            itr!=r_tableDefList.end(); itr++)
        {
            string s=tableName;
            if(itr->m_tableName==s)
            {
                return true;
            }
        }
        return false;*/
        if(!m_memMgr->getTableDescByName(tableName, r_pTableDesc))
        {
            return false;
        } 
        return true;
    }
    catch(Mdb_Exception &e)
    {
        e.toString();
        return false;
    }
}    

//检查是否有该字段存在
int isColumnExists(const char* tableName, const char* columnName)
{
    //cout << "tableName=" << tableName << endl;
    //cout << "columnName=" << columnName << endl;
    string  columnStrName= columnName;
    if(columnStrName=="*")
    {
        return 0;
    }
    
    TableDesc* r_pTableDesc;
    if(!m_memMgr->getTableDescByName(tableName, r_pTableDesc))
    {
        return -1;    
    }
    else
    {
        //根据表信息，查找字段是否存在 
        for(int k =0; k<(r_pTableDesc->m_tableDef).m_columnNum; k++)
        {
            string s =(r_pTableDesc->m_tableDef).m_columnList[k].m_name;
            if (strcasecmp(s.c_str(),columnName) == 0)
            //if (s==columnStrName)
            {
                return 0;
            }    
        }
        return -2;    
    }    
}  


//检查该索引是否存在 ,此处vetor<string> vecColNames是指字段名而非索引名
int isIndexExists(const char* tableName, const vector<string>& vecColNames, string& indexName)
{
    //cout << "tableName=" << tableName << endl;
    //cout << "vecColNames.size=" << vecColNames.size() << endl;
    //cout << vecColNames[0] << endl;
    indexName = "";
    TableDesc* r_pTableDesc;
    if(!m_memMgr->getTableDescByName(tableName, r_pTableDesc))
    {
        return -1;    
    }
    else
    {
        vector<IndexDef> r_pIndexLis;
        //根据表信息，查找表的索引
        m_memMgr->getIndexListByTable(tableName, r_pIndexLis); 
        //cout << "r_pIndexLis.size=" << r_pIndexLis.size() << endl;
        for(vector<IndexDef>::iterator itr=r_pIndexLis.begin();
            itr!=r_pIndexLis.end(); itr++)
        {
            cout <<"索引名称:"<< itr->m_indexName << endl;
            vector<string> vecs;
            vecs.clear();
            //cout << "m_columnNum = " << itr->m_columnNum << endl;
            for(int i=0;i<itr->m_columnNum;i++)
            {
                //cout << itr->m_columnList[i] << endl;
                string s = itr->m_columnList[i];
                vecs.push_back(s);
            }
            //cout << "vecColNames=" << vecColNames[0] << endl
            //     << "vecs=" << vecs[0] << endl;
            //逐个索引与vecColNames比较，是否相同

            if(equalVecStr(vecColNames, vecs))
            {
                //cout << "itr->m_indexName=" << itr->m_indexName << endl;
                //返回索引名
                indexName = itr->m_indexName;
                return 0;
            }
        }
        return -2;    
    }    
}    
    
//通过表名查找表的所有字段 
int judgeColumns(const char* tableName, vector<Column>& vecColumns)
{
    TableDesc* r_pTableDesc;
    if(!m_memMgr->getTableDescByName(tableName, r_pTableDesc))
    {
        return -1;    
    }
    for(vector<Column>::iterator iter = vecColumns.begin();
        iter != vecColumns.end(); iter++)
    {
        //根据表信息，查找字段是否存在 
        for(int k =0; k<(r_pTableDesc->m_tableDef).m_columnNum; k++)
        {
            string s =(r_pTableDesc->m_tableDef).m_columnList[k].m_name;
            //cout << "s=" << s << "   " << "iter->m_colName=" << iter->m_colName << endl;
            if (capToLower(s)==capToLower(iter->m_colName))
            {
                //cout << "iter->m_colName=" << iter->m_colName << endl;
                //将字段内容填入
                iter->m_colName = s;
                iter->m_colType = (r_pTableDesc->m_tableDef).m_columnList[k].m_type;
                //cout << "iter->m_colName=" << iter->m_colName << endl;
                //cout << " iter->m_colType" <<  iter->m_colType << endl;
                break;
            }    
            if(k>=(r_pTableDesc->m_tableDef).m_columnNum)
            {
                return -2;
            }
        }    
    }
    
    return 0;
}


//断开数据库连接
int Disconnect()
{
    if(m_db!=NULL)
    {
        cout << "disconnecting " << m_dbName << " now..." << endl;
        try
        {
            m_db->disconnect(m_session);
            //delete m_db;
            //delete m_memMgr;
            m_db = NULL;
            m_session = NULL;
            m_memMgr = NULL;
        }
        catch(Mdb_Exception &e)
        {
            cout << "disconnect " + m_dbName + "error.";
            e.toString();
            return -2;
        }
        m_dbName = "";
        cout << m_dbName << " successfully disconnected " << endl;
    }
    else
    {
        m_dbName = "";
        return -1;
    }
    
    return 0;
}

 /**  
 * showObjList显示所有对象列表.(供tablespaces,tables,indexes,sessions使用)
 *@param  int objType对象类型 0表空间 1表 2索引 3session
 *@return true类型存在 false不存在
 */
//bool FuncCaller::showObjList(int objType)
bool showObjList(int objType)
{
    vector<SpaceInfo>   spaceInfoList;
    vector<TableDef>    tableDefList;
    vector<IndexDef>    r_pIndexList;
    vector<SessionInfo> sessionInfoList;
    map<string,float> vUserdPercent;
    
    try
    {
    
        switch(objType)
        {
            case 0:  //表空间
                
                if(!m_memMgr->getSpaceInfoList(spaceInfoList))
                {
                    return false;
                }
                T_FILENAMEDEF  cCtlFileName;
                T_FILENAMEDEF  cLockFileName;
                sprintf(cCtlFileName,"%s/ctl/%s.ctl",getenv(MDB_HOMEPATH.c_str()),m_dbName.c_str());
                sprintf(cLockFileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),m_dbName.c_str());
                
                for(vector<SpaceInfo>::iterator itr=spaceInfoList.begin();
                    itr!=spaceInfoList.end();itr++)
                {
                    cout << "----------------------------------------------" << endl
                         << *itr ;
                    printf("shareM ID   = 0x%08x\n",ftok(cCtlFileName,itr->m_spaceCode));
                }
                
                cout << "----------------------------------------------" << endl;
                printf("ctl space ID   = 0x%08x\n",ftok(cCtlFileName,1));
                printf("lock space ID  = 0x%08x\n",ftok(cLockFileName,MutexShmMgr::MUTEX_FTOK_ID));
                cout << "----------------------------------------------" << endl;
                
                m_memMgr->getTableSpaceUsedPercent(vUserdPercent);
                for(map<string,float>::iterator itr=vUserdPercent.begin();itr!=vUserdPercent.end();++itr)
                {
                	cout<<"Talbespace:"<<itr->first<<" has been used:"<<itr->second*100<<"%"<<endl;	
                }
                break;
            case 1:  //表
                
                if(!m_memMgr->getTableDefList(tableDefList))
                {
                    return false;
                }
                for(vector<TableDef>::iterator itr=tableDefList.begin();
                    itr!=tableDefList.end();itr++)
                {
                    cout << itr->m_tableName << endl;
                }
                break;
            case 2:  //索引
                //先获取表
                if(!m_memMgr->getTableDefList(tableDefList))
                {
                    return false;
                }
                for(vector<TableDef>::iterator itr=tableDefList.begin();
                    itr!=tableDefList.end();itr++)
                {
                    cout << "Table: " << itr->m_tableName << endl;
                    m_memMgr->getIndexListByTable(itr->m_tableName, r_pIndexList);
                    for(vector<IndexDef>::iterator itr = r_pIndexList.begin();
                        itr != r_pIndexList.end(); itr++)
                    {
                        cout << "\tIndex: " << itr->m_indexName << endl;                    
                    }
                
                }
                
                break;
            case 3:  //session
                
                if(!m_memMgr->getSessionInfos(sessionInfoList))
                {
                    return false;
                }
                
                cout << "sessionId pid registTime" << endl;
                
                for(vector<SessionInfo>::iterator itr=sessionInfoList.begin();
                    itr!=sessionInfoList.end(); itr++)
                {
                    cout << *itr ;
                    
                    //cout << "isPidExists(" << itr->m_pId << ")" << endl;
                    if(isPidExists(itr->m_pId))
                    {
                        cout << "进程还在";
                    }
                    else
                    {
                        cout << "进程已退出";
                    }
                    cout << endl;
                    
                    if(getpid()==itr->m_pId)
                    {
                        cout << "***yourself***";
                    }
                    cout << endl;
                }
                cout << "My SessionId = " << m_session->getSessionID() << endl;
                break;
            default:
                return false;
        }
    }
    catch(Mdb_Exception &e)
    {
        e.toString();
        return false;
    }
    
    return true;
}





/*
command命令解析对应的函数 
*/
void help(vector<string>& sqlParam, ErrParam& errParam)
{
    loadHelp();
    
    vector<string>::iterator bitr = vecHelp.begin(), eitr = vecHelp.end();
    vector<string>::iterator itr;
    
    if(sqlParam.size() == 1)
    {
        string strState = "entrance";
        
        int strLen = strState.size();
        for(itr = bitr; itr != eitr; itr++)
        {
            if((*itr).substr(0,3) == "***" 
            	   && (*itr).substr(3,strLen) == strState)
            {
            	  //cout << *itr << endl;
            	  itr++;
                break;
            }
        }
        
        for( ; itr != eitr; itr++)
        {
            if((*itr).substr(0,3) == "***")
            {
                break;
            }
            else
            {
                cout << *itr << endl;
            }
        }        
    }
    else
    {
        string strState = capToLower(trim(sqlParam[1]));
        
        if(strState=="all")
        {
            for(itr = bitr; itr != eitr; itr++)
            {
                cout << *itr << endl;
            }
            return;
        }
        
        
        int strLen = strState.size();
        for(itr = bitr; itr != eitr; itr++)
        {
            if((*itr).substr(0,3) == "***" 
            	   && (*itr).substr(3,strLen) == strState)
            {
            	  //cout << *itr << endl;
            	  itr++;
                break;
            }
        }
        if(itr == eitr)
        {
            cout << "没有匹配与\"" << sqlParam[1] << "\"相关的help关键词" << endl
                 << "请尝试使用\"help all\"查询所有help信息" << endl;
            return;
        }
        
        for( ; itr != eitr; itr++)
        {
            if((*itr).substr(0,3) == "***")
            {
                break;
            }
            else
            {
                cout << *itr << endl;
            }
        }        
    }
    
    return;  
}

void quit(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //断开之前的连接
    Disconnect();
    
    //do... 
    cout << "quit now..." << endl;
    g_run = false;
}    

void version(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //do... 
    cout << "version 0.15" << endl;
}    


void sysdate(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //do... 
    cout << "sysdate" << endl;
}    

void disconnect(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //断开之前的连接
    if(Disconnect()==-1)
    {
        cout << "connected nothing" << endl;
    }
}    

void tablespaces(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //do... 
    showObjList(0);
}    

void tables(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    
    //do... 
    showObjList(1);
}    

void indexes(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //do... 
    showObjList(2);
}    

void sessions(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    //do... 
    showObjList(3);
    
}    
void linemode(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()!=2)
    {
        errParam.m_errInfo = "此命令只带有1个参数(on/off)";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }
	if (sqlParam[1] == "on")
	{
		m_lineMode = true;
	}
	else if (sqlParam[1] == "off")
	{
		m_lineMode = false;
	}
	else
	{
        errParam.m_errInfo = "参数只有2个选项(on或off)";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
	}
}
void kill(vector<string>& sqlParam,  ErrParam& errParam)
{
    //kill sessionId
    if(sqlParam.size()<2)
    {
        errParam.m_errInfo = "此命令需带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }    
    
    if(sqlParam.size()>2)
    {
        errParam.m_errInfo = "此命令只带有1个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }    
    
    const char*  c= sqlParam[1].c_str();
    for(int i=0; i<sqlParam[1].size(); i++, c++)
    {
        if(isdigit(*c)==0)
        {
            errParam.m_errInfo = "此命令参数必须为数字";
            errParam.m_errNo = -12345;
            errParam.m_errPos = 1;//sqlParam的第二个参数 
            return;
        }  
    }      
    
    vector<SessionInfo> r_sessionInfoList;
    try
    {
        cout << "kill " << sqlParam[1] << endl;
        if(!m_memMgr->getSessionInfos(r_sessionInfoList))
        {
            cout << "kill error" << endl;
            return;
        }
        for(vector<SessionInfo>::iterator itr = r_sessionInfoList.begin();
            itr !=r_sessionInfoList.end(); itr++)
        {
            if(itr->m_sessionId==atoi(sqlParam[1].c_str()))
            {
                //检查是否是进程自己
                if(getpid()==itr->m_pId)
                {
                    //是当前进程自身
                    cout << "不能杀掉自己的session" << endl;
                    return;
                }
                //检查进程是否还活着
                if(isPidExists(itr->m_pId))
                {
                    cout << itr->m_pId << "进程仍然活着，不能杀掉他的session" << endl;
                    return;
                }
                m_memMgr->unRegisterSession(*itr);
                cout << "killed" << endl;
                return;
            }
        }
    }
    catch(Mdb_Exception &e)
    {
        cout << "kill error" << endl;
        e.toString();
        return;
    }
}  

void commit(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    try
    {
        //m_session->commit();
        cout << "目前已经采取自动commit，不需要手工做" << endl;
    }
    catch(Mdb_Exception &e)
    {
        cout << "commit err.";
        e.toString();
        return;
    }
    
    cout << "commited" << endl;
}   

void rollBack(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()>1)
    {
        errParam.m_errInfo = "此命令不带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }    
    
    try
    {
        //m_session->rollBack();
        cout << "目前已经采取自动rollBack，不需要手工做" << endl;
    }
    catch(Mdb_Exception &e)
    {
        cout << "rollBack err.";
        e.toString();
        return;
    }
    
    cout << "rollBacked" << endl;
}   

void run(vector<string>& sqlParam,  ErrParam& errParam)
{
    //run fileName
    if(sqlParam.size()<2)
    {
        errParam.m_errInfo = "此命令需带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }    
    
    if(sqlParam.size()>2)
    {
        errParam.m_errInfo = "此命令只带有1个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }    
    
    //检查文件是否存在 
    if (access(sqlParam[1].c_str(), R_OK) !=0)
    {
        errParam.m_errInfo = "文件不存在";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }         
    
    //打开文件 
    fin.open(sqlParam[1].c_str(), ios::in);
    
    if(fin.rdstate() != ios::goodbit)
    {
        fin.clear();
        fin.close();
		errParam.m_errInfo = "文件读取错误";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
	}

    cout << "run file now..." << endl;
    while(!fin.eof())
    {
        cout << "----------------" << endl << "command:" << endl;
        char buffer[500];
        memset(buffer,0,sizeof(buffer));     
        fin.getline(buffer,sizeof(buffer)-1);   
        
        string s = buffer;
        //转换大小写
        for(int k=0;k<strlen(buffer);k++)
        {
            buffer[k]=tolower(buffer[k]);
        }  
        if(s.substr(0, 3)=="run")
        {
            errParam.m_errInfo = "run文件内容不能包含run命令";
            errParam.m_errNo = -12345;
            errParam.m_errPos = 1;//sqlParam的第二个参数 
            return;
        }    
        
        if(s!="")
        {
            cout << s << endl;
            dealCmd(s.c_str());
        }    
        cout << "end command" << endl << endl;
    }   
    
    fin.clear();
    fin.close();
    
    cout << "run file end" << endl;
}   

void connect(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()<2)
    {
        errParam.m_errInfo = "此命令需带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }    
    
    if(sqlParam.size()>2)
    {
        errParam.m_errInfo = "此命令只带有1个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }    
    if (!isValidMDBName(sqlParam[1]))
    {
        errParam.m_errInfo = "无效的数据库名";
        errParam.m_errNo   = -12345;
        errParam.m_errPos  = 1;//sqlParam的第二个参数 
        return;
    }    
    //断开之前的连接
    Disconnect();
    
    cout << "connecting " << sqlParam[1] << " now..." << endl;
    //重新申请连接
    try
    {
        m_db = new DataBase(sqlParam[1].c_str());
        m_session = m_db->connect();
        m_memMgr = m_db->getMemMgrInstance();
    }
    catch(Mdb_Exception &e)
    {
        cout << "connect " + sqlParam[1] + " error.";
        m_db = NULL;
        m_session = NULL;
        m_memMgr = NULL;
        e.toString();
        return;
    }
    
    m_dbName = sqlParam[1];
    cout << sqlParam[1] << " successfully connected " << endl;
    
}   


void showlock(vector<string>& sqlParam,  ErrParam& errParam)
{
    if(sqlParam.size()<2)
    {
        errParam.m_errInfo = "此命令需带有参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }    
    
    if(sqlParam.size()>2)
    {
        errParam.m_errInfo = "此命令只带有1个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }    
    
    string sTable = sqlParam[1];
    string sIndex;
    
    //检查表名是否以LOCK_打头
    if(sTable.substr(0, 5)!="LOCK_")
    {
        errParam.m_errInfo = "lock表名需以\"LOCK_\"开头";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第二个参数 
        return;
    }
    
    try
    {
        
        //检查表是否存在
        if(!isTableExists(sTable.c_str()))
        {
            errParam.m_errInfo = "lock表不存在";
            errParam.m_errNo = -12345;
            errParam.m_errPos = 1;//sqlParam的第二个参数 
            return;
        }
        
        //表是否是lock表
        //TableDesc* r_pTableDesc;
        //m_memMgr->getTableDescByName(tableName, r_pTableDesc);
        //r_pTableDesc
        
        //sIndex = "IDX_" + sTable + "_SES";
        //pSession->select(sTable.c_str(),sIndex.c_str(),NULL,NULL);
    }
    catch(Mdb_Exception &e)
    {
        e.toString();
        return;
    }
} 


void start(vector<string>& sqlParam,  ErrParam& errParam)
{
    //start database dbName
    if(sqlParam.size()<3)
    {
        errParam.m_errInfo = "此命令必须带有2个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }   
    
    if(sqlParam.size()>3)
    {
        errParam.m_errInfo = "此命令必须带有2个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 2;//sqlParam的第二个参数 
        return;
    }    
    
    if(sqlParam[1]!="database")
    {
        errParam.m_errInfo = "此命令第1个参数必须为\"database\"";
        errParam.m_errNo = -12345;
        errParam.m_errPos = 1;//sqlParam的第1个参数 
        return;
    }  
    if (!isValidMDBName(sqlParam[2]))
    {
        errParam.m_errInfo = "无效的数据库名";
        errParam.m_errNo   = -12345;
        errParam.m_errPos  = 2;//sqlParam的第二个参数 
        return;
    }     
    cout << "start database " << sqlParam[2] << " now..." << endl;
    
    if(sqlParam[2]!=m_dbName)
    {
        //启动别的数据库
        try
        {
            //断开已有链接
            Disconnect();
            
            //重新申请连接
            m_db = new DataBase(sqlParam[2].c_str());
            m_db->start();
            m_session = m_db->connect();
            m_memMgr = m_db->getMemMgrInstance();
            
            m_dbName = sqlParam[2];
            cout << sqlParam[2] << " successfully started " << endl;
        }
        catch(Mdb_Exception &e)
        {
            cout << "start database " + sqlParam[2] + " error."; 
            m_db = NULL;
            m_session = NULL;
            m_memMgr = NULL;
            e.toString();
            return;
        }
    }
    else
    {
        //启动当前数据库
        cout << m_dbName << " database 已经被启动过" << endl;
        return;
    }  
    
}   

void stop(vector<string>& sqlParam,  ErrParam& errParam)
{
    //stop database dbname;
    if(sqlParam.size()<3)
    {
        errParam.m_errInfo = "此命令必须带有2个参数";
        errParam.m_errNo = -12345;
        errParam.m_errPos = sqlParam.size();//sqlParam的第二个参数 
        return;
    }   
    
    if(sqlParam.size()>3)
    {
        errParam.m_errInfo = "此命令必须带有2个参数";
 db->start();
            m_session = m_db->connect();
            m_memMgr = m_db->getMemMgrInstance();
            
            m_dbName = sqlParam[2];
            cout << sqlParam[2] << " successfully started " << endl;
        }
        catch(Mdb_Exception &e)
        {
            cout << "start database " + sqlParam[2] + " error."; 
            m_db = NULL;
            m_session = NULL;
            m_memMgr = NULL;
            e.toString();
            return;
        }
    }
    else
    {
        //鍚