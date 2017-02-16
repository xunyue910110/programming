#ifndef PO_COMMON_H
#define PO_COMMON_H

#include "base/config-all.h"

#include <string>
#include <vector>
#include <iostream>

USING_NAMESPACE(std)


typedef vector<string> StringVector;
typedef StringVector::iterator StringVectorIterator;

#define USERNAME_1		"param"
#define PASSWORD_1		"param123"
#define SERVNAME_1		"eboss"

#define USERNAME_2		"account"    
#define PASSWORD_2		"accountdsj" 
#define SERVNAME_2		"eboss"       

#define USERNAME_3		"param"    
#define PASSWORD_3		"param123" 
#define SERVNAME_3		"eboss"       

#define FILEPATH_1		"$WORK_HOME/data/dbparam"
#define SQLUPDATE_1		"select time_stamp from td_update_flag where rownum<=1"


#define FROM_FILE		1
#define FROM_DATABASE	2

#define BY_FLAG			1
#define BY_TIME			2

typedef struct {
	int dataSource;
	int updateCondition;
	string filePath;
	string fileName;
	string userName;
	string passWord;
	string servName;
	string sqlStatement;
	int fieldNum;
	string countSql;
    //added by shandy 20060712
    time_t fileModifyTime;

}PO_Property;
    
#endif // PO_COMMON_H

