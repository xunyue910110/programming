//written by chenzm@2009-3-9
#ifndef COcciAPIH
#define COcciAPIH

#include <iostream>
#include <map>
#include <occi.h>
using namespace oracle::occi;
using namespace std;
//OCCIConnect,OCCISQL,如果执行出错,都会抛出 SQLException 异常,程序中需要注意捕捉
class OCCISQL;
typedef struct _COLUMN_ATTRIBUTE_
{
	int    m_pos;             //字段的顺序
	string m_columnName;      //字段名称
	int    m_columnType;      //字段类型
	int    m_columnSize;      //字段长度
	int    m_columnPrecision; 
	int    m_columnScale;
	bool   m_nullAble;
} ColumnAttribute;
class OCCIConnect
{
	friend class OCCISQL;
	public:
		OCCIConnect():env(NULL),conn(NULL){}
		OCCIConnect(const string&user, const string &passwd, const string &db,bool multiThreadsSupport=false);
		~OCCIConnect();
		bool connect(const string&user, const string &passwd, const string &db,bool multiThreadSupport=false);
		bool disConnect();
		void commit()
		{
			if (conn == NULL) return;
			return conn->commit();
		}
		void rollback()
		{
			if (conn == NULL) return;
			return conn->rollback();
		}
		const Environment * getEnv() const{return env;}
		bool getTableColumnAttributes(const string& r_tableName, map<string,ColumnAttribute> & r_mAttrs);
		bool getTableColumnAttributes(const string& r_tableName, vector<ColumnAttribute> & r_vAttrs);
	private:
		Environment *env;
		Connection *conn;
		Statement* createStatement(const string &sql =""){return conn->createStatement(sql);}
		void terminateStatement(Statement *statement){return conn->terminateStatement(statement);}
};
class OCCISQL
{
	private:
		OCCIConnect *occicon;
	public:
		
		OCCISQL(OCCIConnect *ob,const string& sqlStr="")
				:occicon(ob),rset(NULL),stmt(NULL)
		{
			createStatement(sqlStr);
		}
		~OCCISQL()
		{
			destroy();
		}
		void destroy()
		{
			closeResultSet(); terminateStatement();
		}
		//一般不要使用stmt变量，但如你对他的方法很熟悉，
		//或OCCISQL未提供你需要方法的公共接口，可以直接使用
		Statement *stmt;
		
		//一般不要使用rset变量，但如你对他的方法很熟悉，
		//或OCCISQL未提供你需要方法的公共接口，可以直接使用
		ResultSet *rset;
		void createStatement(const string &sql ="")
		{
			if (stmt == NULL)
				stmt = occicon->createStatement(sql);
		}
		void terminateStatement()
		{
			if (stmt != NULL)
				occicon->terminateStatement(stmt);
			stmt = NULL;
		}
		void setSQL(const string &sql){return stmt->setSQL(sql);}
		//executeUpdate:A) a non-query statement such as a SQL INSERT, UPDATE, DELETE statement, 
		//				B) a DDL statement such as CREATE/ALTER and so on, 
		//				C) or a stored procedure call. 
		//Returns either the row count for INSERT, UPDATE or DELETE or 0 for SQL statements that return nothing
		unsigned int executeUpdate(){return stmt->executeUpdate();}
		
		void setMaxIterations(unsigned int maxIterations){return stmt->setMaxIterations(maxIterations);}
		void setMaxParamSize(unsigned int paramIndex,unsigned int maxSize){return stmt->setMaxParamSize(paramIndex,maxSize);}

		void addIteration(){return stmt->addIteration();}
		
		//设置sql语句中绑定变量的值(Statement *stmt)
		void setInt(unsigned int paramIndex,int x){return stmt->setInt(paramIndex, x);}
		void setString(unsigned int paramIndex,const string &x){return stmt->setString(paramIndex, x);}

		void setFloat(unsigned int paramIndex,float x){return stmt->setFloat(paramIndex, x);}
		void setDouble(unsigned int paramIndex,double x){return stmt->setDouble(paramIndex, x);}
		void setNumber(unsigned int paramIndex,const Number &x){return stmt->setNumber(paramIndex, x);}
		void setDate(unsigned int paramIndex,const Date &x){return stmt->setDate(paramIndex, x);}
		void setBlob(unsigned int paramIndex,const Blob &x){return stmt->setBlob(paramIndex, x);}
		void setClob(unsigned int paramIndex,const Clob &x){return stmt->setClob(paramIndex, x);}
		void setObject(unsigned int paramIndex,PObject * x){return stmt->setObject(paramIndex, x);}
	    void setPrefetchRowCount(unsigned int rowCount){return stmt->setPrefetchRowCount(rowCount);}
		void setUInt(unsigned int paramIndex,unsigned int x){return stmt->setUInt(paramIndex, x);}
		void setNull(unsigned int paramIndex,Type type){return stmt->setNull(paramIndex, type);}
		//设置存储过程中的 out/in out 变量
		void registerOutParam(unsigned int paramIndex,Type type,unsigned int maxSize = 0,const string &sqltype = "")
		{return stmt->registerOutParam(paramIndex, type, maxSize, sqltype);}

		void closeResultSet()
		{
			if (rset != NULL) stmt->closeResultSet (rset);
			rset = NULL;
		}
		//执行查询
		void executeQuery()
		{
			closeResultSet();
			rset = stmt->executeQuery();
		}
		//以下2个获得结果集字段属性的方法要在 executeQuery方法之后调用
		bool getColumnAttributes(vector<ColumnAttribute> & r_vAttrs);
		bool getColumnAttributes(map<string,ColumnAttribute> & r_vAttrs);
		
		void setParamDataBuffer(int paramIndex,void *buffer,Type type,sb4 size,ub2 *length,sb2 *ind = NULL, ub2 *rc = NULL)
		{
			return stmt->setDataBuffer(paramIndex,buffer,type,size,length,ind,rc);
		}
		//setParamDataBufferArray用于存储过程
		void setParamDataBufferArray(unsigned int paramIndex,void *buffer,Type type,ub4 arraySize, ub4 *arrayLength,
		   						sb4 elementSize,ub2 *elementLength,sb2 *ind = NULL,ub2 *rc = NULL)
		{
			return stmt->setDataBufferArray(paramIndex,buffer,type,arraySize,arrayLength,elementSize,elementLength,ind,rc);
		}		
		Statement::Status executeArrayUpdate(unsigned int arrayLength){return stmt->executeArrayUpdate(arrayLength);}

		//从结果集中获取记录(fetch,next功能相同)
		ResultSet::Status fetch(unsigned int numRows =1){return rset->next(numRows);}//usage: while (fetch()){...}
		ResultSet::Status next(unsigned int numRows =1){return rset->next(numRows);}
		unsigned int getNumArrayRows() const{return rset->getNumArrayRows();}
		//从结果集中获取输出字段值
		Blob getBlob(unsigned int colIndex){return rset->getBlob(colIndex);}
		Clob getClob(unsigned int colIndex){return rset->getClob(colIndex);}
		vector<MetaData> getColumnListMetaData() const{return rset->getColumnListMetaData();}
		Date getDate(unsigned int colIndex){return rset->getDate(colIndex);}
		double getDouble(unsigned int colIndex){return rset->getDouble(colIndex);}
		float getFloat(unsigned int colIndex){return rset->getFloat(colIndex);}
		int getInt(unsigned int colIndex){return rset->getInt(colIndex);}
		Number getNumber(unsigned int colIndex){return rset->getNumber(colIndex);}
		PObject * getObject(unsigned int colIndex){return rset->getObject(colIndex);}
		string getString(unsigned int colIndex){return rset->getString(colIndex);}
		unsigned int getUInt(unsigned int colIndex){return rset->getUInt(colIndex);}
		bool isNull(unsigned int colIndex) const{return rset->isNull(colIndex);}
		//ResultSet::Status status() const{return rset->Status();}//Status:RESULT_SET_AVAILABLE,END_OF_FETCH,DATA_AVAILABLE,STREAM_DATA_AVAILABLE 
		void setResultDataBuffer(int colIndex,void *buffer,Type type,sb4 size=0,ub2 *length=NULL,sb2 *ind = NULL, ub2 *rc = NULL)
		{
			return rset->setDataBuffer(colIndex,buffer,type,size,length,ind,rc);
		}
		void commit(){return occicon->commit();}
		void rollback(){return occicon->rollback();}
};
#endif

/*
SQLException Class
	The SQLException class provides information on generated errors, their codes and associated messages
Method  		Summary  
getErrorCode()  Return the database error code.  
getMessage()  	Return the error message string for this exception. 
setErrorCtx()  	Set the error context. 

getErrorCode()	Gets the database error code. 
	Syntax
	int getErrorCode() const;

getMessage() Returns the error message string of this SQLException if it was created with an error message string. 
             Returns null if the SQLException was created with no error message. 
	Syntax
	string getMessage() const;

setErrorCtx() Sets the pointer to the error context. 
	Syntax
		void setErrorCtx(void *ctx);
	Parameters
		ctx The pointer to the error context. 
*/
/*
更多Statement方法,请参阅:http://zuse.esnig.cifom.ch/database/doc_oracle/Oracle901_Linux/appdev.901/a89860/cci08ref.htm
Summary of Statement Methods
Method  						Description  
addIteration()  				Add an iteration for execution. 
closeResultSet()  				Immediately releases a result set's database and OCCI resources instead of waiting for automatic release. 
closeStream()  					Close the stream specified by the parameter stream. 
execute()  						Execute the SQL statement. 
executeArrayUpdate()  			Execute insert/update/delete statements which use only the setDataBuffer() or stream interface for bind parameters.  
executeQuery()  				Execute a SQL statement that returns a single ResultSet. 
executeUpdate()  				Execute a SQL statement that does not return a ResultSet. 
getAutoCommit()  				Return the current auto-commit state. 
getBfile()  					Return the value of a BFILE as a Bfile object.  
getBlob()  						Return the value of a BLOB as a Blob object.  
getBytes()  					Return the value of a SQL BINARY or VARBINARY parameter as Bytes. 
getCharSet()  					Return the character set that is in effect for the specified parameter. 
getClob()  						Return the value of a CLOB as a Clob object. 
getConnection()   				This method returns the connection from which the persistent object was instantiated.
getCurrentIteration()  			Return the iteration number of the current iteration that is being processed. 
getCurrentStreamIteration()  	Return the current iteration for which stream data is to be read or written. 
getCurrentStreamParam()  		Return the parameter index of the current output Stream that must be read or written. 
getCursor()  					Return the REF CURSOR value of an OUT parameter as a ResultSet. 
getDatabaseNCHARParam()  		Return whether data is in NCHAR character set. 
getDate()  						Return the value of a parameter as a Date object 
getDouble() 					Return the value of a parameter as a C++ double. 
getFloat()  					Return the value of a parameter as a C++ float. 
getInt()  						Return the value of a parameter as a C++ int. 
getIntervalDS()  				Return the value of a parameter as a IntervalDS object. 
getIntervalYM()  				Return the value of a parameter as a IntervalYM object. 
getMaxIterations()  			Return the current limit on maximum number of iterations.  
getMaxParamSize()  				Return the current max parameter size limit. 
getNumber()  					Return the value of a parameter as a Number object. 
getObject()  					Return the value of a parameter as a PObject. 
getOCIStatement()  				Return the OCI statement handle associated with the Statement. 
getRef()  						Return the value of a REF parameter as RefAny 
getResultSet()  				Return the current result as a ResultSet. 
getRowid()  					Return the rowid param value as a Bytes object. 
getSQL()  						Return the current SQL string associated with the Statement object. 
getStream()  					Return the value of the parameter as a stream. 
getString()  					Return the value of the parameter as a string.  
getTimestamp()  				Return the value of the parameter as a Timestamp object 
getUInt()  						Return the value of the parameter as a C++ unsigned int 
getUpdateCount()  				Return the current result as an update count for non-query statements. 
getVector()  					Return the specified parameter as a vector. 
isNull()  						Check whether the parameter is null. 
isTruncated()  					Check whether the value is truncated. 
preTruncationLength()   
registerOutParam()  			Register the type and max size of the OUT parameter. 
setAutoCommit()  				Specify auto commit mode. 
setBfile()  					Set a parameter to a Bfile value. 
setBinaryStreamMode()  			Specify that a column is to be returned as a binary stream. 
setBlob()  						Set a parameter to a Blob value. 
setBytes()  					Set a parameter to a Bytes array.  
setCharacterStreamMode()  		Specify that a column is to be returned as a character stream. 
setCharSet()  					Specify the character set for the specified parameter. 
setClob()  						Set a parameter to a Clob value. 
setDate()  						Set a parameter to a Date value. 
setDatabaseNCHARParam()  		Set to true if the data is to be in the NCHAR character set of the database; set to false to restore the default. 
setDataBuffer()  				Specify a data buffer where data would be available for reading or writing.  
setDataBufferArray()  			Specify an array of data buffers where data would be available for reading or writing.  
setDouble()  					Set a parameter to a C++ double value. 
setErrorOnNull()  				Enable/disable exceptions for reading of null values.  
setErrorOnTruncate()  			Enable/disable exception when truncation occurs. 
setFloat()  					Set a parameter to a C++ float value. 
setInt()  						Set a parameter to a C++ int value. 
setIntervalDS()  				Set a parameter to a IntervalDS value. 
setIntervalYM()  				Set a parameter to a IntervalYM value. 
setMaxIterations()  			Set the maximum number of invocations that will be made for the DML statement. 
setMaxParamSize()  				Set the maximum amount of data that can sent or returned from the parameter. 
setNull()  						Set a parameter to SQL null.  
		Set a parameter to SQL null. Note that you must specify the parameter's SQL type. 		
		Syntax:		void setNull(unsigned int paramIndex,Type type);
		Parameters :paramIndex	The first parameter is 1, the second is 2, . . . . 
				   :type		SQL type code defined by Type

setNumber()  					Set a parameter to a Number value. 
setObject()  					Set the value of a parameter using an object. 
setPrefetchMemorySize()  		Set the amount of memory that will be used internally by OCCI to store data fetched during each round trip to the server. 
setPrefetchRowCount()  			Set the number of rows that will be fetched internally by OCCI during each round trip to the server. 
setRef()  						Set a parameter to a RefAny value. 
setRowid()  					Set a row id bytes array for a bind position. 
setSQL()  						Associate a new SQL string with a Statement object. 
setString()  					Set a parameter to an string value. 
setTimestamp()  				Set a parameter to a Timestamp value. 
setUInt()  						Set a parameter to a C++ unsigned int value. 
setVector()  					Set a parameter to a vector of unsigned int. 
status()  						Return the current status of the statement. This is useful when there is streamed data to be written. 
*/
/*
更多 ResultSet 方法,请参阅:http://zuse.esnig.cifom.ch/database/doc_oracle/Oracle901_Linux/appdev.901/a89860/cci08ref.htm
Summary of ResultSet Methods
Method  Description  
cancel()  Cancel the ResultSet. 
closeStream()  Close the specified Stream. 
getBfile()  Return the value of a column in the current row as a Bfile. 
getBlob()  Return the value of a column in the current row as a Blob object. 
getBytes()  Return the value of a column in the current row as a Bytes array. 
getCharSet()  Return the character set in which data would be fetched. 
getClob()  Return the value of a column in the current row as a Clob object. 
getColumnListMetaData()  Return the describe information of the result set columns as a MetaData object. 
getCurrentStreamColumn()  Return the column index of the current readable Stream. 
getCurrentStreamRow()  Return the current row of the ResultSet being processed. 
getCursor()  Return the nested cursor as a ResultSet.  
getDate()  Return the value of a column in the current row as a Date object. 
getDouble()  Return the value of a column in the current row as a C++ double. 
getFloat()  Return the value of a column in the current row as a C++ float. 
getInt()  Return the value of a column in the current row as a C++ int. 
getIntervalDS()  Return the value of a column in the current row as a IntervalDS. 
getIntervalYM()  Return the value of a column in the current row as a IntervalYM. 
getMaxColumnSize()  Return the maximum amount of data to read from a column. 
getNumArrayRows()  Return the actual number of rows fetched in the last array fetch when next(int numRows) returned END_OF_DATA. 
getNumber()  Return the value of a column in the current row as a Number object. 
getObject()  Return the value of a column in the current row as a PObject. 
getRef()  Return the value of a column in the current row as a Ref. 
getRowid()  Return the current ROWID for a SELECT FOR UPDATE statement. 
getRowPosition()  Return the Rowid of the current row position. 
getStatement()  Return the Statement of the ResultSet. 
getStream()  Return the value of a column in the current row as a Stream. 
getString()  Return the value of a column in the current row as a string. 
getTimestamp()  Return the value of a column in the current row as a Timestamp object. 
getUInt()  Return the value of a column in the current row as a C++ unsigned int 
getVector()  Return the specified parameter as a vector. 
isNull()  Check whether the value is null. 
isTruncated()  Check whether truncation has occurred. 
next()  Make the next row the current row in a ResultSet. 
preTruncationLength()   
setBinaryStreamMode()  Specify that a column is to be returned as a binary stream. 
setCharacterStreamMode()  Specify that a column is to be returned as a character stream. 
setCharSet()  Specify the character set in which the data is to be returned.  
setDataBuffer()  Specify the data buffer into which data is to be read. 
setErrorOnNull()  Enable/disable exception when null value is read.  
setErrorOnTruncate()  Enable/disable exception when truncation occurs. 
setMaxColumnSize()  Specify the maximum amount of data to read from a column. 
status()  Return the current status of the ResultSet. 
	Returns the current status of the result set. The status method can be called repeatedly to find out the status of the result. 
	Data is available for getxxx method when the RESULT_SET_AVAILABLE status is returned. 
	Returns one of following: 
		1:DATA_AVAILABLE -- call getxxx() or read data from buffers specified with the setDataBuffer method 
		2:STREAM_DATA_AVAILABLE -- call getCurrentStream() and read stream 
		3:END_OF_FETCH 
	Syntax
		Status status() const;
*/
