#include <vector>
#include <string>
#include <fstream> 
#include <iostream>
#include "Session.h"
#include "DataBase.h"
#include "MemManager.h"
#include "base/Types.h"
#include "base/SysParam.h"

SysParam *pSysParam;
SysParam *pSysParam2;


#define GET_PARAM(X,Y)  name = Y; if (!pSysParam->getValue(X, name, value)) { cout << "pSysParam->getValue err!"; return -1; };
#define GET_PARAM2(X,Y)  name2 = Y; if (!pSysParam2->getValue(X, name2, value2)) { cout << "pSysParam2->getValue err!"; return -1; };
                                                                      
string    D, C;         
string    name;      
string    value;     

string    D2, C2; 
string    name2;  
string    value2;

//enum COLUMN_VAR_TYPE 
//{ 
//    VAR_TYPE_INT2=0,  //2位整数:  2个字节
//    VAR_TYPE_INT,   //整数： sizeof(int)
//    VAR_TYPE_DATE,//2个整型数：2*sizeof(int)
//    VAR_TYPE_LONG,  //长整数:sizeof(long)
//    VAR_TYPE_REAL,  //浮点数:sizeof(float)
//    VAR_TYPE_NUMSTR,//数字字符串：采用BCD码压缩
//    VAR_TYPE_VSTR,  //字符串:长度可定义
//    VAR_TYPE_UNKNOW //未知
//};

COLUMN_VAR_TYPE  StringToEnum(const string& strValueType)
{
    if(strcasecmp("var_type_int2", strValueType.c_str())==0)
    return VAR_TYPE_INT2;
 
    else if(strcasecmp("var_type_int", strValueType.c_str())==0) 
    return VAR_TYPE_INT;

    else if(strcasecmp("var_type_date", strValueType.c_str())==0) 
    return VAR_TYPE_DATE;

    else if(strcasecmp("var_type_long", strValueType.c_str())==0) 
    return VAR_TYPE_LONG;

    else if(strcasecmp("var_type_real", strValueType.c_str())==0) 
    return VAR_TYPE_REAL;

    else if(strcasecmp("var_type_numstr", strValueType.c_str())==0) 
    return VAR_TYPE_NUMSTR;
 
    else if(strcasecmp("var_type_vstr", strValueType.c_str())==0) 
    return VAR_TYPE_VSTR;

    else
    {
        //未知类型报错
        return VAR_TYPE_UNKNOW;
    }  
}  

/*
 #define GET_PARAM(X,Y)  name = Y;        \
if (!pSysParam->getValue(X, name, value)) \
{                                         \
   return false;                          \
};
*/

vector<TableDef> tabledefVector;

bool createTable()	                                                                                                                           
{
	D  = SECTDELIM;                                                                                                                 
	D += "MdbCreateInfo" ;                                                                                                         
	D += SECTDELIM;                                                                                                            
	D += "table";
	
	GET_PARAM(D,"tableNum");                                                                                              
	int tableNum = atoi(value.c_str());

	for(int i=1; i<=tableNum; i++)
	{
		TableDef tabledef;
		
		char ch[100];
		sprintf(ch, "table%d", i);
		string stmp = ch;
		C = D;       //创建的table可能有多个
		C += SECTDELIM;
		C += stmp; 
		
		GET_PARAM(C, "tableName");
		sprintf(tabledef.m_tableName, "%s", value.c_str()); //表名   
		GET_PARAM(C, "tableType");
		tabledef.m_tableType = DATA_TABLE;          //表类型写死
		GET_PARAM(C, "columnNum");
		tabledef.m_columnNum = atoi(value.c_str()); //表字段数目   
	
		for(int j=1; j<=tabledef.m_columnNum; j++)
		{
			ColumnDef columnDef;
			
			char ch[100];
			sprintf(ch, "column%d", j);
			string stmp = ch;
			//C += SECTDELIM;
			//C += stmp;

			GET_PARAM(C + SECTDELIM + stmp, "columnName");
			sprintf(columnDef.m_name,"%s",value.c_str());    //列名
			GET_PARAM(C + SECTDELIM + stmp, "columnType");
			columnDef.m_type = StringToEnum(value);          //列类型
			GET_PARAM(C + SECTDELIM + stmp, "columnLength");
			columnDef.m_len = atoi(value.c_str());           //列长度
			
			tabledef.m_columnList[j-1] = columnDef; 
		}
		
		GET_PARAM(C, "spaceNum");
		int tablespaceNum = atoi(value.c_str()); 
		tabledef.m_spaceNum = tablespaceNum;
		
		for(int k=1; k <= tablespaceNum; k++)
		{
			char ch[100];              
			sprintf(ch, "spaceName%d", k);
			string stmp = ch;       
			GET_PARAM(C, stmp);   
			strcpy(tabledef.m_spaceList[k-1],value.c_str());   //表空间名
		}
		
		GET_PARAM(C, "keyClumnNum");
		tabledef.m_keyFlag = 0;
		tabledef.m_keyClumnNum = atoi(value.c_str());
		for(int l = 1; l <= tabledef.m_keyClumnNum; l++)
		{
			if(tabledef.m_keyFlag == 0)tabledef.m_keyFlag = 1;
				
			char ch[100];
			sprintf(ch, "keyClumn%d", l);
			string stmp = ch;
			GET_PARAM(C, stmp);

			sprintf(tabledef.m_keyColumnList[l - 1],"%s", tabledef.m_columnList[atoi(value.c_str()) - 1].m_name);			
		}
		 	
		//写死 
		tabledef.m_lockType=1;     
		tabledef.m_logType=1;	   
		
		//表创建完成
		tabledefVector.push_back(tabledef);
	}         
	
	
	return true;                                                                                   
}


vector<IndexDef> indexdefVector;


bool createIndex()
{	
	D2  = SECTDELIM;                                                                                                                 
	D2 += "MdbCreateInfo" ;                                                                                                         
	D2 += SECTDELIM;                                                                                                            
	D2 += "index";
	
	GET_PARAM2(D2, "indexNum"); 
	int indexNum = atoi(value2.c_str());
	
	for(int j = 1; j <= indexNum; j++)
	{
		IndexDef indexdef;
		char ch[100];
		sprintf(ch, "index%d", j);
		string stmp = ch;
		C2  = D2;		//创建的index可能有多个
		C2 += SECTDELIM;
		C2 += stmp; 	

		GET_PARAM2(C2, "indexName");
		sprintf(indexdef.m_indexName,"%s",value2.c_str());

		GET_PARAM2(C2, "indexType");
		indexdef.m_indexType = HASH_INDEX;   //索引类型写死
		
		GET_PARAM2(C2, "hashSize");
		indexdef.m_hashSize = atoi(value2.c_str());
		
		GET_PARAM2(C2, "tableName");	
		sprintf(indexdef.m_tableName, "%s", value2.c_str());
		
		GET_PARAM2(C2, "columnNum");
		indexdef.m_columnNum = atoi(value2.c_str());
		
		for(int i = 1; i<= indexdef.m_columnNum; i++)
		{
			char ch[100];
			sprintf(ch, "column%d", i);
			string stmp = ch;		
			GET_PARAM2(C2, stmp);

			sprintf(indexdef.m_columnList[i-1],"%s",value2.c_str());
		}
		
		GET_PARAM2(C2, "spaceNum");                                             
		int tablespaceNum = atoi(value2.c_str());                              
		indexdef.m_spaceNum = tablespaceNum;                                  
		                                                                      
		for(int k=1; k <= tablespaceNum; k++)                                 
		{                                                                     
			char ch[100];                                                     
			sprintf(ch, "spaceName%d", k);                                    
			string stmp = ch;                                                 
			GET_PARAM2(C2, stmp);                                               
			strcpy(indexdef.m_spaceList[k-1],value2.c_str());   //表空间名     
		}                                                                     
    	
    	GET_PARAM2(C2, "isUnique");
    	indexdef.m_isUnique = atoi(value2.c_str());  
 		
 		indexdefVector.push_back(indexdef);
	}                                                        
	return true;
}


int main(int argc, char *argv[])
{
  string m_tableName = "";
  string m_indexName = "";

  char *r_pos = NULL;
  char m_inputParam[128];
  //默认无参数输入
  if(argc==1)
  {
  	m_tableName = "table.cfg";
  	m_indexName = "index.cfg";
  }
  else if(argc==2)
  {
    memset(m_inputParam,0,sizeof(m_inputParam));
    strcpy(m_inputParam ,argv[1]);
    r_pos = strstr(m_inputParam,"index");
    if(r_pos != NULL)
    {
      m_indexName = argv[1];
    }
    else
    {
      m_tableName = argv[1];
    }
  }
  else
  {
    memset(m_inputParam,0,sizeof(m_inputParam));
    strcpy(m_inputParam ,argv[1]);
    r_pos = strstr(m_inputParam,"index");
    if(r_pos != NULL)
    {
      m_tableName = argv[2];
      m_indexName = argv[1];
    }
    else
    {
      m_tableName = argv[1];
      m_indexName = argv[2];
    }
  }

  string sDataName;

if(m_tableName.length()>0)
{
	cout << "========================================================" << endl;
	cout << "create table now..." << endl;
	
	pSysParam = new SysParam;                                                                                                
	//pSysParam->initialize("table.cfg"); 
	pSysParam->initialize((char *)m_tableName.c_str());

	DataBase *pDB = NULL;
	Session *pSession = NULL;
	
	D  = SECTDELIM;                                                                                                                 
	D += "MdbCreateInfo" ;                                                                                                         
	D += SECTDELIM;                                                                                                            
	D += "table";
	
	GET_PARAM(D, "dataBase"); 
	sDataName = value;
	
	try
    {
        pDB = new DataBase(sDataName.c_str());    //库名
        pSession = pDB->connect();
    }
    catch(Mdb_Exception e)
    {

        cout << "connect " << sDataName.c_str() << " error." << endl;

        e.toString();

        return -1;

    }
	                                                           
	createTable();
			
	vector<TableDef>::iterator iter;
	for(iter = tabledefVector.begin(); iter != tabledefVector.end();iter++)	
	{	
		cout << *iter;
		try
		{pSession->createTable(*iter);}
		catch(Mdb_Exception e)
        {
            pDB->disconnect(pSession);
            cout << "create table " << (*iter).m_tableName << "error." << endl;
            e.toString();
            return -1;
        }
	}
	
	pDB->disconnect(pSession);
	pDB = NULL;
    pSession = NULL;
}

	sleep(2);

if(m_indexName.length()>0)
{
	cout << "========================================================" << endl;
	cout << "create index now..." << endl;
	
	pSysParam2 = new SysParam;                                                                                                
	//pSysParam2->initialize("index.cfg"); 
	pSysParam2->initialize((char *)m_indexName.c_str());
	
	D2  = SECTDELIM;                                                                                                                 
	D2 += "MdbCreateInfo" ;                                                                                                         
	D2 += SECTDELIM;                                                                                                            
	D2 += "index";
	
	GET_PARAM2(D2, "dataBase"); 
	sDataName = value2;
	
	DataBase *pDB2 = NULL;
	Session *pSession2 = NULL;
	
	try
    {
        pDB2 = new DataBase(sDataName.c_str());    //库名
        pSession2 = pDB2->connect();
    }
    catch(Mdb_Exception e)
    {

        cout << "connect " << sDataName.c_str() << " error." << endl;

        e.toString();

        return -1;

    }
	
	                                                                 
	vector<IndexDef>::iterator iter2;
	createIndex();		
	
	for(iter2 = indexdefVector.begin(); iter2 != indexdefVector.end();iter2++)
	{
	    cout << *iter2 << endl;
		try
		{	
			pSession2->createIndex(*iter2);
		}
		catch(Mdb_Exception e)
        {
            pDB2->disconnect(pSession2);
            cout << "create index" << (*iter2).m_indexName << " error." << endl;
            e.toString();
            return 0;
        }
	}
	
	
	pDB2->disconnect(pSession2);
	pDB2 = NULL;
    pSession2 = NULL;
}

	return 0;
}
