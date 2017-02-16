#include "MdbParamManager.h"
#include "Mdb_TestCm.h"

int main()
{
  MdbParamManager mdbManager;
  MdbInfo db;
  db.m_dbType = MDB;
  db.m_dbServer = "testmdb";
  db.m_hostName = "";
  db.m_port = 0;
  if (!mdbManager.initialization(db))
  {
  	cout << "初始化内存数据库失败!" << endl;
  	return -1;
  }
  
  Mdb_TestCm po,rpo;
  vector<Mdb_TestCm> rpoVec;
  po.recordStruct.m_userId = 0;
  po.recordStruct.m_billId = 10;
  cout << "=============== search() example ===================" << endl;
  if (mdbManager.search<Mdb_TestCm>(po,rpo))
  {
    cout << " po : " << po << endl;
    cout << " rpo : " << rpo << endl;
  }
  else
  {
  	cout << "查找失败！" << endl;
  }
  
  cout << "=============== searchMulti() example ===============" << endl;
  po.recordStruct.m_userId = 1;
  po.recordStruct.m_billId = 20;
  cout << " po : " << po << endl;
  if (mdbManager.searchMulti<Mdb_TestCm>(po,rpoVec))
  {
    for (int i = 0; i < rpoVec.size(); i++)
    {
    	cout << " rpoVec[" << i << "]: " << rpoVec[i] << endl;
    }
  }
  else
  {
  	cout << "查找失败！" << endl;
  }
  return 0;
}
