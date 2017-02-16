#ifndef MDB_PROPERTY_H
#define MDB_PROPERTY_H

#include <iostream>
#include <utility>
#include <string>

using namespace std;

typedef struct
{
	//表名
  string tableName;
  //索引名
  string indexName;

  //索引字段数
  int    indexParams;

  //其它条件语句
  string whereCaluse;
  //其它条件参数个数
  int    otherParams;
  
}Mdb_Property;

#endif // MDB_PROPERTY_H
