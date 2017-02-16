#include "MdbDmlSql.h"
#include <ctype.h>
MdbDmlSql::MdbDmlSql(DataBase* r_db,Session* r_session,MemManager* r_memMgr,bool r_lineMode)
{
	m_db = r_db;
	m_session = r_session;
	m_memMgr = r_memMgr;
	m_lineMode = r_lineMode;
	clear();
}
MdbDmlSql::~MdbDmlSql()
{
	clear();
}
void MdbDmlSql::clear()
{
	iFromPos=iTablePos=iWherePos=iLimitPos=iSetPos=iIntoPos=iValuesPos= -1;
	//m_allParts.clear();
	m_selectCols.clear();
	m_indexCols.clear();
	m_indexValues.clear();
	m_insertCols.clear();
	m_insertValues.clear();
	m_expr.clear();
	m_sql.clear();
	m_exprCols.clear();
	m_exprValues.clear();
	m_tableName.clear();
	m_indexName.clear();
	m_tableDesc = NULL;
	iLimitCount = -1;
	m_whereClause.clear();
	m_lowerSql.clear();
	m_indexDesc = NULL;
	m_inndexList.clear();
	m_updateCols.clear();
	m_updateValues.clear();
}
bool MdbDmlSql::getUpdateCols()
{
	int iEnd;
	if (iWherePos == -1) 
		iEnd = m_sql.length() - 1;
	else 
		iEnd = iWherePos;
	
	string strSet = lTrim(rTrim(m_sql.substr(iSetPos+3,iEnd - (iSetPos+3))));
	if (strlen(strSet.c_str()) != strSet.length())
		strSet.resize(strlen(strSet.c_str()));
	
	if (strSet[strSet.length() - 1] != ',')
		strSet+= ',';
	else
	{
		cout<<"set子句中最后逗号“,”后无设置值的语句!"<<endl;
		return false;
	}
	string strtmp,col,val;
	int iCondNum=0;
	for (int i=0;i<strSet.length();i++)
	{
		if (strSet[i] != ',')
		{
			strtmp += strSet[i];
		}
		else
		{
			iCondNum ++;
			strtmp = lTrim(rTrim(strtmp));
			col.clear();val.clear();
			if (!isValidCon(strtmp,col,val))
			{
				cout<<"错误位于set子句第"<<iCondNum<<"个条件"<<endl;
				return false;
			}
			m_updateCols.push_back(col);
			m_updateValues.push_back(val);
			strtmp.clear();
		}
	}
	if (m_updateCols.size() == 0)
	{
		cout<<"set子句缺少条件设置!"<<endl;
		return false;
	}
	return true;
}
bool MdbDmlSql::setSQL(const string& r_sql)
{
	if (m_session == NULL) return false;
	if (r_sql.length() == 0)
		return false;
	clear();
	if (strncasecmp(r_sql.c_str(),"select",6) == 0)
	{
		m_sqlType = SQLTYPE_SELECT;
	}
	//else if (strncasecmp(r_sql.c_str(),"insert",6) == 0)
	//{
	//	m_sqlType = SQLTYPE_INSERT;
	//}
	else if (strncasecmp(r_sql.c_str(),"update",6) == 0)
	{
		m_sqlType = SQLTYPE_UPDATE;
	}
	else if (strncasecmp(r_sql.c_str(),"delete",6) == 0)
	{
		m_sqlType = SQLTYPE_DELETE;
	}
	else
	{
		//cout<<"仅处理 select/insert/update/delete !"<<endl;
		return false;
	}
	m_sql = r_sql;
	m_sql = rTrim(lTrim(m_sql));
	if (m_sql[m_sql.length() - 1] == ';')
		m_sql[m_sql.length() - 1] = '\0';
	return true;
}
bool MdbDmlSql::parseSQL()
{
	if (m_sql.length() == 0)
	{
		cout<<"Call setSQL first !"<<endl;
		return false;	
	}

	if (!getAllParts())//解析出 from where limit set 等关键字所在的位置
		return false;
	
	
	if (m_sqlType == SQLTYPE_SELECT)
		return parseSelect();
	else if (m_sqlType == SQLTYPE_INSERT)
		return parseInsert();
	else if (m_sqlType == SQLTYPE_UPDATE)
		return parseUpdate();
	else if (m_sqlType == SQLTYPE_DELETE)
		return parseDelete();
	else
		return false;
}
bool MdbDmlSql::getWhereClause()
{
	m_whereClause.clear();
	if (iWherePos == -1) return true;
	if (iLimitPos != -1)
	{
		m_whereClause = lTrim(rTrim(m_sql.substr(iWherePos + 5,iLimitPos - (iWherePos + 5))));
	}
	else
	{
		m_whereClause = lTrim(rTrim(m_sql.substr(iWherePos + 5,m_sql.length() - (iWherePos + 5))));
	}
	if (m_whereClause.length() == 0)
	{
		cout<<"where后缺少条件!"<<endl;
		return false;
	}
	return true;
}
bool MdbDmlSql::parseSelect()
{
	bool isCount = false;
	if (!getSelectCols(isCount))
		return false;
	if (isCount) return true;

	if (!parseWhereClause(m_whereClause))
		return false;
	if (!doSelect())
		return false;
	return true;
}
bool MdbDmlSql::getInputParams(InputParams *&pIndexParams
	                          ,vector<string>& cols
	                          ,vector<string>& values)
{
	int icnt = cols.size();
	if (icnt == 0) return true;
	short iShort;
	int iInt;
	long iLong;
	float iFloat;

	pIndexParams= new InputParams(icnt);
	for (int i=0;i<icnt;i++)
	{
		int iPos = getTableColPos(cols[i]);
		if (iPos == -1){
			delete pIndexParams;
			pIndexParams = NULL;
			return false;
		}
		switch (m_tableDesc->m_tableDef.m_columnList[iPos].m_type)
		{
			case VAR_TYPE_INT2:
				iShort=atoi(values[i].c_str());
				pIndexParams->setValue(VAR_TYPE_INT2, (void *)&iShort);
				break;
			case VAR_TYPE_INT:
                iInt = atoi(values[i].c_str());
                pIndexParams->setValue(VAR_TYPE_INT, (void *)&iInt);	
                break;
            case VAR_TYPE_DATE:
            	pIndexParams->setValue(VAR_TYPE_DATE, (void *)values[i].c_str());	
            	break;
            case VAR_TYPE_LONG:
            	iLong=atol(values[i].c_str());
            	pIndexParams->setValue(VAR_TYPE_LONG, (void *)&iLong);
            	break;
            case VAR_TYPE_REAL:
            	iFloat=atof(values[i].c_str());
            	pIndexParams->setValue(VAR_TYPE_REAL, (void *)&iFloat);
            	break;
            case VAR_TYPE_NUMSTR:
            	pIndexParams->setValue(VAR_TYPE_NUMSTR, (void *)values[i].c_str());
            	break;
            case VAR_TYPE_VSTR:
            	pIndexParams->setValue(VAR_TYPE_VSTR, (void *)values[i].c_str());
            	break;
		}
	}
	return true;	
}
bool MdbDmlSql::doSelect()
{
	int icnt=m_indexCols.size();
	InputParams *pIndexParams = NULL;
	if (icnt>0)
	{
		if (!getInputParams(pIndexParams,m_indexCols,m_indexValues))
			return false;
	}
	else
	{
        long lRowCount = getTableRowCount(m_tableName);
		if (lRowCount == -1)
        	return false;
        if (iLimitCount == -1 && lRowCount>10000)
        {
        	cout<<"记录总数:"<<lRowCount<<",未用limit关键字限定查询的记录数."<<endl;
        	cout<<"继续查询可能导致程序异常!"<<endl;
        	cout<<"是否继续查询？(y/n)";
        	char choice;
        	cin>>choice;
        	if (choice != 'y' && choice != 'Y')
        		return false;
        }
	}
	try {
		m_session->select(m_tableName.c_str(),m_indexName.c_str(), pIndexParams, NULL, NULL,iLimitCount);
	} catch (Mdb_Exception& e) {
		cout<<"执行查询失败："<<endl;
		e.toString();
		delete pIndexParams;
		return false;
	}
	delete pIndexParams;
	icnt = m_selectCols.size();
    if (m_lineMode)
    {
	    for(int i=0; i<icnt; i++)
	    {
	        cout << m_selectCols[i];
	        if (i != icnt - 1)
	        	cout<<",";
	    }
	    cout << endl;
	}
    vector<COLUMN_VAR_TYPE> t_vColType;
	for(int g=0; g<icnt; g++)
	{
		int iColPos=getTableColPos(m_selectCols[g]);
		t_vColType.push_back(m_tableDesc->m_tableDef.m_columnList[iColPos].m_type);
	}
	long iRowCount=0;
	int lsize;
    //获取记录
    while(m_session->getNextRec())
    {
		iRowCount ++;
    	if (!m_lineMode)
    		printf("第[%ld]条记录:\n",iRowCount);
        void *p;
        
        for(int g=0; g<icnt; g++)
        {
            if(t_vColType[g] == VAR_TYPE_INT2)
            {
                p = new char[2];
            }
            else if (t_vColType[g] == VAR_TYPE_INT)
            {
                p = new char[sizeof(int)];         
            }
            else if (t_vColType[g] == VAR_TYPE_DATE)
                p = new char[sizeof(long)];         
            else if (t_vColType[g] == VAR_TYPE_LONG)
            {
                p = new char[sizeof(long)]; 
            }
            else if (t_vColType[g] == VAR_TYPE_REAL)
            {
                p = new char[sizeof(float)]; 
            }
            else if (t_vColType[g] == VAR_TYPE_NUMSTR)
            {
                ;
            }
            else if (t_vColType[g] == VAR_TYPE_VSTR)
            {
                p = new char[100];
                memset(p, NULL, 100);
            }
            
            m_session->getColumnValue(m_selectCols[g].c_str(), p, lsize);

            if(t_vColType[g] == VAR_TYPE_INT2)
            {
                if (m_lineMode)
                	cout<<(((char*)p)[0]*256+((char*)p)[1])<<",";
                else
                	printf("%40s = %d\n",m_selectCols[g].c_str(),(((char*)p)[0]*256+((char*)p)[1]));
            }
            else if (t_vColType[g] == VAR_TYPE_INT)
            {
                if (m_lineMode)
 	               cout<<*((int *)p)<< ",";         
                else
                	printf("%40s = %d\n",m_selectCols[g].c_str(),*((int *)p));
            }
            else if (t_vColType[g] == VAR_TYPE_DATE)
                ;
            else if (t_vColType[g] == VAR_TYPE_LONG)
            {
                if (m_lineMode)
	                cout<<*((long *)p)<< ",";
                else
                	printf("%40s = %ld\n",m_selectCols[g].c_str(),*((long *)p));
            }
            else if (t_vColType[g] == VAR_TYPE_REAL)
            {
                if (m_lineMode)
	                cout<<*((float *)p)<< ",";
                else
                	printf("%40s = %ld\n",m_selectCols[g].c_str(),*((float *)p));
            }
            else if (t_vColType[g] == VAR_TYPE_NUMSTR)
            {
				char ch[100];
				memset(ch, 0, sizeof(ch));
				memcpy(ch, p, lsize);
                 if (m_lineMode)
					cout << ch << ",";
                else
                	printf("%40s = %s\n",m_selectCols[g].c_str(),ch);
					
            }
            else if (t_vColType[g] == VAR_TYPE_VSTR)
            {
                char ch[100];
                memset(ch, 0, sizeof(ch));
                memcpy(ch, p, lsize);
                if (m_lineMode)
	                cout << ch << ",";
                else
                	printf("%40s = %s\n",m_selectCols[g].c_str(),ch);
            }
            delete[] p;
        }
        if (m_lineMode)
 	       cout << endl;
    }	
    if (m_lineMode)
    	cout<<iRowCount<<" row(s) fetched."<<endl;
	
	return true;
}
bool MdbDmlSql::parseInsert()
{
	return true;
}
bool MdbDmlSql::parseUpdate()
{
	if (!getUpdateCols())
		return false;
	if (!parseWhereClause(m_whereClause))
		return false;
	if (!doUpdate())
		return false;
	return true;
}
bool MdbDmlSql::doUpdate()
{
	if (iWherePos == -1)
	{
		cout<<"未指定where条件，将更新"<<m_tableName<<"所有记录"<<endl;;
		cout<<"继续执行更新操作？(y/n)";
    	char choice;
    	cin>>choice;
    	if (choice != 'y' && choice != 'Y')
    		return false;		
	}
	InputParams *pUpdateParams = NULL;
	if (!getInputParams(pUpdateParams,m_updateCols,m_updateValues))
		return false;
	InputParams *pIndexParams = NULL;
	if (!getInputParams(pIndexParams,m_indexCols,m_indexValues))
		return false;
	int iUpdateCount=0;
	try {
		m_session->begin(m_tableName.c_str());
		iUpdateCount = m_session->update(m_tableName.c_str()
		                                ,m_indexName.c_str()
		                                ,pIndexParams
		                                ,m_updateCols
		                                ,pUpdateParams,NULL,NULL);
		m_session->commit();
		cout<<iUpdateCount<<" row(s) updated."<<endl;
	} catch (Mdb_Exception& e) {
		cout<<"执行更新失败："<<endl;
		e.toString();
		m_session->rollBack();
		delete pIndexParams;
		delete pUpdateParams;
		return false;
	}
	delete pIndexParams;
	delete pUpdateParams;
	return true;
}
bool MdbDmlSql::doDelete()
{
	if (iWherePos == -1)
	{
		cout<<"未指定where条件，将删除"<<m_tableName<<"所有记录"<<endl;;
		cout<<"继续执行删除操作？(y/n)";
    	char choice;
    	cin>>choice;
    	if (choice != 'y' && choice != 'Y')
    		return false;		
	}
	InputParams *pIndexParams = NULL;
	if (!getInputParams(pIndexParams,m_indexCols,m_indexValues))
		return false;
	int iDeleteCount=0;
	try {
		m_session->begin(m_tableName.c_str());
		iDeleteCount = m_session->deleteRec(m_tableName.c_str()
		                                   ,m_indexName.c_str()
		                                   ,pIndexParams
		                                   ,NULL
		                                   ,NULL);
		m_session->commit();
		cout<<iDeleteCount<<" row(s) deleted."<<endl;
	} catch (Mdb_Exception& e) {
		cout<<"执行删除失败："<<endl;
		e.toString();
		m_session->rollBack();
		delete pIndexParams;
		return false;
	}
	delete pIndexParams;
	return true;
	
}
bool MdbDmlSql::parseDelete()
{
	if (!parseWhereClause(m_whereClause))
		return false;
	if (!doDelete())
		return false;	
	return true;
}
long MdbDmlSql::getTableRowCount(const string& r_tableName)
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
bool MdbDmlSql::getSelectCols(bool& isCount)
{
	string cols=rTrim(lTrim(m_sql.substr(6,iFromPos - 6)));
	if (cols == "count(*)")
	{
		if (iWherePos != -1){
			cout<<"count(*)不支持指定 where 条件!"<<endl;
			return false;
		}
		long cnt = getTableRowCount(m_tableName);
		if (cnt == -1) return false;
		cout<<cnt<<endl;
		isCount = true;
		return true;
	}
	if (cols != "*")
	{
		string col;
		for (int i=0; i<cols.length(); i++)
		{
			if (cols[i] != ',')
			{
				col+=toupper(cols[i]);
			}
			if (cols[i] == ',' || i == cols.length() - 1)
			{
				col = lTrim(rTrim(col));
				if (!isValidColumn(col)){
					cout<<"select from 之间无效的字段名："<<col<<endl;
					return false;
				}
				m_selectCols.push_back(col);
				col.clear();
			}
		}
	}
	else
	{
		for (int i=0;i<m_tableDesc->m_tableDef.m_columnNum;i++)
		{
			m_selectCols.push_back(m_tableDesc->m_tableDef.m_columnList[i].m_name);
		}
	}
	return true;
}
bool MdbDmlSql::isValidColumn(const string& col)
{
	int i;
	for (i=0;i<m_tableDesc->m_tableDef.m_columnNum;i++)
	{
		if (col == m_tableDesc->m_tableDef.m_columnList[i].m_name)
			break;
	}
	if (i == m_tableDesc->m_tableDef.m_columnNum)
	{
		return false;
	}
	return true;
}
void MdbDmlSql::getLowerSQL(const string& r_old,string& r_new)
{
	r_new = r_old;
	for (int i=0; i<r_new.length(); i++)
	{
		if (isBlank(r_new[i])) r_new[i] = ' ';//tab回车换行等 替换为空格
		r_new[i] = tolower(r_new[i]);//转换为小写
	}
	return;
}
bool MdbDmlSql::getAllParts()
{
	getLowerSQL(m_sql,m_lowerSql);
	int iSqlLen=m_lowerSql.length();
	
	const char * pFrom  = NULL;
	const char * pWhere = NULL;
	const char * pLimit = NULL;
	const char * pSet   = NULL;
	int iStart=0;
	switch (m_sqlType)
	{
		case SQLTYPE_SELECT:
			//获取from所在的位置
			pFrom = strstr(m_lowerSql.c_str()," from ");
			if (pFrom == NULL){
				cout<<"SELECT 缺少from关键字"<<endl;
				return false;
			}
			iFromPos = (pFrom + 1) - (char *)&(m_lowerSql[0]);
			//获取where所在的位置
			pWhere = strstr(m_lowerSql.c_str()," where ");
			if (pWhere != NULL){
				iWherePos = (pWhere + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取limit关键字所在的位置
			pLimit = strstr(m_lowerSql.c_str()," limit ");
			if (pLimit != NULL){
				iLimitPos = (pLimit + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取表名和表名起始位置
			for (int i=iFromPos + 4;i<iSqlLen;i++)
			{
				if (!isBlank(m_lowerSql[i])){
					if (iTablePos == -1) iTablePos = i;
					m_tableName+=toupper(m_lowerSql[i]);
				} 
				else {
					if (iTablePos != -1) break;						
				}
			}
			if (iTablePos == iWherePos || iTablePos == iLimitPos || iTablePos == -1){
				cout<<"缺少表名！"<<endl;
				return false;
			}
			if (!getTableDesc()) return false;
			if (iLimitPos != -1)
			{
				string limitCount = m_lowerSql.substr(iLimitPos + 5,m_lowerSql.length() - (iLimitPos + 5 + 1));
				limitCount = lTrim(rTrim(limitCount));
				if (!isNumber(limitCount)){
					cout<<"limit 后请使用有效的整数！"<<endl;
					return false;
				}
				iLimitCount = atoi(limitCount.c_str());
			}
			//else	cout<<"table name:"<<m_tableName<<endl;
			break;
		case SQLTYPE_INSERT:
		case SQLTYPE_UPDATE:
			//获取from所在的位置
			pFrom = strstr(m_lowerSql.c_str()," from ");
			if (pFrom != NULL){
				iFromPos = (pFrom + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取set所在位置
			pSet = strstr(m_lowerSql.c_str()," set ");
			if (pSet != NULL){
				iSetPos = (pSet + 1) - (char *)&(m_lowerSql[0]);
			} else {
				cout<<"缺少set关键字！"<<endl;
				return false;
			}
			//获取where所在的位置
			pWhere = strstr(m_lowerSql.c_str()," where ");
			if (pWhere != NULL){
				iWherePos = (pWhere + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取limit关键字所在的位置
			pLimit = strstr(m_lowerSql.c_str()," limit ");
			if (pLimit != NULL){
				cout<<"update语句中不支持 limit !"<<endl;
				return false;
			}
			//获取表名和表名起始位置
			if (iFromPos != -1) 
				iStart = iFromPos + 4;
			else
				iStart = 0 + 6;
			for (int i=iStart;i<iSqlLen;i++)
			{
				if (!isBlank(m_lowerSql[i])){
					if (iTablePos == -1) iTablePos = i;
					m_tableName+=toupper(m_lowerSql[i]);
				} 
				else {
					if (iTablePos != -1) break;						
				}
			}
			if (iTablePos == iSetPos || iTablePos == -1){
				cout<<"缺少表名！"<<endl;
				return false;
			}
			if (!getTableDesc()) return false;

			break;
		case SQLTYPE_DELETE:
			//获取from所在的位置
			pFrom = strstr(m_lowerSql.c_str()," from ");
			if (pFrom != NULL){
				iFromPos = (pFrom + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取where所在的位置
			pWhere = strstr(m_lowerSql.c_str()," where ");
			if (pWhere != NULL){
				iWherePos = (pWhere + 1) - (char *)&(m_lowerSql[0]);
			}
			//获取表名和表名起始位置
			if (iFromPos != -1) 
				iStart = iFromPos + 4;
			else
				iStart = 0 + 6;
			for (int i=iStart;i<iSqlLen;i++)
			{
				if (!isBlank(m_lowerSql[i])){
					if (iTablePos == -1) iTablePos = i;
					m_tableName+=toupper(m_lowerSql[i]);
				} 
				else {
					if (iTablePos != -1) break;
				}
			}
			if (iTablePos == iWherePos || iTablePos == -1 || m_tableName.length() == 0){
				cout<<"缺少表名！"<<endl;
				return false;
			}
			if (!getTableDesc()) return false;

			break;
	}
	if (!getWhereClause())
		return false;
	return true;
}
bool MdbDmlSql::isNumber(const string& r_src)
{
	for (int i = 0;i<r_src.length(); i++)
	{
		if (r_src[i]<'0' || r_src[i]>'9')
			return false;
	}
	return true;
}
bool MdbDmlSql::parseWhereClause(const string& src)
{
    vector<IndexDef> r_pIndexLis;
    //根据表信息，查找表的索引
    
    m_memMgr->getIndexListByTable(m_tableName.c_str(), r_pIndexLis); 
    if (r_pIndexLis.size()>0)
    	m_indexName = r_pIndexLis[0].m_indexName;
    else
    {
    	return false;
    }

	if (iWherePos == -1) 
	{
		if (!getIndexDesc())
			return false;
		return true;
	}
	int iLen = src.length();
	
	vector<string> t_vall;
	string strtmp;
	
	//a = 1 and b=2 and ( e =5 or f= 6 )
	//解析出空格分隔的各个条件
	int i=0;
	while (i<iLen)
	{
		if (!isBlank(src[i]) && src[i] != '(' && src[i] != ')')
		{
			strtmp+=src[i];
			i++;
		}
		else
		{
			while (i<iLen)
			{
				if (isBlank(src[i]) || src[i] == '(' || src[i] == ')')
					i ++;
				else
					break;
			}
			t_vall.push_back(strtmp);
			strtmp.clear();
		}
	}
	if (strtmp.length() > 0)
	{
		t_vall.push_back(strtmp);
	}
	//解析出and分隔的各个条件(字段=值)
	int iStartPos = 0;
	vector<string> t_vcon;
	if (t_vall[t_vall.size() - 1] != "and")
		t_vall.push_back("and");
	else
	{
		cout<<"where条件最后的and后缺少条件！"<<endl;
		return false;
	}
	for (i=0;i<t_vall.size();i++)
	{
		if (strcasecmp(t_vall[i].c_str(),"and") == 0 || strcasecmp(t_vall[i].c_str(),"or") == 0)
		{
			strtmp.clear();
			for (int j=iStartPos;j<i;j++)
			{
				strtmp += t_vall[j];
			}
			if (strtmp.length() == 0)
			{
				cout<<"where条件中，第"<<i<<"个"<<t_vall[i]<<"前缺少条件"<<endl;
				return false;
			}
			t_vcon.push_back(strtmp);
			iStartPos = i+1;
		}
	}
	//检验条件的有效性
	string col,val;
	for (int i=0;i<t_vcon.size();i++)
	{
		col.clear();val.clear();
		if (!isValidCon(t_vcon[i],col,val))
		{
			cout<<"错误位于第 "<<i+1<<" 个 where 条件!"<<endl;
			return false;
		}
	}

    bool t_bFoundIndex=false;
    for(vector<IndexDef>::iterator itr=r_pIndexLis.begin();
        itr!=r_pIndexLis.end(); itr++)
    {
    	m_indexCols.clear();
    	m_indexValues.clear();
    	int i;
        for(i=0;i<itr->m_columnNum;i++)
        {
        	int j;
			for (j=0;j<t_vcon.size();j++)
			{
				col.clear();val.clear();
				isValidCon(t_vcon[j],col,val);
				if (strcasecmp(itr->m_columnList[i],col.c_str()) == 0)
				{
					m_indexCols.push_back(itr->m_columnList[i]);
					m_indexValues.push_back(val);
					break;
				}
			}
        }
        if (m_indexCols.size() > 0)
        { 
        	if (m_indexCols.size() == itr->m_columnNum)
        	{
        		t_bFoundIndex = true;
        		m_indexName = itr->m_indexName;
        		break;
        	}
        }
    }
	if (!t_bFoundIndex)
	{
		cout<<"未找到匹配索引(索引为N个字段，则条件必须为N个条件)！"<<endl;
    	m_indexCols.clear();
    	m_indexValues.clear();
		return false;
	}
	if (!getIndexDesc())
		return false;	
	return true;
}
bool MdbDmlSql::getIndexDesc()
{
	try
	{
		if (!m_memMgr->getIndexDescByName(m_indexName.c_str(),m_indexDesc))
		{
			cout<<"getIndexDescByName error!["<<m_indexName<<"]"<<endl;
			return false;
		}
	} catch (Mdb_Exception& e) {
		cout<<"getIndexDescByName error! -- "<<m_indexName<<endl;
		e.toString();
		return false;
	}
	return true;
}
int MdbDmlSql::getTableColPos(const string& col)
{
	for (int i=0;i<m_tableDesc->m_tableDef.m_columnNum;i++)
	{
		if (strcasecmp(col.c_str(),m_tableDesc->m_tableDef.m_columnList[i].m_name) == 0)
			return i;
	}
	return -1;
}
bool MdbDmlSql::isValidCon(const string& src,string&col,string&val)
{
	string::size_type t_pos = src.find("=");
	if (t_pos == string::npos)
	{
		cout<<"缺少“=”符号!"<<endl;
		return false;
	}
	col = toUpper(src.substr(0,t_pos));
	col = lTrim(rTrim(col));
	if (col.length() == 0)
	{
		cout<<"缺少字段名!"<<endl;
		return false;
	}
	val = src.substr(t_pos+1,src.length() - (t_pos+1));
	val = lTrim(rTrim(val));
	if (val.length() == 0)
	{
		cout<<col<<"缺少字段值!"<<endl;
		return false;
	}
	if (!isValidColumn(col))
	{
		cout<<"无效的字段名:"<<col<<"!"<<endl;
		return false;
	}
	if (val[0]== '\'')
	{
		if (strlen(val.c_str()) != val.length())
		{
			val.resize(strlen(val.c_str()));
		}
		if (val[val.length() - 1] == '\'')
		{
			val = val.substr(1,val.length() - 2);
		}
		else
		{
			cout<<col<<"的值中，“'”前后不匹配!"<<endl;
			return false;
		}
	}
	return true;
}
bool MdbDmlSql::getTableDesc()
{
	if (m_tableName.length() == 0)
		return false;
    try
    {
        if(!m_memMgr->getTableDescByName(m_tableName.c_str(), m_tableDesc))
        {
        	cout<<"无效的表名:"<<m_tableName<<endl;
            return false;
        } 
        return true;
    }
    catch(Mdb_Exception &e)
    {
        e.toString();
    	cout<<"无效的表名！"<<endl;
        return false;
    }
}

string MdbDmlSql::toUpper(const string& str)
{
	string strRet;
	for (unsigned int i=0;i<str.length();i++)
	{
		strRet+=toupper(str[i]);
	}
	return strRet;
}
string MdbDmlSql::toLower(const string& str)
{
	string strRet;
	for (unsigned int i=0;i<str.length();i++)
	{
		strRet+=tolower(str[i]);
	}
	return strRet;
}
string MdbDmlSql::lTrim(const string& src)
{
    int ipos=0;
    int iLen=src.length();
    for(int i=0;i<iLen;i++)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            ipos ++;
        else
            break;
    }
    return src.substr(ipos,iLen - ipos + 1);
}
string MdbDmlSql::rTrim(const string &src)
{
    int  i,iLen;
    iLen=src.length();
    int iCount = 0;
    for(i=iLen - 1;i>=0;i--)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            iCount ++;
        else
            break;
    }
    return src.substr(0,iLen - iCount);	
}
