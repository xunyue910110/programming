#ifndef CBUFFER_H_HEADER_INCLUDED_BE67DF62
#define CBUFFER_H_HEADER_INCLUDED_BE67DF62
#include "mwci/MWGlobal.h"
// Base of All Buffer.
//##ModelId=4190C88F0194
class CBuffer
{
  public:

    //! 获取内部实际数据区指针
    /*!
		返回值需要进行强制类型转换

		\return 内部实际数据区指针
		\sa SetData()
    */
    //##ModelId=4190CF330062
    virtual void * GetData() = 0;

    //! 设置内部实际数据区指针
    /*!
		\param theData 新的FBFR*指针
		\sa GetData()
    */
    //##ModelId=41C96FD20354
    virtual void SetData(void *) = 0;

    //! 获取缓冲区的数据类型
    //##ModelId=41918D5200C1
    virtual int GetBufType() const = 0;

     //! 获取内部数据实体的大小字节数
    //##ModelId=419C766B030C
    virtual long GetBufSize() const=0;

    //! 析构
    //##ModelId=4199D84200EE
    virtual ~CBuffer();

    //! 获取内部 缓冲区 指针的地址
    //##ModelId=41A2D49D0230
    virtual void * GetBufRef()=0;

    //! 修改缓冲区的大小
    //##ModelId=41AB13740146
	virtual bool Resize(long)=0;
};



#endif /* CBUFFER_H_HEADER_INCLUDED_BE67DF62 */
