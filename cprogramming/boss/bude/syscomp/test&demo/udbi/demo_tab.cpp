#include <iostream>
#include <unistd.h>

#include "udbi.h"

/** \file demo_tab.cpp
 *  \brief UDBI组件测试，同时也是UDBI组件的使用样例
 * 
 *  测试数据库表
 *  \code
 *  create table udbi_test (
 *   id  number(3),
 *   name varchar2(20),
 *   type varchar2(3),
 *   dtime date
 *   );
 *  
 *  insert into udbi_test values(1, 'note', 'a0h', sysdate);
 *  insert into udbi_test values(2, 'book', 'a0h', sysdate);
 *  insert into udbi_test values(3, 'word', 'a0h', sysdate);
 *  \endcode
 */

using namespace std;
using namespace UDBI;

JConnect *pcon;

//typedef string[8]  myString ;

void tab_selCycleTest();
void tab_updTest();
void tab_procTest();

void tab_selTestAlias();
void tab_selTestDTY();
void tab_selCycleTestBB();
void tab_selTestS();
void tab_selTest();

/**
 *  \fn main(int argc, char argv[])
 *  \brief 测试主函数
 *  本测试程序基于非XA方式
 */
main(int argc, char argv[])
{
	// 非单件连接实例的初始化，这种初始化方式不支持SQL缓存
	//jdb::init();
	//jdb::Connect("channel", "channel", "channel", con);
	
	// 单件连接实例的初始化，这种初始化方式支持SQL缓存，可以通过传入SQL引用标识执行关联SQL语句
	try 
	{
	jdb::jinit("eboss", "code_user", "code_user");
	pcon = jdb::getConnect();
	
	
	/*cout << "Please input a val of int64_t: " << endl;
	int64_t int64Val;
	char ss[12]="11000000000";
	
	int64Val = atoll(ss);
	//cin >> int64Val;
	cout << "The value of you input is :" << int64Val << endl;
	*/
	
		//tab_procTest();
		
		//tab_updTest();
		//tab_selTestAlias();
		//tab_selTestDTY();
		//tab_selCycleTestBB();
		//tab_selTestS();
		tab_selTest();
	} catch (Exception &er)
	{
		cout << er << endl;
	}
		
	cout << "wait ... " << endl;
	sleep(3);
	
	return 0;	
}

/**
 *  测试字段别名、表别名支持
 *  测试结果：OK
 */
void tab_selTestAlias()
{
	cout << "Test alias (field&table alias)(udbi_test_dty, udbi_test_alias) ... " << endl;
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select a.id IDD, a.kind_id KindID, b.kind_name KindName, a.vip_nbr VipNbr, ");
	pTab->AddSql(" a.soc_nbr SocNbr, a.sex, a.password, a.name, a.remark, ");
	pTab->AddSql(" a.state, a.state_date StateDate");
	pTab->AddSql(" from udbi_test_dty a, udbi_test_alias b ");
	pTab->AddSql(" where a.id=:id and a.kind_id = b.kind_id");
	
	int ival;
	cout << "Please input the id for select: ";
	cin >> ival;
	
	pTab->SetParam(":id", ival);
	
	pTab->ExecSelect();
	
	while(pTab->Fetch()) {
			cout << "IDD : " << pTab->GetInt("IDD") << endl;
			cout << "KIND_ID: " << pTab->GetInt("KINDID") << endl;
			cout << "KIND_NAME: " << pTab->GetString("KINDNAME") << endl;
			cout << "VIP_NBR: " << pTab->GetInt64("VIPNBR") << endl;
			cout << "SOC_NBR: " << pTab->GetInt("SOCNBR") << endl;
			cout << "SEX: " << pTab->GetString("SEX") << endl;
			cout << "PASSWORD: " << pTab->GetString("PASSWORD") << endl;
			cout << "NAME: " << pTab->GetString("NAME") << endl;
			cout << "REMARK: " << pTab->GetString("REMARK") << endl;
			cout << "STATE: " << pTab->GetString("STATE") << endl;
			cout << "STATE_DATE:" << pTab->GetString("STATEDATE") << endl;
	}		

    delete pTab;
    
    cout << "Alias test end ." << endl;
}	


/**
 *  测试数据类型支持
 *  测试结果：
 *  \arg number(<= 10) -> int, string;	GetInt()、GetString()
 *  \arg number(>=11) -> int64_t        GetInt64()
 *  \arg number(>=11) -> string;		GetString()
 *  \arg char(x) -> string;		        GetString()
 *  \arg varchar2 -> string;		    GetString()
 *  \arg date -> string;			    GetString()
 */
void tab_selTestDTY()
{
	cout << "Test data type support(udbi_test_dty) ... " << endl;
	
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date ");
	pTab->AddSql(" from udbi_test_dty where id=:id");
	
	int ival;
	cout << "Please input the id for select(0 is all): ";
	cin >> ival;
	
	if (ival==0) {
		pTab->AddSql("select id, kind_id, vip_nbr, soc_nbr, sex, password, name, remark, state, state_date ");
		pTab->AddSql(" from udbi_test_dty ");
	} else {
		pTab->SetParam(":id", ival);
	} 
	
	pTab->ExecSelect();
	
	while(pTab->Fetch()) {
			cout << "ID : " << pTab->GetInt("ID") << endl;
			cout << "KIND_ID: " << pTab->GetInt("KIND_ID") << endl;
			cout << "VIP_NBR: " << pTab->GetInt64("VIP_NBR") << endl;
			cout << "SOC_NBR: " << pTab->GetInt("SOC_NBR") << endl;
			// cout << "ID : " << pTab->GetString("ID") << endl;
			// cout << "KIND_ID: " << pTab->GetString("KIND_ID") << endl;
			// cout << "VIP_NBR: " << pTab->GetString("VIP_NBR") << endl;
			// cout << "SOC_NBR: " << pTab->GetString("SOC_NBR") << endl;
			cout << "SEX: " << pTab->GetString("SEX") << endl;
			cout << "PASSWORD: " << pTab->GetString("PASSWORD") << endl;
			cout << "NAME: " << pTab->GetString("NAME") << endl;
			cout << "REMARK: " << pTab->GetString("REMARK") << endl;
			cout << "STATE: " << pTab->GetString("STATE") << endl;
			cout << "STATE_DATE:" << pTab->GetString("STATE_DATE") << endl;
	}		

    delete pTab;
    
    cout << "Data type test end ." << endl;
}	

/**
 *  测试SQL语句缓存
 *  测试结果：OK
 */
void tab_selCycleTestBB()
{
	cout << "Test sql statement buffer(udbi_test_cust) ... " << endl;	
	
	JTable *pTab = pcon->CreateJTable("TD_B_PRODUCT");
	
	int ival;
	while (1) {
		cout << "Please input the id for select(0 is exit): ";
		cin >> ival;
			
		if (ival == 0) break;
		int ival1=1;
		string tmpStr1("0001");
		
		pTab->Clear();
		pTab->SetParam(":VPRODUCT_TYPE_CODE", tmpStr1);
		pTab->SetParam(":VTRADEEPARCHYCODE", tmpStr1); 
	
		pTab->ExecSelect("SEL_BY_TYPE");
	
		while(pTab->Fetch()) {
			cout << "PRODUCT_ID : " << pTab->GetInt("PRODUCT_ID") << endl;
			cout << "PRODUCT_NAME: " << pTab->GetString("PRODUCT_NAME") << endl;
		}	
	}	
	
	delete pTab;

	cout << "SQL statement test end." << endl;
}

/**
 *  测试查询语句执行
 */
void tab_selTestS()
{
	// demo code for select 
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select sql_stmt from code_code where sql_ref=:aaa");
	
	string tmp_str;
	cout << "Please input the where_by for select: ";
	cin >> tmp_str;
	
	pTab->SetParam(":aaa", tmp_str);
	
	pTab->ExecSelect();
	
	while(pTab->Fetch()) {
			cout << "STMT: " << pTab->GetString("SQL_STMT") << endl;
	}		
	// end of select 

    delete pTab;
}	

/**
 *  测试查询语句执行
 */
void tab_selTest()
{
	cout << "Test simple select statement(wfj_test) ... " << endl;
	
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select id, name, type, dtime from udbi_test where id=:id and name=:vname");
	
	int ival;
	string sname;
	cout << "Please input the id for select(0 is all): ";
	cin >> ival;
	
	if (ival==0) {
		pTab->AddSql("select id, name, type from udbi_test");
	} else {
		//1: ival = 2;
		//1: sname = "book";
		//1: pTab->SetParam(":id", ival);
		//1: pTab->SetParam(":vname", sname);
		pTab->SetParam(":id", ival);
		pTab->SetParam(":vname", "word");
	} 
	
	pTab->ExecSelect();
	
	while(pTab->Fetch()) {
			cout << "ID : " << pTab->GetInt("ID") << endl;
			cout << "NAME: " << pTab->GetString("NAME") << endl;
			cout << "TYPE: " << pTab->GetString("TYPE") << endl;
			cout << "DTIME:" << pTab->GetString("DTIME") << endl;
	}		

    delete pTab;
	
	cout << "Simple select statement test end." << endl; 
}	


/**
 *  测试查询语句执行, 循环测试
 */
void tab_selCycleTest()
{
	cout << "Test simple select statement (wfj_test, cycle) ... " << endl;
	
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select id, name, type, dtime from wfj_test where id=:id");
	
	int ival;
	while (1) {
		cout << "Please input the id for select(0 is exit): ";
		cin >> ival;
	
		if (ival == 0) break;
		
		pTab->Clear();
		pTab->SetParam(":id", ival);
	
		pTab->ExecSelect("");
	
		while(pTab->Fetch()) {
			cout << "ID : " << pTab->GetInt("ID") << endl;
			cout << "NAME: " << pTab->GetString("NAME") << endl;
			cout << "TYPE: " << pTab->GetString("TYPE") << endl;
			cout << "DTIME:" << pTab->GetString("DTIME") << endl;
		}	
	}	

    delete pTab;
	
	cout << "Select statement test end. " << endl;	
}


/**
 *  测试更新语句执行
 */
void tab_updTest()
{
	cout << "Test simple update statement(udbi_test) ... " << endl;
	
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("update udbi_test set name=:name where id=:id");
	
	int ival;
	string sval;
	
	cout << "Please input the id to update: " ;
	cin >> ival;
	cout << "Please input the name to update: ";
	cin >> sval;
	
	pTab->SetParam(":id", ival);
	pTab->SetParam(":name", sval);

	pcon->TransBegin();
	pTab->ExecUpdate();
	pcon->TransCommit();
	//con.TransRollback();

    delete pTab;
	
	cout << "Update statement test end ." << endl;	
}


/**
 *  测试存储过程调用
 */
void tab_procTest()
{
	cout << "Test proc call (tab_procTest)(wfj_test) ... " << endl;
	
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("BEGIN  proc_test(:id,:name,:vtype,:vret);  END; ");
	
	int ival=1100000004;
	string sval("NewRecord update");
	int iRet=0;
	//要求初始化大于等于3个字符长度字符串，否则ORA-06502: PL/SQL: numeric or value error: character string buffer too small
	// string sType("abc"); 
	//必须初始化，否则ORA-01480: trailing null missing from STR bind value
	//必须大于等于数据库varchar2长度定义＋1，否则ORA-06502: PL/SQL: numeric or value error: character string buffer too small
	char sbuf[4]="";  
	pTab->SetParam(":id", ival);
	pTab->SetParam(":name", sval);
	//pTab->SetParam(":vtype", sType);
	pTab->SetParam(":vtype", sbuf, 4);  //此处的长度必须与定义大小一致，如果小于定义（如3），则报非空串错误
	pTab->SetParam(":vret", iRet);
	
	pcon->TransBegin();
	pTab->ExecInsert();
	pcon->TransCommit();	
	
	//cout << "Result: type, " << sType << "; ret, " << iRet << endl;
	cout << "Result: type, " << sbuf << "; ret, " << iRet << endl;
	
    delete pTab;

	cout << "Proc call test  end . " << endl;
}

