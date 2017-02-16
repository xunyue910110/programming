// ##########################################
// Source file : PpConstDef.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef PPCONSTDEF_H_HEADER_INCLUDED_B7A657C5
#define PPCONSTDEF_H_HEADER_INCLUDED_B7A657C5

#include "config-all.h"
#include "base/StringUtil.h"
#include <string>
#include <vector>

USING_NAMESPACE(std);

const int DIVIDE = 1000;   // FieldInfos对象中,FieldInfo的最大个数
const int OUTOF_FIELD_RANGE=-1; // FieldInfos中未定义的字段

const char CDR_TYPE[] = "cdr_type";
const char OUTPUT_FILE_TYPE[] = "output_file_type";
const char RAWFILE_TYPE[]     = "rawFile_type";
const char EMPTY_FILE_ALERT[] = "empty_file_alert";
const char FILE_HEANDER[]     = "file_header";
const char FILE_HEADER_SIZE[] = "file_header_size";
const char FILE_TAIL_SIZE[]   = "file_tail_size";
const char FILE_HEAD_RECEIPT[]= "file_head_receipt";
const char FILE_TAIL_RECEIPT[]= "file_tail_receipt";
const char CHECK_CDR_SUM[]    = "check_cdr_sum";
const char FIX[]              = "FIX";
const char TOKEN[]            = "TOKEN";
const char ASN[]              = "ASN";
const char SASN[]             = "SASN";
const char GASN[]             = "GASN";
const char MODULE[]           = "Module";
const char SUB_MODULE[]       = "Sub_Module";
const char SPLIT_CODE_1[]     = " ";
const char SPLIT_CODE_2[]     = "\t";
const char SPLIT_COMMA[]      = ",";
const char EMPTY[]            = "EMPTY";
const char OTHER[]            = "OTHER"; 
const char DEAL_OBJ_NAME[]    = "FIELD";
const char STR_OUTOF_FIELD_RANGE[] = "OUTOF_FIELD_RANGE";
const char CONDITION_ALL[]     = "ALL";
const char TAG_ALL[]      = "TAG_ALL";

// 特殊字段名
const char FILE_NAME[]        = "FILENAME";

const char H_FILE_HEAD[]      = "h_file_head";
const char H_TOTAL_CDRS[]     = "h_total_cdrs";
const char T_FILE_TAIL[]      = "t_file_tail";
const char T_TOTAL_CDRS[]     = "t_total_cdrs";
const char T_TOTAL_FEE[]      = "t_total_fee";

const char CALL_TYPE[]        = "CALL_TYPE";
const char START_DATE[]       = "START_DATE";
const char START_TIME[]       = "START_TIME";
const char END_DATE[]         = "END_DATE";
const char END_TIME[]         = "END_TIME";
const char START_YEAR[]       = "START_YEAR";       
const char START_MONTH[]      = "START_MONTH";      
const char START_DAY[]        = "START_DAY";        
const char START_HOUR[]       = "START_HOUR";       
const char START_MINUTE[]     = "START_MINUTE";     
const char START_SECOND[]     = "START_SECOND";     
const char START_NTIME[]      = "START_NTIME";      
const char END_YEAR[]         = "END_YEAR";         
const char END_MONTH[]        = "END_MONTH";        
const char END_DAY[]          = "END_DAY";          
const char END_HOUR[]         = "END_HOUR";         
const char END_MINUTE[]       = "END_MINUTE";       
const char END_SECOND[]       = "END_SECOND";       
const char DURATION[]         = "DURATION";         
const char DURATION_CARRY[]   = "DURATION_CARRY";   
const char DURATION_ROUND[]   = "DURATION_ROUND";   
const char DURATION_NOCARRY[] = "DURATION_NOCARRY"; 
const char DURATION_3NOCARRY[]= "DURATION_3NOCARRY";
const char MODULE_NAME[]      = "Module_name";
const char MSISDN[]           = "msisdn";
const char ERROR_CODE[]       = "ERROR_CODE";
const char CALL_TOTAL_FEE[]   = "CALL_TOTAL_FEE";
const char RECORD_TYPE[]      = "record_type";

class MyString
{
	public:
		MyString(){}
		MyString(const char * pStr)
		{
			strcpy(m_pStr,pStr);	
		}
		friend bool operator < (const MyString &left,const MyString &right)
		{
			return (strcmp(left.m_pStr,right.m_pStr)<0);	
		}
		const char *c_str() const
		{
			return m_pStr;	
		}
	private:
		char m_pStr[32];
};

#endif /* PPCONSTDEF_H_HEADER_INCLUDED_B7A657C5 */
