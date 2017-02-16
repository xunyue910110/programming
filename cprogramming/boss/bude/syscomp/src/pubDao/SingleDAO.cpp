#include "SingleDAO.h"

#include <map>

#include "mwci/CFmlBuf.h"

using std::map;


///
long SingleDAO::jselect(CFmlBuf& buf, const string& sqlRef)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);

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
					string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
					errmsg += __tabName;
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
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

    return iRecCnt;
}

///
long SingleDAO::jselect(CFmlBuf& buf, const string& sqlRef, const string& swhere)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef, swhere);

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
					string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
					errmsg += __tabName;
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
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

	return iRecCnt;
}

///◆＝＝
///
long SingleDAO::jselecta(CFmlBuf& buf, const string& sqlRef)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);

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
				
		//◆
		CFmlBuf abuf(8*1024), tempBuf(2*1024);
		int k = 0;
		
		while ( Fetch() ) 
		{
			map<string, int>::iterator iter = _map_fields.begin();

			for ( ; iter!=_map_fields.end(); iter++)
			{
				if (iter->second == FLD_STRING) 
				{
					tempBuf.SetString((iter->first).c_str(), GetString(iter->first), k);
				}
				else if (iter->second == FLD_LONG) 
				{
					tempBuf.SetInt((iter->first).c_str(), GetInt(iter->first), k);
				}
				else  //此处应该抛出异常
				{
					string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
					errmsg += __tabName;
					errmsg += ", FML字段类型未发现!";
					THROW_C(DAOException, 90000, errmsg.c_str());
				}
			}
			
			//
			iRecCnt++;	
			k++;
			
			if ((iRecCnt+1)%200 == 0)
			{
				abuf.Append(tempBuf);
				
				k = 0;
				tempBuf.ClearFmlValue();	
			}				
		}
		abuf.Append(tempBuf);
		
		//◆
		buf.Append(abuf);
	} 
	catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

    return iRecCnt;
}


///
long SingleDAO::jselecta(CFmlBuf& buf, const string& sqlRef, const string& swhere)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef, swhere);

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

		//◆
		CFmlBuf abuf(8*1024), tempBuf(2*1024);
		int k = 0;

		while ( Fetch() ) 
		{
			map<string, int>::iterator iter = _map_fields.begin();

			for ( ; iter!=_map_fields.end(); iter++)
			{
				if (iter->second == FLD_STRING) 
				{
					tempBuf.SetString((iter->first).c_str(), GetString(iter->first), iRecCnt);
				}
				else if (iter->second == FLD_LONG) 
				{
					tempBuf.SetInt((iter->first).c_str(), GetInt(iter->first), iRecCnt);
				}
				else  //此处应该抛出异常
				{
					string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
					errmsg += __tabName;
					errmsg += ", FML字段类型未发现!";
					THROW_C(DAOException, 90000, errmsg.c_str());
				}
			}

			//
			iRecCnt++;					
			k++;
			
			if ((iRecCnt+1)%300 == 0)
			{
				abuf.Append(tempBuf);
				
				k = 0;
				tempBuf.ClearFmlValue();	
			}				
		}
		abuf.Append(tempBuf);
		
		//◆
		buf.Append(abuf);
	} 
	catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

	return iRecCnt;
}


///
long SingleDAO::jselpage(CFmlBuf& buf, const string& sqlRef, int page, int rowsPerPage)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);

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
					string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
					errmsg += __tabName;
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
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(SingleDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

	return iRecCnt;
}

///
long SingleDAO::jcount(const string& sqlRef)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);
		
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
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName;
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return 0;
}

///
long SingleDAO::jcount(const string& sqlRef, const string& swhere)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef, swhere);
		
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
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return 0;
}

///
long SingleDAO::jupdate(const string& sqlRef)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);
    	AddSql(sstmt);

		return ExecUpdate();
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

	//
	return 0;
}

///
long SingleDAO::jdelete(const string& sqlRef)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);
		AddSql(sstmt);

		return ExecDelete();
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

	//
	return 0;
}

long SingleDAO::jinsert(const string& sqlRef, int size)
{
	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);
		AddSql(sstmt);

		return ExecExt(sqlRef, size);
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(SingleDAO).TAB_NAME(SingleDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

	//
	return 0;
}

