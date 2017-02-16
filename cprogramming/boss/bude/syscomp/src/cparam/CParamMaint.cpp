#include <fstream>
#include <iomanip>

#include "udbi/udbi.h"

#include "budeipc.h"
#include "IIpcParam.h"
#include "IpcParam.h"
#include "IpcLock.h"

using std::ofstream;

using namespace UDBI;
using namespace budeipc;

///
void printmenu();
void printmenu_x();
int dbconnect(const string& _sin);
void par_analysis();

/**
 * 参数组件后台维护工具程序
 */
int main(int argc, char* argv[])
{
	int dbconnected = 0;
	
	if (argc == 2) 
	{
		if (dbconnect(argv[1]) < 0) exit(-1);
		dbconnected = 1;
	}
	
	fprintf(stdout, "\n\t--------------------------------------------\n");
	fprintf(stdout, "\t|       欢迎使用共享内存参数维护程序       |\n");
	fprintf(stdout, "\t|       Copyright (C) 2001 by Linkage      |\n");
	fprintf(stdout, "\t--------------------------------------------\n\n");
	fprintf(stdout, "\t程序执行方式: %s dbuser/dbpass@dbalias 或 %s \n", argv[0], argv[0]);
	
	printmenu();
		
	fprintf(stdout, "\n\t请输入操作选项: ");
		
	string sop, soper;
	int ioper, idx =0, pcnt =0;
	
	cin >> sop;
	for (int x=0; x<sop.size(); ++x)
	{
		if ( !isdigit(sop[x]) ) 
		{
			fprintf(stdout, "\t非法操作选项!\n");
			exit(-1);
		}
	}
	
	ioper = atoi(sop.c_str());

	while (ioper != 0) 
	{
		switch (ioper) 
		{
			case 11:
				try
				{
					IIpcParam::initialize();
					fprintf(stdout, "\t\tIPC资源创建成功!");
				} catch (Exception& err) {
					cout << err;
				}

				break;
				
			case 12:
				cout << "\t确认删除参数组件的共享内存么(Y or N)? ";
				cin >> soper;
				if ( !(soper == "Y") ) {
					cout << "\n\t放弃操作!" << endl;
					break;
				}
				
				try
				{
					ofstream _out2("cpar_status_end.log");
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					_ipp.status(&_out2);
					
					IIpcParam::uninitialize();
					fprintf(stdout, "\t\tIPC资源已删除!");
				} catch (Exception& err) {
					cout << err;
				}

				break;
				
			case 13:
				try
				{
					if (dbconnected == 0) {
						cout << "\t请输入数据库连接串: ";
						cin >> soper;
						
						if (dbconnect(soper) < 0) 
							break;
					}
					
					dbconnected = 1;
					
					cout << "\t请输入参数名: ";
					cin >> soper;
					
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					idx = _ipp.load(soper);
					if (idx < 0) {
						fprintf(stdout, "\t\t参数加载失败!\n");
					} else {
						fprintf(stdout, "\t\t参数加载成功!\n");
					}
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 14:
				try
				{
					if (dbconnected == 0) {
						cout << "\t请输入数据库连接串: ";
						cin >> soper;
						
						if (dbconnect(soper) < 0) break;
					}
					
					dbconnected = 1;

					cout << "\t请输入参数名: ";
					cin >> soper;
					
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					pcnt = _ipp.refresh(soper);
					if (pcnt < 0) {
						fprintf(stdout, "\t\t参数刷新失败!\n");
					} else {
						fprintf(stdout, "\t\t参数刷新成功(%d)!\n", pcnt);
					}
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 15:
				try
				{
					cout << "\t请输入参数名: ";
					cin >> soper;

					char sfile[200];
					sprintf(sfile, "cpar_dump_%s.log", soper.c_str());
					ofstream _out1(sfile);
					
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					pcnt = _ipp.dump(soper, &_out1);
					if (pcnt < 0) {
						fprintf(stdout, "\t\t指定参数数据dump失败!\n");
					} else {						
						fprintf(stdout, "\t\t指定参数数据已dump到文件(%s)!\n", sfile);
					}
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;		
						
			case 21:
				try
				{
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					_ipp.status();
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 22:
				try
				{
					ofstream _out2("cpar_status.log");
					
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					if (_ipp.status(&_out2) < 0) {
						fprintf(stdout, "\t\t共享内存状态信息查询失败!\n");
					} else {					
						fprintf(stdout, "\t\t共享内存状态信息已dump到文件(cpar_status.log)!\n");
					}
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 23:
				try
				{
					IIpcParam& _ipp = IIpcParam::getIpcParam();
					_ipp.headinfo(1);
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 24:
				try
				{
					cout << "\t请输入参数名: ";
					cin >> soper;

					IIpcParam& _ipp = IIpcParam::getIpcParam();
					_ipp.keyinfo(soper, 1);
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 31:
				try
				{
					cout << "\t请输入转换条件(KEY:val1,val2,val3): ";
					cin >> soper;

					string skey, sval1, sval2, sval3, srslt;
					int k = soper.find(':');
					int v1 = soper.find(',');
					int v2 = soper.rfind(',');
					
					if (k == string::npos) 
					{
						//
						fprintf(stdout, "\t\t输入转换条件非法!\n");
						break;
					}					
					skey = soper.substr(0, k);
					
					IIpcParam& _ipp = IIpcParam::getIpcParam();
						
					if ((v1==string::npos) && (v2 == string::npos))
					{
						//
						sval1 = soper.substr(k+1);
						srslt = _ipp.getTrans(skey, sval1);
					} else if (v1 == v2)
					{
						//
						sval1 = soper.substr(k+1, v1-k-1);
						sval2 = soper.substr(v1+1);
						//cout << skey << ":" << sval1 << ":" << sval2 << endl;
						srslt = _ipp.getTrans(skey, sval1, sval2);
					} else
					{
						//
						sval1 = soper.substr(k+1, v1-k-1);
						sval2 = soper.substr(v1+1, v2-v1-1);
						sval3 = soper.substr(v2+1);						
						srslt = _ipp.getTrans(skey, sval1, sval2, sval3);
					}

					fprintf(stdout, "\t\t转换结果:%s!\n", srslt.c_str());
					
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 41:
				if (dbconnected == 0) {
					cout << "\t请输入数据库连接串: ";
					cin >> soper;
					
					if (dbconnect(soper) < 0) break;
				}

				par_analysis();
				break;
				
			case 51:
				try
				{
					IpcLock& __lck = IpcLock::getLock(CPAR_IPCKEY_);
					__lck.status();
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 52:
				try
				{
					int _lckid = -1;
					cout << "\t请输入锁ID: ";
					cin >> _lckid;
					
					IpcLock& __lck = IpcLock::getLock(CPAR_IPCKEY_);
					__lck.status(_lckid);
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;
				
			case 53:
				try
				{
					int _lckid = -1;
					cout << "\t请输入锁ID: ";
					cin >> _lckid;
					
					IpcLock& __lck = IpcLock::getLock(CPAR_IPCKEY_);
					__lck.kill(_lckid);
					cout << "\t解锁成功!" << endl;
				} catch (Exception& err) {
					cout << err;
				} 
				
				break;

			case 9:
				printmenu();
				break;		

			case 999:
				printmenu_x();
				break;		
										
			default:
				fprintf(stdout, "\t\t无效操作选项!\n");
				break;
				
		} // switch
		
		fprintf(stdout, "\n\t请输入操作选型(9.重新显示菜单, 0.退出): ");
		cin >> sop;
		for (int x=0; x<sop.size(); ++x)
		{
			if ( !isdigit(sop[x]) ) 
			{
				fprintf(stdout, "\t非法操作选项!\n");
				exit(-1);
			}
		}
		
		ioper = atoi(sop.c_str());
	} // while
			
	return 0;
}



/**
 *
 */
void printmenu()
{
	fprintf(stdout, "\t11. 创建并初始化IPC资源\n");
	fprintf(stdout, "\t12. 删除IPC资源\n");
	fprintf(stdout, "\t13. 加载参数数据到内存[需要连接数据库]\n");
	fprintf(stdout, "\t14. 刷新内存参数数据[需要连接数据库]\n");
	fprintf(stdout, "\t15. 倒出参数数据到文件\n");
	fprintf(stdout, "\t21. 查询内存状态\n");
	fprintf(stdout, "\t22. 输出内存状态到日志文件\n");
	fprintf(stdout, "\t23. 查询头信息\n");
	fprintf(stdout, "\t24. 查询参数索引信息\n");
	fprintf(stdout, "\t31. 测试参数转换\n");
	fprintf(stdout, "\t0. 退出程序\n");
}

/**
 *
 */
void printmenu_x()
{
	fprintf(stdout, "\t11. 创建并初始化IPC资源\n");
	fprintf(stdout, "\t12. 删除IPC资源\n");
	fprintf(stdout, "\t13. 加载参数数据到内存[需要连接数据库]\n");
	fprintf(stdout, "\t14. 刷新内存参数数据[需要连接数据库]\n");
	fprintf(stdout, "\t15. 倒出参数数据到文件\n");
	fprintf(stdout, "\t21. 查询内存状态\n");
	fprintf(stdout, "\t22. 输出内存状态到日志文件\n");
	fprintf(stdout, "\t23. 查询头信息\n");
	fprintf(stdout, "\t24. 查询参数索引信息\n");
	fprintf(stdout, "\t31. 测试参数转换\n");
	fprintf(stdout, "\t41. 参数数据分析[需要连接数据库]\n");
	fprintf(stdout, "\t51. 查询锁信息(所有)\n");
	fprintf(stdout, "\t52. 查询锁信息(指定)\n");	
	fprintf(stdout, "\t53. 强制解锁(指定)\n");	
	fprintf(stdout, "\t0. 退出程序\n");
}

/**
 *
 */
int dbconnect(const string& _sin)
{
	string sopt(_sin);
	int i = sopt.find('/');
	int j = sopt.find('@');
	if ((i== string::npos) || (j== string::npos))
	{
		fprintf(stdout, "\t\t数据库连接串(user/password@alias)不合法!");
		return -1;
	}
		
	try
	{
		jdb::jinit(sopt.substr(j+1), sopt.substr(0,i), sopt.substr(i+1,j-i-1));
	} catch (EUdbiException& err) {
		cout << err;
		return -1;
	}
	
	//
	return 1;
}	
	
/**
 *
 */
void par_analysis()
{
	cout << "开始参数数据分析,可能需要几分钟时间,请稍候..." << flush;
	
	JConnect *pcon = jdb::getConnect();
	
	JTable* pTab = pcon->CreateJTable();
		
	pTab->AddSql("select sql_ref, sql_stmt from code_code where dao_name = 'CPARAM' order by sql_ref");
	pTab->ExecSelect();

	int reccnt=0, vcnt=0, vvsize=0, vrsize=0;
	int vv1size=0, vv2size=0, vv3size=0;
	JTable* pTab2 = pcon->CreateJTable();
	
	string skey, ssql;
	ofstream _out2("cpar_analysis.log");
	ofstream _out3("cpar_analysis_except.log");
	
	_out2 << setw(31) << "KEY(类型):";
	_out2 << "\t" << setw(10) << " 记录数  ";
	_out2 << "\t" << setw(10)  << " 值长度  " ;
	_out2 << "\t" << setw(10)  << " 值长度  " ;
	_out2 << "\t" << setw(10)  << " 值长度  " ;
	_out2 << "\t" << setw(12)  << " 结果长度  " << endl;

	_out2 << setw(31) << "-------------------------------";
	_out2 << "\t" << setw(10) << "----------";
	_out2 << "\t" << setw(10)  << "----------" ;
	_out2 << "\t" << setw(10)  << "----------" ;
	_out2 << "\t" << setw(10)  << "----------" ;
	_out2 << "\t" << setw(12)  << "------------" << endl;
		
	while (pTab->Fetch()) 
	{
		skey = pTab->GetString("SQL_REF");
		ssql = pTab->GetString("SQL_STMT");
		
		try
		{
		pTab2->Clear();
		pTab2->AddSql(ssql);
		pTab2->SetParam(":VKEY", skey);
		pTab2->ExecSelect();
		
		reccnt = 0;
		int iv=0, iv1=0, iv2=0, iv3=0, ir=0, ir0=0, ix=0;
		int iiv1=0, iiv2=0, iiv3=0;
		
		if ( (ssql.find("VALUE3") != string::npos) || (ssql.find("value3") != string::npos) )
		{
			ix = 3;
			
			while (pTab2->Fetch())
			{
				iv1 = (pTab2->GetString("VALUE1")).length();				
				iv2 = (pTab2->GetString("VALUE2")).length();				
				iv3 = (pTab2->GetString("VALUE3")).length();				
				ir0 = (pTab2->GetString("VRESULT")).length();		
				
				reccnt++;
	
				iiv1 = iiv1>iv1 ? iiv1 : iv1;
				iiv2 = iiv2>iv2 ? iiv2 : iv2;
				iiv3 = iiv3>iv3 ? iiv3 : iv3;
				ir = ir>ir0 ? ir : ir0;
			}		
		}
		else
		{
			ix = 2;
			
			while (pTab2->Fetch())
			{
				iv1 = (pTab2->GetString("VALUE1")).length();				
				iv2 = (pTab2->GetString("VALUE2")).length();				
				ir0 = (pTab2->GetString("VRESULT")).length();		
				
				reccnt++;
	
				iiv1 = iiv1>iv1 ? iiv1 : iv1;
				iiv2 = iiv2>iv2 ? iiv2 : iv2;
				ir = ir>ir0 ? ir : ir0;
			}		
			iiv3 = 0;
		}
		
		///
		_out2 << setw(26) << skey << "( " << ix << "):";
		_out2 << "\t" << setw(10) << reccnt;
		_out2 << "\t" << setw(10)  << iiv1 ;
		_out2 << "\t" << setw(10)  << iiv2 ;
		_out2 << "\t" << setw(10)  << iiv3 ;
		_out2 << "\t" << setw(12)  << ir << endl;
		
		vv1size = vv1size>iiv1 ? vv1size : iiv1;
		vv2size = vv2size>iiv2 ? vv2size : iiv2;
		vv3size = vv3size>iiv3 ? vv3size : iiv3;
		vrsize = vrsize>ir ? vrsize : ir;
		vcnt += reccnt;
		} catch (EUdbiException& er) {
			_out3 << skey << ":" << er << endl;	
		}
	}
	
	///
	vvsize = vv1size;
	vvsize = vvsize>vv2size ? vvsize : vv2size;
	vvsize = vvsize>vv3size ? vvsize : vv3size;

	_out2 << "总记录数：" << vcnt ;
	_out2 << ", 值字段最大长度：" << vvsize ;
	_out2 << ", 结果字段最大长度：" << vrsize << endl;
	
	///
	cout << " 分析结束, 请检查分析日志文件. " << endl;
}
