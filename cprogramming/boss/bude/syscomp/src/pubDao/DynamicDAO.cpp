#include "DynamicDAO.h"

#include <map>

#include "mwci/CFmlBuf.h"

using std::map;

///
inline string _clause(const string& fldName, int type)
{
    string _sss = "" ;
    string vfiled = ":V" + fldName;

    if (type == 1)
    {
        _sss = fldName + " = TO_DATE(" + vfiled + ", 'YYYY-MM-DD HH24:MI:SS')";
    }
    else if (type == 2)
    {
        _sss = fldName + " = TO_NUMBER(" + vfiled + ")";
    }
    else
    {
        _sss = fldName + "=" + vfiled ;
    }
    
    //
    return _sss;
}


///
void DynamicDAO::appendUpdate(const string& fldName, const string& value, int type)
{
    if (__appendUpdate.length() != 0)
    {
        __appendUpdate += ", ";
    }
	
	__appendUpdate += _clause(fldName, type);
	
	string vfield = ":V" + fldName;
	SetParam(vfield, value);
}

///
void DynamicDAO::appendUpdate(CFmlBuf& buf, const string& fldName, int type)
{
    if (__appendUpdate.length() != 0)
    {
	    __appendUpdate += ", ";
	}

    __appendUpdate += _clause(fldName, type);

	//if ( buf.IsFldExist(fldName.c_str()) ) {
		string _vvv = buf.GetString(fldName.c_str());
	//}

	string vfield = ":V" + fldName;
    SetParam(vfield, _vvv);
	
}

///
void DynamicDAO::appendWhere(const string& andclause)
{
	__appendWhere += " AND " + andclause;
}

///
void DynamicDAO::appendWhere(CFmlBuf& buf, const string& fldName, int type)
{
    __appendWhere += " AND " + _clause( fldName, type );
	
	//if ( buf.IsFldExist(fldName.c_str()) ) {
		string _vvv = buf.GetString(fldName.c_str());
	//}
	
	string vfield = ":V" + fldName;
    SetParam(vfield, _vvv);	
}


long DynamicDAO::xselect(CFmlBuf& buf, const string& sqlRef)
{
	typedef map<string, int>::value_type valType;

	///
	int iRecCnt = 0;

	try
	{
		string sstmt;
		JSQL::getSQL(sstmt, __tabName, sqlRef);

		AddSql(sstmt+__appendWhere);

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
					string errmsg("\nDaoName(DynamicDAO).TAB_NAME: ");
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
		string expmsg("\nDaoName(DynamicDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Select, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}
	catch (CFmlException& e)
	{
		string errmsg("\nDaoName(DynamicDAO).TAB_NAME: ");
		errmsg += __tabName;
		THROW_A(DAOException, e, errmsg.c_str());
	}

    return iRecCnt;	
}

long DynamicDAO::xupdate(const string& sqlRef)
{
	try
	{
		string sstmt = "UPDATE " + __tabName;
		sstmt += " SET " + __appendUpdate;
		sstmt += __appendWhere;

    	AddSql(sstmt);

		return ExecUpdate();
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName(DynamicDAO).TAB_NAME: "); 
		expmsg += __tabName; 
		expmsg += ", Oper: Update, sqlRef: "+sqlRef+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

	//
	return 0;	
}

