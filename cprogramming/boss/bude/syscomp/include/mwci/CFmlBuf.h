//!  FML Buffer
/*!
  封装了FML Buffer 的一些操作
  注意:<B>不要FML文件中不要使用short 和 float 类型,使用long 和 doule 替代</B>

  下列数据类型没有支持
  - FLD_PTR       pointer to a buffer
  - FLD_FML32     embedded FML32 buffer
  - FLD_VIEW32    embedded VIEW32 buffer
  - FLD_MBSTRING  multibyte character array
*/

#ifndef CFMLBUF_H_HEADER_INCLUDED_BE67F029
#define CFMLBUF_H_HEADER_INCLUDED_BE67F029
#include "mwci/CBuffer.h"
#include "mwci/MWException.h"
#include "mwci/MWGlobal.h"
#include "log4cpp/Logger.h"
using namespace log4cpp;

//##ModelId=418A1213000E
class CFmlBuf : public CBuffer
{
  friend class CTuxCaller;
  friend class IMWCaller;

	public:

    /// 用给定的CFmlBuf覆盖自己
    /**
		用给定的theRight中的数据覆盖自己的数据。
		@param 用来覆盖自己的数据
		@return 覆盖后的缓冲区大小
		@sa append()
    */
	long Cover(CFmlBuf & theRight) throw (CFmlException);

    //! 获取FML Buffer (FBFR)内部实际数据区指针
    /*!
		返回值需要进行强制类型转换

		\return 内部实际数据区指针 FBFR*
		\sa SetData()
    */
	//##ModelId=4199D843013F
    virtual void * GetData() ;

    //! 设置FML Buffer (FBFR)内部实际数据区指针
    /*!
		\param theData 新的FBFR*指针
	\sa GetData()
    */
	//##ModelId=41C78B360292
    void SetData( void* theData=NULL) { m_pFmlBuf=(FBFR*)theData;};

    //! 获取缓冲区的数据类型
    /*!
		\return 使用返回TUX_BUF_TYPE_FML
    */
    //##ModelId=4199D84301F4
    virtual int GetBufType() const;

    //! 没有实现
    //##ModelId=418A1A690175
    int BatSetVal();

    //! 没有实现
    //##ModelId=418A248A0169
    int BatGetVal();

    //! 没有实现
    //##ModelId=418A24B70204
    int LoadFromFile(const char *strFileName);

    //! 没有实现
    //##ModelId=418A24D803BA
    int LoadFromTab(const char *tabName);



    //! 指定字段是否在缓存中存在
    /*!
		\param strFldName 字段名称
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return 存在返回true,不存在返回false
    */
    //##ModelId=418A36A40253
    bool IsFldExist(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 指定字段是否在缓存中存在
    /*!
		\param lFldId 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return 存在返回true,不存在返回false
    */
    //##ModelId=418B4E1201CB
    bool IsFldExist(FLDID lFldId, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取字段类型
    /*!
		\param strFldName 字段名称
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return
			- 0	short int
			- 1	long int
			- 2	character
			- 3	single-precision float
			- 4	double-precision float
			- 5	char * - null terminated
			- 6	character array
    */
    //##ModelId=418B4E710196
    int GetFldType(const char * strFldName)  throw(CFmlException);

    //! 获取字段类型
    /*!
		\param fldid 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return
			- 0	short int
			- 1	long int
			- 2	character
			- 3	single-precision float
			- 4	double-precision float
			- 5	char * - null terminated
			- 6	character array
    */
    //##ModelId=418B4EB201F4
    int GetFldType(FLDID fldid)  throw(CFmlException);

    //! 获取最大下标
    /*!
		不会抛出异常
		\param strFldName 字段名称
		\return 返回最大下标,从0开始
    */
    //##ModelId=418B4F1600AD
    int GetCount(const char *strFldName) throw(CFmlException);

    //! 获取最大下标
    /*!
		不会抛出异常
		\param fldid 字段ID
		\return 返回最大下标,从0开始
    */
    //##ModelId=418B4F4F0394
    int GetCount(FLDID fldid) throw(CFmlException);

    //! 删除指定字段
    /*!
		没有找到则抛出异常
		\param strFldName 字段名称
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return 始终返回0
    */
    //##ModelId=418B5EFF0349
    int DelFld(const char *strFldName,FLDOCC iIdx = 0) throw(CFmlException);

    //! 删除指定字段
    /*!
		没有找到择抛出异常
		\param lFldID 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\return 始终返回0
    */
    //##ModelId=418B60D602E0
    int DelFld(FLDID lFldID, FLDOCC iIdx = 0) throw(CFmlException);

    //! 把包含所有字段信息的字符串输出到standard output
    //##ModelId=418B60FE037E
    void Print();

    //! 获取内部FML数据实体的大小字节数
    /*!
		\return 大小字节数
    */
   //##ModelId=419C766B03A2
	virtual long GetBufSize() const;

     //默认构造函数申请DEFAULT_SIZE 大小的缓冲区
     //##ModelId=418F2D1C02A6
	 //CFmlBuf() throw(CFmlException);

    //! 复制构造
    //##ModelId=418F2D1C02CE
    CFmlBuf(const CFmlBuf& right) throw(CFmlException);


    //! 直接构造
    /*!
		直接对给定的FBFR * 进行包装构造
		\param right 给定的 FML Buffer 数据区指针
    */
    //##ModelId=41921273008A
    CFmlBuf(FBFR * right) throw(CFmlException);

	//! 析构
    //##ModelId=418F2D1C035B
    virtual ~CFmlBuf();

    //! 重载赋值函数
    /*!
		\param right 给定的 CFmlBuf 实体
    */
    //##ModelId=418F2D1C03A1
    CFmlBuf& operator=(const CFmlBuf& right) throw(CFmlException);

    //! 清除对象中的数据
    //##ModelId=418F5F520097
    void ClearFmlValue();

    //! 添加字段值
    /*!
		\param fldid 字段ID
		\param value 要添加的数据
		\param fldlen 数据的长度,只有在添加字符数组(char [])时需要指定
    */
    //##ModelId=41902B550003
    void AppFldValue(FLDID fldid, void *value,FLDLEN fldlen=0) throw(CFmlException);

    //! 添加字段值
    /*!
		有些编译器在编译时可能会找不到对应的函数,在value前加强制类型转换(void *)可以解决
		\param strFldName 字段名
		\param value 要添加的数据
		\param fldlen 数据的长度,只有在添加字符数组(char [])时需要指定
    */
    //##ModelId=41902B8801F1
    void AppFldValue(const char *strFldName, void *value,FLDLEN fldlen=0) throw(CFmlException);

    //! 获取Int型字段值
	/*!
		获取Int型字段值,如果该字段不是short/long型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param lFldID 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=41902C0603DD
    long GetInt(FLDID fldid, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取string返回值
	/*!
		推荐使用GetStr(),效率更高
		获取string型字段值,如果该字段不是FML 的string型(实际上是 char *),
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param lFldID 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\sa GetStr()
	*/
    //##ModelId=41902C150168
    const string GetString(FLDID fldid, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取char返回值
	/*!
		获取char型字段值,如果该字段不是FML 的char型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param lFldID 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=41902C1C0258
    char GetChar(FLDID fldid, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取double返回值
	/*!
		获取double型字段值,如果该字段不是FML 的double型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param lFldID 字段ID
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=41902CCB0228
    double GetFloat(FLDID fldid, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取Int型字段值
	/*!
		获取Int型字段值,如果该字段不是short/long型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=41902D400244
    long GetInt(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取string返回值
	/*!
		获取string型字段值,如果该字段不是FML 的string型(实际上是 char *),
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
	//##ModelId=41AC2D71011D
    const string GetString(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取char返回值
	/*!
		获取char型字段值,如果该字段不是FML 的char型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
		\sa GetStr()
	*/
    //##ModelId=41902D400276
    char GetChar(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取double返回值
	/*!
		获取double型字段值,如果该字段不是FML 的double型,
		会试图进行转换,转换失败或者字段不存在就抛出异常
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
	//##ModelId=41902D400294
    double GetFloat(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 构造函数
	//##ModelId=41C78B37009E
    CFmlBuf(long lSize=SIZE_32K);

    //! 删除指定字段
	/*! 删除指定字段名的全部数据,删除失败就抛出异常
		\param strFldName 字段名
		\return 0
	*/
    //##ModelId=41931A8C01D3
    int DelAllFld(const char *strFldName) throw(CFmlException);

    //! 删除指定字段
	/*! 删除指定字段的全部数据,删除失败就抛出异常
		\param lFldID 字段ID
		\return 0
	*/
    //##ModelId=41931A8C01DD
    int DelAllFld(FLDID lFldID) throw(CFmlException);

    //! 重载()函数
	/*!
		Example:
			- theCFmlBuf("EPARCHCODE")="0A"
			- theCFmlBuf("EPARCHCODE",1)="0B"
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=41931BE7002D
    CFmlBuf& operator()(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //! 重载赋值函数
	/*!
		Example:
			- theCFmlBuf("CONTEXTID")=1234
		\param value 值
	*/
    //##ModelId=41931C77007B
    CFmlBuf& operator=(const long value) throw(CFmlException);


    //! 重载()函数
	/*!
		Example:
			- theCFmlBuf(EPARCHCODE)="0A"
			- theCFmlBuf(EPARCHCODE,1)="0B"
		\param strFldName 字段名
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=419327EB02B7
    CFmlBuf& operator()(FLDID lFldId, FLDOCC iIdx = 0) throw(CFmlException);

    //! 重载赋值函数
	/*!
		Example:
			- theCFmlBuf("CONTEXTID")=1234
		\param value 值
	*/
    //##ModelId=4193282002EF
    CFmlBuf& operator=(const int value) throw(CFmlException);

    //! 重载赋值函数
	/*!
		Example:
			- theCFmlBuf("STRINFO")="look at me !"
		\param value 值
	*/
    //##ModelId=419328400115
    CFmlBuf& operator=(const char *value) throw(CFmlException);

	//! 重载 ==
	/*!
		\param a 左值
		\param b 右值
		\return 相等返回true ,否则返回false
	*/
    //##ModelId=41C96FD3002A
    friend bool operator==(const CFmlBuf& a, const CFmlBuf& b)
    { return !Fcmp(a.m_pFmlBuf,b.m_pFmlBuf);}

    //! 重载赋值函数
	/*!
		Example:
			- theCFmlBuf("CHARINFO")='@';
		\param value 值
	*/
    //##ModelId=419328AE02C2
    CFmlBuf& operator=(const char value) throw(CFmlException);

    //! 设置long型数据
	/*!
		注意:我们会把short/int都转换成long型
		\param fldid 字段ID
		\param iValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B700038E
    void SetInt(FLDID fldid, long iValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置String型数据
	/*!
		注意:我们会把String型转换成char*存放到FML Buffer
		\param fldid 字段ID
		\param strValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B7010096
    void SetString(FLDID fldid, string strValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置String型数据
	/*!
		注意:我们会把String型转换成char*存放到FML Buffer
		\param strFldName 字段名
		\param strValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=419AF8070268
    void SetString(const char * strFldName, string strValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置char型数据
	/*!
		注意:我们会把String型转换成char*存放到FML Buffer
		\param fldid 字段ID
		\param strValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B7010190
    void SetChar(FLDID fldid, char cValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置double型数据
	/*!
		注意:我们会把fload型转换成double存放到FML Buffer
		\param fldid 字段ID
		\param fValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B7010281
    void SetFloat(FLDID fldid, double fValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置long型数据
	/*!
		注意:我们会把short/int都转换成long型
		\param strFldName 字段名
		\param iValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B701037B
    void SetInt(const char *strFldName, long iValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置char型数据
	/*!
		注意:我们会把String型转换成char*存放到FML Buffer
		\param strFldName 字段名
		\param strValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B702019C
    void SetChar(const char *strFldName, char cValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 设置double型数据
	/*!
		注意:我们会把fload型转换成double存放到FML Buffer
		\param strFldName 字段名
		\param fValue 要存放的值
		\param iIdx 用来表示第几个数据,可以理解成数组的下标
	*/
    //##ModelId=4194B70202A0
    void SetFloat(const char *strFldName, double fValue, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取字段名对应的字段ID
	/*!
		注意:没有找到,就抛出异常
		\param strFldName 字段名
		\return 字段ID
		\sa GetFldName
	*/
    //##ModelId=4199D8440078
    FLDID GetFldID(const char *strFLDName) throw(CFmlException);

    //! 获取字段ID对应的字段名
	/*!
		注意:没有找到,就抛出异常
		\param fldid 字段ID
		\return 字段名
		\sa GetFldID
	*/
    //##ModelId=419B3E0B0366
    char *GetFldName(FLDID fldid);

    //! 把另一个CFmlBuf中的数据,添加到自己的数据中
	/*!
		\param right 另一个CFmlBuf
		\return 最新缓冲区的大小
	*/
    //##ModelId=419B530400DB
    long Append (CFmlBuf& right);

    //! 把另一个CFmlBuf中的一行数据,添加到自己的一行数据中
	/*!
		\param right 另一个CFmlBuf
		\return 最新缓冲区的大小
	*/
    //##ModelId=419B530400DB
    long CoverLine(int targetLine, CFmlBuf& right, int sourceLine) throw(CFmlException);

    //##ModelId=41AB1374024A
	//! 修改缓冲区的大小
	/*!
		<b>注意:如果操作失败,该缓冲区将不能使用!!</b>
		\param newSize 新缓冲区的大小
		\return 操作成功返回true,失败返回false
	*/
	virtual bool Resize(long newSize);

	//! 缓冲区中使用的字节数
	/*!
		\return 缓冲区中使用的字节数
	*/
    //##ModelId=41AC109F011E
    long GetUsed();


	//! 缓冲区中未使用的字节数
	/*!
		\return 缓冲区中未使用的字节数
	*/
    //##ModelId=41AC10DC00FE
    long GetUnused();


	//! 缓冲区中index使用的字节数
	/*!
		\return 缓冲区中index使用的字节数
	*/
    //##ModelId=41AC112E0124
    long GetIdxUsed();


    //! 获取下一个字段数据
	/*!
     \param fldid 字段ID指针
	 \param fldidx 字段下标
	 \return
      - 1 找到
      - 0 到达缓冲区末尾
      - -1 出错
	*/
  //##ModelId=41AC128E013E
    int GetNext(FLDID *fldid, FLDOCC *fldidx,
        // value.
        void *value = NULL,
        // Buffer size for value.
        FLDLEN *len = NULL)  throw(CFmlException) ;

    //! 获取char *返回值
	/*!
     \param fldid 字段ID
	 \param fldidx 字段下标
	 \return 值
	*/
    //##ModelId=41902D400258
    char *GetStr(FLDID fldid, FLDOCC iIdx = 0) throw(CFmlException);


    //! 获取char *返回值
	/*!
     \param strFldName 字段名
	 \param fldidx 字段下标
	 \return 值
	*/
    //##ModelId=41AC2D6F01EC
    char * GetStr(const char *strFldName, FLDOCC iIdx = 0) throw(CFmlException);

    //!  获取 全部字段数
	//! \return 全部字段数
    //##ModelId=41AC5EFC0088
    FLDOCC CountAll();

    //!  是否为空
	//! \return true空,false非空
    //##ModelId=41AC60740238
    bool IsEmpty();

	//! 释放掉内部的FML缓冲区
    //##ModelId=41C78B3701CB
    void Destory() { if(m_pFmlBuf) tpfree( (char*)m_pFmlBuf); m_pFmlBuf=NULL; }

	//! 将全部FML数据输出到指定的输出流
    //##ModelId=41D163700162
	int printFmlBuffer(FILE *out=stdout);

	//! 将全部FML数据输出到指定的logger对象文件中
    //##ModelId=41D163700162
	int printFmlBufLogger(Logger& logfile);
	
	//! 将全部FML数据输出成字符串
    //##ModelId=41D163700162
	const string ToString();
	const string ToString1();

    //! 获取FML Buffer 中的数据
    /*!
      <B>该方法可是控制是否产生异常，调用者必须保证 value指针的数据类型正确</B>
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放取得的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \param bIfThrowEx 如果获取数据失败是否需要抛出异常
      \return 如果数据获取失败返回-1,不支持的数据返回-2,其他返回>0 的值代表数据类型
	  \sa GetValue()
    */
    int GetValueEx(FLDID lFldId, void *value, FLDOCC iIdx = 0,bool bIfThrowEx=false) throw(CFmlException);

    //! 获取FML Buffer 中的数据
    /*!
      <B>该方法可是控制是否产生异常，调用者必须保证 value指针的数据类型正确</B>
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放取得的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \param bIfThrowEx 如果获取数据失败是否需要抛出异常
      \return 如果数据获取失败返回-1,不支持的数据返回-2,其他返回>0 的值代表数据类型
	  \sa GetValue()
    */
    int GetValueEx(const char *strFldName , void *value, FLDOCC iIdx = 0,bool bIfThrowEx=false) throw(CFmlException);

	//! 设置FML Buffer 中的数据
    /*!
	  <B>该方法可是控制是否产生异常，调用者必须保证 value指针的数据类型正确</B>
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \param bIfThrowEx 如果操作失败是否需要抛出异常
	  \return  - 0  操作成功
			   - -1 操作失败
	  \sa SetInt(),SetFloat(),SetChar,SetString
    */
    int SetValueEx(FLDID lFldid, void *value, FLDOCC iIdx = 0,FLDLEN fldlen=0,bool bIfThrowEx=false) throw(CFmlException);

	//! 设置FML Buffer 中的数据
    /*!
	  <B>该方法可是控制是否产生异常，调用者必须保证 value指针的数据类型正确</B>
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \param bIfThrowEx 如果操作失败是否需要抛出异常
	  \return  - 0  操作成功
			   - -1 操作失败
	  \sa SetInt(),SetFloat(),SetChar,SetString
    */
    int SetValueEx(const char *strFldName , void *value, FLDOCC iIdx = 0,FLDLEN fldlen=0,bool bIfThrowEx=false) throw(CFmlException);


    //! 计算缓冲区的大小
    //##ModelId=419B514C01BF
    long CalBufSize() const;


    //! 获取内部 FML缓冲区(FBFR) 指针的地址
    /*!
		该方法只在CTuxCaller类内部使用,可以使用该函数替换掉当前类实力中的实际数据
    */
    //##ModelId=41A599A20353
	virtual void * GetBufRef();


	//! 设置FML Buffer 中的数据
    /*!
      只在内部使用
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \sa SetInt(),SetFloat(),SetChar,SetString
    */
	//##ModelId=418A36A4020D
    void SetValue(FLDID lFldid, void *value, FLDOCC iIdx = 0,FLDLEN fldlen=0) throw(CFmlException);

	//! 设置FML Buffer 中的数据
    /*!
      只在内部使用
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \sa SetInt(),SetFloat(),SetChar,SetString
    */
	//##ModelId=418A36A4020D
    void SetValue(const char *strFldName, void *value, FLDOCC iIdx = 0,FLDLEN fldlen=0) throw(CFmlException);

    //! 获取FML Buffer 中的数据
    /*!
      只在内部使用
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放取得的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \sa GetValueEx()
    */
    //##ModelId=418A36A4022B
    int GetValue(FLDID lFldId, void *value, FLDOCC iIdx = 0) throw(CFmlException);

    //! 获取FML Buffer 中的数据
    /*!
      只在内部使用
      \param lFldId FML域文件中对应的字段编码
      \param value 用来存放取得的数据
	  \param iIdx 用来表示第几个数据,可以理解成数组的下标
	  \sa GetValueEx()
    */
    //##ModelId=418A36A4022B
    int GetValue(const char *strFldName, void *value, FLDOCC iIdx = 0) throw(CFmlException);

  protected:

  private:
	//! 内部FML 缓冲区指针
	//##ModelId=418A18EE0242
    FBFR *m_pFmlBuf;

	//! 变量
    //##ModelId=419AF8030335
    FLDID m_fldid;

	//! 变量
    //##ModelId=419AF80700BA
    FLDOCC m_fldidx;

	//! 变量
    //##ModelId=419C52880059
    short siValue;

	//! 变量
    //##ModelId=419C5288018F
    long liValue;

	//! 变量
    //##ModelId=419C5288024E
    float fValue;

	//! 变量
    //##ModelId=419C52880302
    double dValue;


};



#endif /* CFMLBUF_H_HEADER_INCLUDED_BE67F029 */
