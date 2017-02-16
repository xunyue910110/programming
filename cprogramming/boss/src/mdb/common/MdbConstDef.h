/**
*   MDB常量定义.
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-04-12
*/
#ifndef _MDB_CONSTDEF_H_INCLUDE_20080411_
#define _MDB_CONSTDEF_H_INCLUDE_20080411_ 

#include "config-all.h"
#include "base/supdebug.h"
#include <string>
#include <vector>
USING_NAMESPACE(std)
#include <iostream>

typedef char  T_NAMEDEF[64+1];      ///<名称类型：表空间名、表名、索引名、字段名
typedef char  T_IPADDR[32+1];       ///<IP地址信息
typedef char  T_FILENAMEDEF[256+1]; ///<文件全名类型
typedef char  T_GPARAMVALUE[64+1] ; ///<全局参数值类型

const int   MAX_SESSION_NUM = 10000;///<Session最大个数 10000
const int   MAX_COLUMN_NUM  = 200;  ///<一个表允许的最大字段数
const int   MAX_SPACE_NUM   = 20;   ///<数据库允许的最大表空间数
const int   MAX_INDEX_NUM   = 5  ;  ///<一张表支持的最大索引个数
const int   MAX_IDXCLMN_NUM = 10 ;  ///<一个索引允许包含的最大字段数
const int   SHM_MODEFLAG    = 0666; ///<共享内存权限属性    
const string MDB_HOMEPATH("MDB_HOME");

enum T_DBLK_TYPE ///<定义信号灯类型
{
  T_DBLK_LOCK   = 0,  ///<LOCK文件信号灯
  T_DBLK_DB     = 1,  ///<数据库信号灯
  T_DBLK_SPACE  = 2,  ///<表空间信号灯
  T_DBLK_TABLE  = 3   ///<表信号灯
};   
enum T_SPACETYPE ///<表空间类型
{
  CONTROL_SPACE = 0,    ///< 控制区表空间
  TABLE_SPACE   = 1,    ///< 数据区表空间
  INDEX_SPACE   = 2,    ///< 索引区表空间
  UNKNOW_SPACE          ///< 未知类型表空间
} ;

enum T_TABLETYPE ///<表属性
{
  SYSTEM_TABLE = 0, ///<系统表
  DATA_TABLE   = 1, ///<数据表
  INDEX_TABLE  = 2, ///<索引表
  LOCK_TABLE   = 3, ///<锁表
  UNKNOW_TABLE      ///<未知类型表
};
enum T_INDEXTYPE
{
  HASH_INDEX  = 0, ///<Hash索引
  TREE_INDEX  = 1, ///<树索引
  HASH_TREE   = 2, ///<Hash树
  UNKNOW_INDEX     ///<未知类型索引
};

enum T_MEMUSEDFLAG
{
  MEMUSE_IDLE=0,    ///<未使用
  MEMUSE_USED=1,    ///<在用
  MEMUSE_USEDIDLE   ///<已使用过现未用
};

//DB支持的字段数据类型
enum COLUMN_VAR_TYPE 
{ 
    VAR_TYPE_INT2=0,  //2位整数:  2个字节
    VAR_TYPE_INT,   //整数： sizeof(int)
    VAR_TYPE_DATE,//2个整型数：2*sizeof(int)
    VAR_TYPE_LONG,  //长整数:sizeof(long)
    VAR_TYPE_REAL,  //浮点数:sizeof(float)
    VAR_TYPE_NUMSTR,//数字字符串：采用BCD码压缩
    VAR_TYPE_VSTR,  //字符串:长度可定义
    VAR_TYPE_UNKNOW //未知
};

// 支持的数据库类型
enum T_DATABASETYPE
{
  MDB       =0,    //自主研发本地
  ODBCMDB   =1,    // ODBC标准接口
  RMDB      =2     // 自主远程
};

// 支持的信号控制类型
enum T_SEMAPHORETYPE
{
  	SYS_SEMAPORE       =0,    //系统信号灯
	POSXI_SEMAPORE       =1
};

// 支持的session类型
enum T_SESSIONTYPE
{
	LOCAL_SESSION    = 0, // 自主本地连接
	REMOTE_SESSION   = 1, // 自主远程连接
	ODBC_SESSION     = 2
};

const char COLUMN_VAR_DEFLIST[][10+1] = {
  "INT2",
  "INT",
  "DATE", 
  "LONG",
  "REAL",
  "NUMSTR",
  "VSTR"
};


//记录占用最大内存
const int MAX_VALUE_LEN = 500;

const int MAX_COLUMN_LEN = 100; // 一个表字段的最大长度
const int CHAR_INT_MAX   = 256; // 字符对应的最大数字
const int OFFSET_LEN=5;
class ShmPosition
{
	public:
	  	char    m_spaceCode;
	  	char    m_offSet[OFFSET_LEN];
	public:
		ShmPosition()
		{
		  memset(m_offSet,0,sizeof(ShmPosition));
		}
		ShmPosition(const unsigned int &r_spaceCode,const size_t &r_offSet)
		{
		  setSpaceCode(r_spaceCode);
		  setOffSet(r_offSet);
		}
		~ShmPosition(){}
	public:
		unsigned int getSpaceCode() const
		{
		  unsigned int t_spCode;  
		  t_spCode=m_spaceCode;
		  return t_spCode;
		}
		size_t getOffSet() const
		{
		  size_t t_offSet=0;
		  for(int i=0;i<OFFSET_LEN;i++)
		  {
		    t_offSet=t_offSet*CHAR_INT_MAX+(unsigned char)(m_offSet[i]);
		  }
		  return t_offSet;
		}
		bool setSpaceCode(unsigned int r_spaceCode)
		{
		  if(r_spaceCode>CHAR_INT_MAX) return false;
		  m_spaceCode = r_spaceCode;
		  return true;
		}
		bool setOffSet(size_t r_offSet)
		{
		  size_t t_offSet=r_offSet;
		  for(int i=OFFSET_LEN-1;i>=0;i--)
		  {
		    m_offSet[i]= t_offSet%CHAR_INT_MAX;
		    t_offSet   = t_offSet/CHAR_INT_MAX;
		  }
		  if(t_offSet>0 ||r_offSet<0 )
		  {
		    return false;
		  }
		  return true;
		}
		bool setValue(unsigned int r_spaceCode,size_t r_offSet)
		{
		  return (setSpaceCode(r_spaceCode)&&setOffSet(r_offSet));
		}
	public:
		ShmPosition& operator=(const ShmPosition& right)
		{
		  memcpy(this,&right,sizeof(ShmPosition));
		  return *this;
		}
		friend int operator==(const ShmPosition& left,const ShmPosition& right)
		{
		  return (memcmp(&left,&right,sizeof(ShmPosition))==0);
		}
		friend int operator!=(const ShmPosition& left,const ShmPosition& right)
		{
		  return (memcmp(&left,&right,sizeof(ShmPosition))!=0);
		}
		friend ostream& operator<<(ostream& os,const ShmPosition& r_obj)
		{
		  os<<"{"<<r_obj.getSpaceCode()<<","<<r_obj.getOffSet()<<"}";
		  return os;
		}

};
const ShmPosition NULL_SHMPOS(0,0);

// 以下为mdb通讯协议定义常量
const char SELECT_CMD[] = "select";
const char INSERT_CMD[] = "insert";
const char UPDATE_CMD[] = "update";
const char DELETE_CMD[] = "deleteRec";
const char QUIT_CMD[] = "quit";
const char BEGIN_CMD[] = "begin";
const char COMMIT_CMD[] = "commit";
const char ROLLBACK_CMD[] = "rollback";
const char GETTABLEDESC_CMD[] = "getTableDesc";
const char GETINDEXDESC_CMD[] = "getIndexDesc";
const char GETALLTABLENAME_CMD[] = "getALlTableName";
const char GETALLINDEXNAME_CMD[] = "getALlIndexName";
const char GETTABLEROWCOUNTNAME_CMD[] = "getTableRowCount";
const char GET_ROWBYROW_NEXTREC[]     = "getrowbyrownextrec";

const char SUCCESS[]="success"; 


// 命令字段间分割定义 select:tableName;indexName;expr;13512519742,20080605,;
const char CMD_TOKEN[] = ":";              
const char FIELD_TOKEN[] = ";";
const char VALUE_TOKEN[] = ",";
//Added 2008-7-8
const char GROUP_TOKEN[] = "|";

const int TRANS_FIELDS_LENTH_BYTES = 2;

// add by chenm 2009-5-18 
const int SELECT_ROW_BY_ROW = -2;
const int SELECT_ROW_BY_ROW_INTERNAL = -3;
// over 2009-5-18 

// 每次socket通讯,所能传递的最大字节数
const int MAX_TRANSMISSION_BYTE_SIZE = 2048;
// client和server端公用,定义用于一次交互通讯的最大缓存大小
const int MAX_TRANSMISSION_BUFFER    = MAX_TRANSMISSION_BYTE_SIZE*64;
const size_t MAX_POS_OFFSET = 90000000000;

// add by chenm for 不等值查询
const int RANGE = 1; // ()
const int RANGE_L = 2; // [)
const int RANGE_R = 3; // (]
const int RANGE_LR = 4; // []


#endif //_MDB_CONSTDEF_H_INCLUDE_20080411_
