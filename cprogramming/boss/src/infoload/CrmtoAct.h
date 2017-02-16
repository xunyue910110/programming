#ifndef _CrmtoAct_H_
#define _CrmtoAct_H_

#include "InfoPublic.h"
#include "DbCommon.h"
class DbInterface;
class CrmtoAct
{
	public:
		CrmtoAct();
		virtual ~CrmtoAct();
		bool BeginPro(const string  &proname,const string& tablename,const int& infotype,string &err_info);
		bool EndPro(const string  &proname,const string& tablename,const int& infotype,string &err_info);
		void Cinit(DbInterface *csrcdbi=NULL,DbInterface *cobjdes=NULL);
		void LoadData(const string& selectsql, const string& updatesql);
		bool SynoNms(const string& proname,const int& infotype, string &err_info);
	private:
			DbInterface *m_csrcdbi;
			DbInterface *m_cobjdes;	
			short *m_ind;		 				
};
#endif
