#ifndef _MemInterface_H_
#define _MemInterface_H_
/*
  2010.05.11 提示内存库报错时，建议提示到哪个数据库，减少维护的工作量及准确定位问题
  
*/

#include "MmCommon.h"


class DataBase;
class Session;
class TableDef;


/**
* MmInterface:内存数据库操作类,完成基于内存数据库的各种操作。
* 	MmInterface:			内存数据库操作类构造函数
*	~MmInterface:			内存数据库操作类析构函数
*	connect:				建立与内存数据库的连接
*	disconnect:				断开与内存数据库的连接
*	beginTrans:				开启事务
*	commitTrans:			提交事务
*	rollbackTrans:			回滚事务
*	select:					执行查询
*	select:					执行查询
*	deleteRec:				执行删除
*	deleteRec:				执行删除
*	insert:					执行插入
*	insert:					执行插入
*	update:					执行更新
*	update:					执行更新
*	getNext:				获取一行记录
*	getColumnnum:			获取本次查询的记录的列数
*	getColumnShortValue:	获取记录列的短整型值
*	getColumnIntValue:		获取记录列的整型值
*	getColumnLongValue:		获取记录列的长整型值
*	getColumnFloatValue:	获取记录列的浮点值
*	getColumnDateValue:		获取记录列的日期值
*	getColumnStringValue:	获取记录列的字符值
*	truncate
*/
class MmInterface
{
	public:
		
		/**
		* MmInterface:内存数据库操作类构造函数
		* @param	无
		* @return
		*/	
		MmInterface();

		/**
		* ~MmInterface:内存数据库操作类析构函数
		* @param	无
		* @return
		*/			
		~MmInterface();

		/**
		* connect:建立与内存数据库的连接(抛异常)
		* @param	dbtype		数据库类型   0:自主研发本地 1:oracle timesten 2:altibase 3:IBM SOLID 4:自主远程
		* @param	dbname		数据库名称
		* @param	hostname	数据库主机名称
		* @param	port		  数据库端口
		* @param	username	数据库用户名
		* @param	password	数据库用户密码		
		* @return
		*/
		//modify by chenxt 2008-12-09			
		//void connect(const T_DATABASETYPE &dbtype, const string &dbname, const string &hostname, const int &port);
    void connect(const T_DATABASETYPE &dbtype, const string &dbname, const string &hostname, const int &port, 
                 const string &username, const string &password);
    	
		/**
		* disconnect:断开与内存数据库的连接
		* @param	无
		* @return	无
		*/		
		void disconnect();

		/**
		* beginTrans:开启事务(抛异常)
		* @param	tablename	事务基于的表
		* @param	locktype	事务表锁类型 0:表锁 1:行级锁
		* @return	无
		*/
		void beginTrans(const string &tablename, const int &locktype = 1);

		/**
		* commitTrans:提交事务(抛异常)
		* @param	无
		* @return	无
		*/		
		void commitTrans();

		/**
		* rollbackTrans:回滚事务(抛异常)
		* @param	无
		* @return	无
		*/		
		void rollbackTrans();

		/**
		* select:执行查询(抛异常)
		* @param	tablename	表名称
		* @param	indexname	查询基于的表索引名称
		* @param	expr		查询基于的表索引外的其他条件组成的表达式
		*						一个参数msisdn=:0 
		*						两个参数(msisdn=:0) and (startTime <= :1)
		*						三个参数((msisdn=:0) and (startTime <= :1)) and (endTime <= :2) 
		* @param	params		查询参数集合,如果是和索引相关,MmParam参数里面的m_indextag=1;否则和expr相关的,m_indextag=0
		* @return	返回查询到的记录条数
		*/
		int select(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params);

		/**
		* select:执行查询(抛异常)
		* @param	tablename	表名称
		* @param	indexname	查询基于的表索引名称
		* @param	expr		查询基于的表索引外的其他条件组成的表达式,同select
		* @param	params		查询参数集合
		* @return	返回查询到的记录条数
		*/
		int select(const string &tablename, const string &indexname, const string &expr, vector<string> &params);

		/**
		* deleteRec:执行删除(抛异常)
		* @param	tablename	表名称
		* @param	indexname	查询基于的表索引名称
		* @param	expr		查询基于的表索引外的其他条件组成的表达式,同select
		* @param	params		查询参数集合,如果是和索引相关,MmParam参数里面的m_indextag=1;否则和expr相关的,m_indextag=0
		* @return	返回删除的记录条数
		*/		
		int deleteRec(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params);

		/**
		* deleteRec:执行删除(抛异常)
		* @param	tablename	表名称
		* @param	indexname	查询基于的表索引名称
		* @param	expr		查询基于的表索引外的其他条件组成的表达式,同select
		* @param	params		查询参数集合
		* @return	返回查询到的记录条数
		*/		
		int deleteRec(const string &tablename, const string &indexname, const string &expr, vector<string> &params);

		/**
		* insert:执行插入(抛异常)
		* @param	tablename	表名称
		* @param	params		查询参数集合,MmParam参数里面的m_indextag随便填写0/1
		* @return	返回插入的记录条数
		*/		
		int	insert(const string &tablename, vector<MmParam> &params);

		/**
		* insert:执行插入(抛异常)
		* @param	tablename	表名称
		* @param	params		查询参数集合
		* @return	返回插入的记录条数
		*/		
		int	insert(const string &tablename, vector<string> &params);

		/**
		* insert:执行插入(抛异常)
		* @param	tablename	表名称
		* @param	colnames	表字段名称		
		* @param	params		查询参数集合,MmParam参数里面的m_indextag随便填写0/1
		* @return	返回插入的记录条数
		*/		
		int	insert(const string &tablename, vector<string> &colnames, vector<MmParam> &params);

		/**
		* insert:执行插入(抛异常)
		* @param	tablename	表名称
		* @param	colnames	表字段名称
		* @param	params		查询参数集合
		* @return	返回插入的记录条数
		*/		
		int	insert(const string &tablename, vector<string> &colnames, vector<string> &params);
		
		/**
		* update:执行更新(抛异常)
		* @param	tablename	表名称
		* @param	indexname	查询基于的表索引名称
		* @param	expr		查询基于的表索引外的其他条件组成的表达式,同select注意必须从0开始顺序增加
		* @param	params		查询参数集合,如果是和索引相关,MmParam参数里面的m_indextag=1;否则和expr相关的,m_indextag=0
		* @param	fieldnames	更新的表列名称集合
		* @param	values		更新的表列参数集合,MmParam参数里面的m_indextag随便填写0/1
		* @return	返回更新的记录条数
		*/		
		int update(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params,
				   vector<string> &fieldnames, vector<MmParam> &values);

		/**
		* update:执行更新(抛异常)
		* @param	tablename	表名称
		* @param	updatesql	更新的SQL,格式如下:indexname|同select|(column1;column2;…;columnN)注意中间不允许有空格
		* @param	values		查询参数+更新参数集合(N + N + N)
		* @return	返回更新的记录条数
		*/
		int update(const string &tablename, const string &updatesql, vector<string> &values);				   

		/**
		* getNext:获取一行记录
		* @param	无
		* @return	true:有记录 false:无记录
		*/		
		bool getNext();

		/**
		* getColumnnum:获取本次查询的记录的列数
		* @param	无
		* @return	返回本次查询的列数
		*/		
		int getColumnnum();

		/**
		* getColumnShortValue:获取记录列的短整型值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的SHORT型参数值
		*/		
		short getColumnShortValue(const unsigned int &pos);

		/**
		* getColumnIntValue:获取记录列的整型值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的INT型参数值
		*/		
		int getColumnIntValue(const unsigned int &pos);

		/**
		* getColumnLongValue:获取记录列的长整型值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的LONG型参数值
		*/		
		long getColumnLongValue(const unsigned int &pos);

		/**
		* getColumnFloatValue:获取记录列的浮点型值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的FLOAT型参数值
		*/		
		float getColumnFloatValue(const unsigned int &pos);

		/**
		* getColumnDateValue:获取记录列的日期型值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的DATE型参数值
		*/		
		string getColumnDateValue(const unsigned int &pos);
		
		/**
		* getColumnStringValue:获取记录列的字符串值
		* @param	pos		记录中第几列,从1开始
		* @return	返回查询记录该列的STRING型参数值
		*/		
		string getColumnStringValue(const unsigned int &pos);

		/**
		* truncate:清空指定表(抛异常)
		* @param	tablename	表名
		* @return	无
		*/		
		void truncate(const string &tablename);
	
	private:
		
		/**
		* getColumnName:根据表达式求解涉及的表字段列名称集合
		* @param	expr		表达式((msisdn=:0) and (startTime <= :1))
		* @param	columnNames	表字段列名称集合
		* @return	无
		*/
		void getColumnName(const char* expr, vector<string>& columnNames);
		
		/**
		* getColumnType:根据表描述符和表列名称获取列值类型
		* @param	tabledef	表描述符
		* @param	columnName	表列名称		
		* @return	列值类型
		*/
		COLUMN_VAR_TYPE getColumnType(const TableDef* tabledef, const char* columnName);
		
	private:
		
		//内存数据库类,提供连接、断开等操作
		DataBase		*m_db;
		//内存数据库会话类
		Session			*m_session;
		//本次查询的列数
		int				m_columnnum;
		//事务开启标志 0:未开启 1:开启
		unsigned char	m_transflag;

	public:
	   //容错处理改进 wangfc@lianchuang.com 2010.04.26
	   //内存库提示错误时，提示到位于哪个数据库上报错
	   string        m_alisname ;
	   //设置别名
	   void         setalisname(string& name ) 
	   {
	      m_alisname = name ;
	   }
	   //读取别名 
	   string      getalisname()
	   {
	     return m_alisname ;
	   }
};


#endif

