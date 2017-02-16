#include <iostream>
#include <unistd.h>
#include <map>

#include "CParamDAO.h"

using namespace std;
using namespace UDBI;

JConnect *pcon;

/**
 * ◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇
 * 测试参数生成
 */
void insParam()
{
	string s1, s2, s3;
	char skey[200], svalue[200], sdesc[200];
	
	JTable *pTab = pcon->CreateJTable();

	for (int i=0; i<500; i++)
	{
		memset(skey, 0, sizeof(skey));
		sprintf(skey, "TAB%d.STATE", i);
		
		pcon->TransBegin();
		try 
		{
			for (int j=0; j<10; j++) {
				memset(svalue, 0, sizeof(svalue));
				memset(sdesc, 0, sizeof(sdesc));
				sprintf(svalue, "%d0A", j);
				sprintf(sdesc, "TAB表状态%d", j);
			
				pTab->Clear();
				pTab->AddSql("insert into TD_S_CPARAM values(:vkey, :vvalue, NULL, :vdesc)");
				pTab->SetParam(":vkey", skey, strlen(skey)+1);
				pTab->SetParam(":vvalue", svalue, strlen(svalue)+1);
				pTab->SetParam(":vdesc", sdesc, strlen(sdesc)+1);
				pTab->ExecUpdate();
					
			}
			
			pcon->TransCommit();
		} catch (Exception &er)
		{
			cout << er << endl;
		}		
	}
	
	delete pTab;
}

/**
 *  ◆参数匹配函数，从静态对象中按key值匹配参数值
 *
 */
int main(int argc, char argv[])
{
	try
	{
		jdb::jinit("eboss", "code_user", "code_user");
		pcon = jdb::getConnect();
	
		//insParam();
		//return 0;
		
		CParamDAO::Load();
	
		string sKey;
		string sValue;
		string sDesc;
		while (sKey != "OK") {
			cout << "Please input key and value: " << endl;
			cin >> sKey;
			cin >> sValue;
		
			sDesc = CParamDAO::getParam(sKey, sValue);
			cout << "Desc :" << sDesc << endl;
		}
		
		CParamDAO::UnLoad();
	} catch (Exception &er)
	{
		cout << er << endl;
	}
	
	return 0;
}
