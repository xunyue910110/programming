//////////////////////////////////////////////////////////////////////
// 
// 类名: file
// 描述: 读写文件类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-16 21:45
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#if !defined(ARCHY_CFILE_HEADER_1034341_RR343)
#define ARCHY_CFILE_HEADER_1034341_RR343

#include <string>
#ifdef WIN32
#include <windows.h>
  #define INVALID_FILE_HANDLE ((UINT)-1)
  #define FILE_HANDLE UINT
#else
  #define INVALID_FILE_HANDLE NULL
  #define FILE_HANDLE FILE*
#endif

#include "archy_config.hpp"
#include "noncopyable.hpp"
#include "date_time.hpp"
namespace archy {

struct CFileStatus
{
   date_time m_ctime;          // status last change time of file
   date_time m_mtime;          // last modification date/time of file
   date_time m_atime;          // last access date/time of file
   long m_size;            // logical size of file in bytes
}; 

class file:private noncopyable
{
public:
	bool b_autoclose;
public:
	file();
	file(FILE_HANDLE fp);
	
	~file();

	// 获取文件状态
	static bool GetStatus(const char* filename, CFileStatus& rStatus);
	
	// 打开一个文件, -1 打开失败 0 成功
	int open(const std::string& strFileName, const std::string& flags);
	int close();
	
	// 读取文件中的一行
	// 读到一行，返回1, strLineRet中有换行符
	// 返回 0 没有行返回
	// 返回 -1 发生错误
	int read_line(std::string& strLine);
	int read(void* buf, int bufsize);
	
	// 写入一行，返回1表示成功,返回-1失败
	int write_line(const std::string& strLine);
	int write(const void* buf, int bufsize);

	int flush();
		
	// 获取文件大小
	long get_length();	
	static long get_length(const std::string& filename);
	
#ifdef WIN32
	enum seekposition { seek_begin = 0x0, seek_current = 0x1, seek_end = 0x2 };
#else
	enum seekposition { seek_begin = SEEK_SET, seek_current = SEEK_CUR, seek_end = SEEK_END };
#endif
	long seek( long lOff, seekposition nFrom );
	
	// 获取当前位置
	long tell();
	
	bool lock_range( long pos, long count );
	bool unlock_range( long pos, long count );
	
	void set_autoclose(bool autoclose = false);
	
	const std::string& get_filename()const { return m_filename; }

	FILE_HANDLE m_fp;
private:
	std::string m_filename;	
};

} // namespace archy

#endif //ARCHY_CFILE_HEADER_1034341_RR343
