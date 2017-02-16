#ifndef FIELDINFOS_H_HEADER_INCLUDED_B3E0D7F7
#define FILEDINFOS_H_HEADER_INCLUDED_B3E0D7F7

#include <string>
#include <map>
#include <vector>

#include "base/config-all.h"
#include "FieldInfo.h"
#include "PpConstDef.h"

USING_NAMESPACE(std)

class FieldInfos;
class RepeatableFieldInfo;

typedef map<int,FieldInfos *> FIELDINFOS_MAP;
typedef FIELDINFOS_MAP::iterator FIELDINFOS_MAP_ITR;
typedef map<MyString,int>  FIELD_NAME_MAP;
typedef FIELD_NAME_MAP::iterator  FIELD_NAME_MAP_ITR; 
	

class FieldInfos
{
  public:
  	FieldInfo* getFieldInfoByName(const char *pFieldName);
  	FieldInfo * getRepeatableFieldInfoByName(const char *pFieldName);
  	int getSequnceByName(const char *pFieldName) ;
  	
	bool reset();
	
	FieldInfo *operator[](const int iSequnce);
	
	static FieldInfos * getInstance(const int iChannelNum);
	
	int getFieldNum();	            // 字段名不可重复字段总数
	int getRepeatableFieldNum();    // 字段名可重复字段总数
	
	const char *getFieldNameBySeq(const int iSequnce);
  private:
	FieldInfos();
	~FieldInfos();

  private:
	static FIELDINFOS_MAP m_fieldInfosMap; // 一个预处理进程,可能处理多个输入目录的原始文件
	int   m_iFieldCount;                   // 一个预处理配置文件里,可能出现的非重复字段总数
	int   m_iRepeatableFieldCount;         // 一个预处理配置文件里,可能出现的可重复字段总数
	FieldInfo *m_pFieldInfos[DIVIDE];           // 根据预处理配置文件中所有涉及的字段个数,构造FieldInfo数组,此数组在预处理运行的生命周期内有效
	FIELD_NAME_MAP m_fieldNameMap;
	FieldInfo *m_pRepeatableFieldInfos[DIVIDE];
};


#endif /* FIELDINFOS_H_HEADER_INCLUDED_B3E0D7F7 */
