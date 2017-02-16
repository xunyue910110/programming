//////////////////////////////////////////////////////////////////////
// 
// 类名: bufwrap
// 描述: 二进制缓冲区包装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-3-5 
// 版本: V1.0.1 
// 版权: 南京联创科技股份有限公司
// 修改记录: 修改缓冲区的申请形式，不再是只申请所需要的缓冲区
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFWRAP_H__3D9F09E0_7348_45CC_9881_A9F486175AF7__INCLUDED_)
#define AFX_BUFWRAP_H__3D9F09E0_7348_45CC_9881_A9F486175AF7__INCLUDED_


// 二进制缓冲区包装类
namespace archy
{
class bufwrap  
{
public:
	// 每次缓冲区扩充至少是STEP_SIZE的整数倍
	enum { STEP_SIZE = 64 };
	// 获得缓冲区的内容
	void* getbuf()const { return (void*)m_vBuf; }
	char* getcbuf()const { return m_vBuf; 	}
	// 获得缓冲区的大小
	unsigned int size()const { return m_iLen; }

	// 在原有的缓冲区的基础上添加上另外一个缓冲区的内容
	bufwrap& operator += (const bufwrap &r);
	void append(const void* vBuf, unsigned int iLen);

	bufwrap& operator = (const bufwrap &r);
	void setbuf(const void* vBuf, unsigned int iLen);


	void assign(const void* vBuf, unsigned int iLen);
	void assign(const char* cBuf);

	// 获得子缓冲区对象
	bufwrap subbuf(unsigned int iStart, unsigned int iLen)const;
	bufwrap subbuf(unsigned int iStart)const;	 
	
	// Replace the buffer from iStart with the (vBuf, iLen)
	void replace(unsigned int iStart, const void* vBuf, unsigned int iLen);
	
	// 清空缓冲
	void clear();
	
	// 清除指定位置开始，长度为iLen的缓冲区
	void erase(unsigned int iStart, unsigned int iLen);

	int find(const void* search, unsigned int len);
	
	// 是否是Empty
	bool empty()const;

	// 对缓冲区中的NULL进行替换,返回替换的数目
	int null_deal(char chReplaceChar = ' ');

	// 改变大小
	// 原来内容会最大的保存下来
	
	void resize(unsigned int iLen);
	
	// ctors
	bufwrap(const void* vBuf, unsigned int iLen);
	bufwrap(const bufwrap& r);
	bufwrap();

	char& operator[](int iIndex);
	char operator[](int iIndex)const;

	// dtor
	~bufwrap();
	
private:
	char* m_vBuf;        // 缓冲区首地址
	unsigned int m_iLen; // 缓冲区已使用的长度
	unsigned int m_iSize; // 缓冲区长度
};

bufwrap operator + (const bufwrap& r1, const bufwrap& r2);
bool operator == (const bufwrap& r1, const bufwrap& r2);
bool operator != (const bufwrap& r1, const bufwrap& r2);

} // namespace archy

#endif // !defined(AFX_BUFWRAP_H__3D9F09E0_7348_45CC_9881_A9F486175AF7__INCLUDED_)
