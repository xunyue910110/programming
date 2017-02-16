#include <string.h>
#include "GetSeqID.h"

using namespace std;
using namespace UDBI;

/********************************************************************
*函数：
*功能：
*返回：
********************************************************************/
GetSeqID::GetSeqID(const string SeqCode,const string SeqCount)
{
	str_SeqCode = SeqCode;
	str_SeqCount = SeqCount;
	i_ResultCode = -1;
	str_ResultInfo = "获取序列号：";
	
	pcon = jdb::getConnect();
}
/********************************************************************
*函数：
*功能：
*返回：
********************************************************************/
GetSeqID::~GetSeqID()
{
	
}
/********************************************************************
*函数：
*功能：
*返回：
********************************************************************/
string GetSeqID::GetDateSequencesId()
{
	string sqlnumber;
	string Sql = "SELECT to_char(trunc(SYSDATE),'yyyymmdd')||LPAD(SEQ_"+str_SeqCode+".nextval,"+str_SeqCount+",'0') A	FROM DUAL";
	
	JTable * TGetDSeqID = pcon->CreateJTable();
	try
	{
		TGetDSeqID->AddSql(Sql);
		TGetDSeqID->ExecSelect();
		if(TGetDSeqID->Fetch()==false)
		{
			i_ResultCode = -1;
			delete TGetDSeqID;
			str_ResultInfo = str_ResultInfo+"获取"+str_SeqCode+"的相应ID失败！";
			return "GetSeqFail";
		}
		else
		{
			i_ResultCode = 0;
			sqlnumber = TGetDSeqID->GetString("A");
			delete TGetDSeqID;
			str_ResultInfo = str_ResultInfo+"成功！";
			return sqlnumber;
		}
	}
	catch(DAOException &ex)
	{
		i_ResultCode = -1;
		str_ResultInfo = str_ResultInfo+"获取"+str_SeqCode+"的相应ID失败！";
		delete TGetDSeqID;
		return "GetSeqFail";
	}
}

/********************************************************************
*函数：
*功能：
*返回：
********************************************************************/
string GetSeqID::GetSequencesId()
{
	string sqlnumber;
	string Sql = "SELECT to_char(LPAD(SEQ_"+str_SeqCode+".nextval,"+str_SeqCount+",'0') A	FROM DUAL";
	JTable * TGetDSeqID = pcon->CreateJTable();
	try
	{
		TGetDSeqID->AddSql(Sql);
		TGetDSeqID->ExecSelect();
		if(TGetDSeqID->Fetch()==false)
		{
			i_ResultCode = -1;
			delete TGetDSeqID;
			str_ResultInfo = str_ResultInfo+"获取"+str_SeqCode+"的相应ID失败！";
			return "GetSeqFail";
		}
		else
		{
			i_ResultCode = 0;
			sqlnumber = TGetDSeqID->GetString("A");
			delete TGetDSeqID;
			str_ResultInfo = str_ResultInfo+"成功！";
			return sqlnumber;
		}
	}
	catch(Exception &ex)
	{
		i_ResultCode = -1;
		str_ResultInfo = str_ResultInfo+"获取"+str_SeqCode+"的相应ID失败！";
		delete TGetDSeqID;
		return "GetSeqFail";
	}
}
