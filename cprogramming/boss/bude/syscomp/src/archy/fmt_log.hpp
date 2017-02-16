//////////////////////////////////////////////////////////////////////
// 
// 类名: fmt_log
// 描述: 书写格式化日志的类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-17 8:39
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////


#if !defined(ARCHY_FMT_LOG_HEADER_CLASS_0801473198433434343420482042)
#define ARCHY_FMT_LOG_HEADER_CLASS_0801473198433434343420482042

#include "noncopyable.hpp"
#include "date_time.hpp"
#include "mutex.hpp"
#include "file.hpp"

namespace archy {

class fmt_log:private noncopyable
{
public:
	// 默认格式是 %Y-%M-%D.%O.log
	fmt_log();	
	~fmt_log();
	
	// 设置日志文件的存放路径
	bool set_filepath(const std::string& strPath, bool bAutoCreate = true);

	// 设置日志文件名的格式
	// 格式里面有%开始的一个字符为格式字符
	// %Y 四位年份; %M 两位月份; %D 两位天数; %H 两位小时数;%N 两位分钟数;%S两位秒数
	// %y 两位年份; %m 月份; %d 天数 %h小时数 %n分钟数 %s秒数
	// %% 表示一个%号
	// %nO表示n位序号，如果序号不足n位，前面补零，超过n位n不起作用
	// %O表示实际的序号，是几位就是几位，不进行补齐
	bool set_filename(const char* fmt, int* startSerialNo = 0);
	
	// 设置每行日志的行首内容，默认是[%H:%N:%S]
	void set_linehead(const char* fmt = "");
	
	// 设置产生新文件的时间间隔，
	// 如果不设置该项，而且也没有设置每个日志文件最大记录行数(set_maxlines)
	// 那么默认一天产生一个日志文件
	
	void set_interval(const time_span& span);
	
	// 设置每个日志文件记录的日志数量(行数)，如果不设置则日志文件没有最大行数限制
	void set_maxlines(int lines);
	
	// 设置日志模式, mode_stdout输出到标准输出, mode_writelog写到日志文件
	// mode_lockwrite 锁定模式， mode_nolockwrite 非锁定模式
	// mode_stdout|mode_writelog两者兼有之
	enum log_mode{ mode_stdout = 0x01, mode_writelog = 0x02, 
		mode_lockwrite = 0x04, mode_nolockwrite = 0x08};
	void set_logmode(int mode);
	
	// 书写日志行
	void write(const std::string& strLog);
	void write(const char* fmt, ...); // like printf
	void buf_write(const char* buf, int iLen);
		
	// 写日志行的一些字段, 不支持锁定模式
	void write_fields(const char* fmt, ...);
	void write_fields(const std::string& logStr);
	// 提交一个日志行到日志文件中
	void commit_fiedls();

	// get current log file name
	const std::string& get_current_logfilename()const {
		return m_file.get_filename();
	}

	void refresh();
private:
	
	bool check(bool isTest = false);
	bool generate_filename();
	bool write_log(const char* buf, int iLen, bool is_buf_write = false);
	
private:
	std::string m_filepath;
	std::string m_filenamefmt;
	std::string m_curname;
	std::string m_linehead;
	long m_maxline;
	long m_lineno;
	int m_logmode;
	archy::mutex m_mutex;
	std::string m_strlogline;
	time_span m_interval;
	date_time m_createtime;
	long m_serialNo;
	archy::file m_file;
        long m_partno;
};

} // namespace archy


#endif // ARCHY_FMT_LOG_HEADER_CLASS_0801473198433434343420482042

