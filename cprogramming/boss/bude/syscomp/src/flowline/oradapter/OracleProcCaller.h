#ifndef _ORACLEPROCCALLER_H_
#define _ORACLEPROCCALLER_H_

#include "flowline/flbase.h"
#include "base/OcciUtil.h"

namespace flowline {
namespace oradapter {
// 例：create or replace procedure P_FL_XX_Checker( 
//        iv_gdparam           IN      varchar2(255),--固定参数
//        iv_opdate            IN      varchar2(20), --操作日期
//        ov_resultcode	       OUT     NUMBER，      --返回码:     0 /错误码
//        ov_resultinfo	       OUT     VARCHAR2,     --返回信息:   什么错误
// )
class OracleProcCaller{
	Connection* con;
public:
	OracleProcCaller(Connection* occicnn){
	    con = occicnn;
	};
	virtual ~OracleProcCaller(){};
	//执行
	int callProcedure(const string &procname, const string& gdparam, const string& opdate,
				int& resultcode, string& resultinfo){
					
		ASSERT(con!=0);	
		int updateCount = 0;
		int errcode = -1;
		string errinfo;		
		if(procname.empty()) {
		   resultcode = errcode;
		   resultinfo = "存储过程名为空或不合法:";
		   resultinfo += procname;
		   return -1;
		}
		
		ostringstream sst; 
		sst << "BEGIN  " << procname << "(:v1,:v2,:v3,:v4); END;";
		string callsql = sst.str();
		
		Statement *stmt = con->createStatement(callsql);
		cout << "执行存储过程:" << stmt->getSQL() << endl;
		
		
		try {
			
			stmt->setMaxParamSize (1, 255);
			stmt->setString (1, gdparam);
			stmt->setMaxParamSize (2, 20);
			stmt->setString (2, opdate);
			
			//可能不需要初始值
			stmt->registerOutParam (3, OCCIINT);
			 //设置输出类型和宽度，初始值
			stmt->registerOutParam (4, OCCISTRING, 255, "");
			updateCount = stmt->executeUpdate();
			 
			//cout << "更新记数:" << updateCount << endl;				
			errcode = stmt->getInt(3);
			errinfo = stmt->getString(4);
			con->terminateStatement(stmt);//一定要释放Statement
			cout << "Invoking ProcCaller sucess. errcode: " << errcode  << " errinfo: " << errinfo << endl;
		} catch(exception& e){
			cout << e << endl;
			errinfo = e.what();			
			con->terminateStatement(stmt);//一定要释放Statement
		    cout << "Invoking ProcCaller ERROR. errcode: " << errcode  << " errinfo: " << errinfo << endl;
        }
      
		resultcode = errcode;
		resultinfo = errinfo;
	    return updateCount;
	};
	
	
};//class
};//name
};//name
#endif //_ORACLEPROCCALLER_H_
