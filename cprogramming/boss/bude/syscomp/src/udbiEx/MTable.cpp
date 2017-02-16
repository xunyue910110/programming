#include "jdb.h"
#include "JConnect.h"
#include "MTable.h"

UDBI::MTable::MTable(const string& tabName)
{
	JConnect* lpcon = jdb::getConnect();
	if (!lpcon) THROW_C_P1(EUdbiException, 90000, "%s:MTable实例失败，未建立数据库连接！", tabName.c_str());
	
	m_ptab = lpcon->CreateJTable(tabName);
}
