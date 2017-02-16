#include "StaticDAO.h"

#include <map>

#include "mwci/CFmlBuf.h"

using std::map;
	

///
StaticDAO& StaticDAO::getDAO()
{
	static StaticDAO _dao = StaticDAO();
	
	_dao.Clear();
	return _dao;
}

///
long StaticDAO::jselect(CFmlBuf& buf, const string& sqlRef, const string& tabName)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);

		AddSql(sstmt);

		ExecSelect();
		
		///		
		string fldName;
		int fmlFldType;
		map<string,int>  _map_fields;

		for (int i=0; i<GetFieldCount(); i++) {
			fldName = GetFieldName(i);
			fmlFldType = buf.GetFldType(fldName.c_str());
			
			_map_fields.insert(valType(fldName, fmlFldType));
		}
				
		while ( Fetch() ) 
		{
			map<string, int>::iterator iter = _map_fields.begin();

			for ( ; iter!=_map_fields.end(); iter++)
			{
				if (iter->second == FLD_STRING) 
				{
					buf.SetString((iter->first).c_str(), GetString(iter->first), iRecCnt);
				}
				else if (iter->second == FLD_LONG) 
				{
					buf.SetInt((iter->first).c_str(), GetInt(iter->first), iRecCnt);
				}
				else  //此处应该抛出异常
				{
					string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
					errmsg += tabName;
					errmsg += ", FML字段类型未发现!";
					THROW_C(DAOException, 90000, errmsg.c_str());
				}
			}
			
			//
			iRecCnt++;					
		}
	} 
	catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
		errmsg += tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

    return iRecCnt;
}

///
long StaticDAO::jselect(CFmlBuf& buf, const string& sqlRef, const string& tabName, const string& swhere)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef, swhere);

		AddSql(sstmt);

		ExecSelect();

		///		
		string fldName;
		int fmlFldType;
		map<string,int>  _map_fields;

		for (int i=0; i<GetFieldCount(); i++) {
			fldName = GetFieldName(i);
			fmlFldType = buf.GetFldType(fldName.c_str());

			_map_fields.insert(valType(fldName, fmlFldType));
		}

		while ( Fetch() ) 
		{
			map<string, int>::iterator iter = _map_fields.begin();

			for ( ; iter!=_map_fields.end(); iter++)
			{
				if (iter->second == FLD_STRING) 
				{
					buf.SetString((iter->first).c_str(), GetString(iter->first), iRecCnt);
				}
				else if (iter->second == FLD_LONG) 
				{
					buf.SetInt((iter->first).c_str(), GetInt(iter->first), iRecCnt);
				}
				else  //此处应该抛出异常
				{
					string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
					errmsg += tabName;
					errmsg += ", FML字段类型未发现!";
					THROW_C(DAOException, 90000, errmsg.c_str());
				}
			}

			//
			iRecCnt++;					
		}
	} 
	catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
		errmsg += tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

	return iRecCnt;
}

///
long StaticDAO::jselpage(CFmlBuf& buf, const string& sqlRef, const string& tabName, int page, int rowsPerPage)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);

		string stmt ("SELECT * FROM ( SELECT row_.*, rownum rownum_ FROM ( ");
		stmt += sstmt;
		stmt += " ) row_ WHERE Rownum <= :VPage * :VRowsPerPage ) WHERE rownum_ > (:VPage-1)* :VRowsPerPage";

        SetParam(":VPage", page);
        SetParam(":VRowsPerPage", rowsPerPage);

		AddSql(stmt);

		ExecSelect();

		///		
		string fldName;
		int fmlFldType;
		map<string,int>  _map_fields;

		for (int i=0; i<GetFieldCount(); i++) {
			fldName = GetFieldName(i);
			fmlFldType = buf.GetFldType(fldName.c_str());

			_map_fields.insert(valType(fldName, fmlFldType));
		}

		while ( Fetch() ) 
		{
			map<string, int>::iterator iter = _map_fields.begin();

			for ( ; iter!=_map_fields.end(); iter++)
			{
				if (iter->second == FLD_STRING) 
				{
					buf.SetString((iter->first).c_str(), GetString(iter->first), iRecCnt);
				}
				else if (iter->second == FLD_LONG) 
				{
					buf.SetInt((iter->first).c_str(), GetInt(iter->first), iRecCnt);
				}
				else  //此处应该抛出异常
				{
					string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
					errmsg += tabName;
					errmsg += ", FML字段类型未发现!";
					THROW_C(DAOException, 90000, errmsg.c_str());
				}
			}

			//
			iRecCnt++;					
		}
	} 
	catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(StaticDAO).TAB_NAME: ");
		errmsg += tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

	return iRecCnt;
}

///
long StaticDAO::jcount(const string& sqlRef, const string& tabName)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);
		
		string stmt("select count(*) J_COUNT from ( ");
		stmt += sstmt;
		stmt += " ) ";
		
    	AddSql(stmt);
		
	    ExecSelect();
	
	    if (Fetch())
	    {
			return GetInt("J_COUNT");
	    }
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName;
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return 0;
}

///
long StaticDAO::jcount(const string& sqlRef, const string& tabName, const string& swhere)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef, swhere);
		
		string stmt("select count(*) J_COUNT from ( ");
		stmt += sstmt;
		stmt += " ) ";
		
		AddSql(stmt);
		
	    ExecSelect();
	
	    if (Fetch())
	    {
			return GetInt("J_COUNT");
	    }
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return 0;
}

///
long StaticDAO::jupdate(const string& sqlRef, const string& tabName)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);
    	AddSql(sstmt);

		return ExecUpdate();
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
}

///
long StaticDAO::jdelete(const string& sqlRef, const string& tabName)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);
		AddSql(sstmt);

		return ExecDelete();
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
}

long StaticDAO::jinsert(const string& sqlRef, const string& tabName, int size)
{
//
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, tabName, sqlRef);
		AddSql(sstmt);

		return ExecExt(sqlRef, size);
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(StaticDAO).TAB_NAME: "); 
		expmsg += tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

	return 0;
}


