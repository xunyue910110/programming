#include "fmt_log.hpp"
#include "path.hpp"
#include "operations.hpp"
#include "stringutils.hpp"
#include <stdarg.h>

namespace archy {

// 默认格式是 %Y-%M-%D.%O.log
fmt_log::fmt_log()
:m_filenamefmt("%Y-%M-%D.%O.log")
,m_linehead("[%H:%N:%S]")
,m_maxline(-1)
,m_logmode(mode_writelog|mode_nolockwrite)
,m_interval(time_span(1, 0, 0, 0))
,m_serialNo(-1)
,m_lineno(0)
,m_partno(0)
{
}



fmt_log::~fmt_log()
{
	if (m_lineno > 0 ) {
		string strFileName = m_file.get_filename();
		m_file.close();
		// change file name away from temp log file
		filesystem::rename(strFileName, m_filepath + m_curname);
		m_lineno = 0;
	}
}

// 设置日志文件的存放路径
bool fmt_log::set_filepath(const std::string& strPath, bool bAutoCreate)
{
	namespace fs = archy::filesystem;
	try {	
		fs::path logpath(strPath, fs::native);
		if ( !fs::exists( logpath ) ) {
			if( bAutoCreate ) {
				fs::create_directories(logpath);
			}
			else return false;		
		}
		m_filepath = logpath.native_directory_string();
        #ifdef WIN32
                if(m_filepath[m_filepath.length()-1] != '\\' )
                        m_filepath += "\\";
        #else
                if(m_filepath[m_filepath.length()-1] != '/' )
                        m_filepath += "/";
        #endif

//std::cout << "filepath:" << m_filepath << std::endl;
		
	}catch( const std::exception & ex){
	        std::cout <<  ex.what() << std::endl;
		return false;
	}
	return true;
}

// 设置日志文件名的格式
// 格式里面有%开始的一个字符为格式字符
// %Y 四位年份; %M 两位月份; %D 两位天数; %H 两位小时数;%N 两位分钟数;%S两位秒数
// %y 两位年份; %m 月份; %d 天数 %h小时数 %n分钟数 %s秒数
// %% 表示一个%号
// %nO表示n位序号，如果序号不足n位，前面补零，超过n位n不起作用
// %O表示实际的序号，是几位就是几位，不进行补齐
bool fmt_log::set_filename(const char* fmt, int* startSerialNo/* = 0*/)
{
	m_filenamefmt = fmt;
	m_serialNo = startSerialNo ? *startSerialNo : 0;
	return true;
}

// 设置每行日志的行首内容，默认是[%H:%N:%S]
void fmt_log::set_linehead(const char* fmt /*= ""*/)
{
	m_linehead = fmt;
}

// 设置产生新文件的时间间隔，
// 如果不设置该项，而且也没有设置每个日志文件最大记录行数(set_maxlines)
// 那么默认一天产生一个日志文件

void fmt_log::set_interval(const time_span& span)
{
	m_interval = span;
}

// 设置每个日志文件记录的日志数量(行数)，如果不设置则日志文件没有最大行数限制
void fmt_log::set_maxlines(int lines)
{
	m_maxline = lines;
}

// 设置日志模式, mode_stdout输出到标准输出, mode_writelog写到日志文件
// mode_stdout|mode_writelog两者兼有之
// enum log_mode{ mode_stdout = 0x1, mode_writelog = 0x2 };
void fmt_log::set_logmode(int mode)
{
	m_logmode = mode;
}


// 书写日志行，不适合多个线程使用同一个对象
void fmt_log::write(const std::string& strLog)
{
	if( m_logmode & mode_lockwrite )
		m_mutex.do_lock();

	write_log(strLog.c_str(), strLog.length());

	if( m_logmode & mode_lockwrite )
		m_mutex.do_unlock();
}

void fmt_log::write(const char* fmt, ...) // lick printf
{
	if( m_logmode & mode_lockwrite )
		m_mutex.do_lock();

	char buf[256];
	va_list argp;
	va_start(argp,fmt);
#ifndef WIN32
	::vsnprintf( buf, sizeof(buf), fmt, argp );
#else
	::_vsnprintf( buf, sizeof(buf), fmt, argp );
#endif
	va_end(argp);
	write_log(buf, strlen(buf));

	if( m_logmode & mode_lockwrite )
		m_mutex.do_unlock();
	
}

void fmt_log::buf_write(const char* buf, int iLen)
{
	if( m_logmode & mode_lockwrite )
		m_mutex.do_lock();

	write_log(buf, iLen, true);

	if( m_logmode & mode_lockwrite )
		m_mutex.do_unlock();
}


// 写日志行的一些字段
void fmt_log::write_fields(const char* fmt, ...)
{
	char buf[256];
	va_list argp;
	va_start(argp,fmt);
#ifndef WIN32
	::vsnprintf( buf, sizeof(buf), fmt, argp );
#else
	::_vsnprintf( buf, sizeof(buf), fmt, argp );
#endif
	va_end(argp);
	m_strlogline += buf;
	
}

void fmt_log::write_fields(const std::string& logStr)
{
	m_strlogline += logStr;
}

// 提交一个日志行到日志文件中
void fmt_log::commit_fiedls()
{
	write_log(m_strlogline.c_str(), m_strlogline.length());
	m_strlogline = "";
}

bool fmt_log::write_log(const char* buf, int iLen, bool is_buf_write)
{
	bool bRet = fmt_log::check(iLen == 0 ? true : false);
	if( bRet == false )
		return false;
	
	int iRet;
	if( m_logmode & mode_stdout ) {
		if( !is_buf_write ) {
			std::cout << date_time::current().format(m_linehead.c_str()) 
				<< buf << std::endl;
		}
	}
	if( m_logmode & mode_writelog ) {
		std::string lineHead = date_time::current().format(m_linehead.c_str());
		iRet = m_file.write(lineHead.c_str(), lineHead.length());
		if( (unsigned)iRet < lineHead.length() ) return false;
		iRet = m_file.write(buf, iLen);
		if( iRet < iLen ) return false;
		iRet = m_file.write("\n", 1);
		if( iRet < 1 ) return false;

        m_file.flush();
		++m_lineno;
	}
	return true;
}

bool fmt_log::generate_filename()
{
	const char *p = m_filenamefmt.c_str();
	std::string strFmt;
	char buf[8], bufSerial[8];
	int serialCount = 0;
	for(; *p != '\0'; p++)  {
		if(*p != '%') {
			sprintf(buf, "%c", *p );
			strFmt += buf;
			continue;
		}
		switch(*++p) {
		case 'Y': case 'y': case 'M': case 'm':
		case 'D': case 'd': case 'H': case 'h':
		case 'N': case 'n': case 'S': case 's':
                        buf[0] = '%';
                        buf[1] = *p;
                        buf[2] = 0;
			break;
		case 'O':
			sprintf(buf, "%d", ++m_serialNo );
	    		break;
	    	default:
	    		while( *p != '\0' && *p >= '0' && *p <= '9')
	    			bufSerial[++serialCount] = *p++;
	    		bufSerial[++serialCount] = 0;
	    		if( *p == 'O' ) {
	    			bufSerial[0] = '%';
	    			bufSerial[serialCount] = 'd';
	    			bufSerial[++serialCount] = '\0';
	    			sprintf(buf, bufSerial, m_serialNo++ );
	    		}
	    		else {
	    			bufSerial[serialCount] = *p;
	    			bufSerial[++serialCount] = '\0';
	    		}
	    	}
	    	strFmt += buf;
	}		    		
//std::cout << "strFmt:" << strFmt << std::endl;

	// 创建文件名
        // 在第一次创建文件名时检查是否有同名的文件的部分(.part(no))存在，找出最大的no号
        namespace fs = archy::filesystem;
	m_createtime = date_time::current();
	std::string filename = m_createtime.format(strFmt.c_str());
        if( m_curname.empty() && m_partno == 0 ) {
        	// 检查对应的文件名后面part(n)最大的
                fs::path logpath(m_filepath, fs::native);
		fs::directory_iterator end_iter;
		std::string strPartName = filename + ".part";
		for ( fs::directory_iterator dir_itr( logpath );
  				dir_itr != end_iter; ++dir_itr ){
                	if ( fs::is_directory( *dir_itr ) ) continue;

                        std::string strfile = dir_itr->leaf();
                        if( ::strncmp(strPartName.c_str(), strfile.c_str(), 
                        		strPartName.length()) != 0 )
                        	continue;
                        
                        std::string strPartNo = strfile.substr(strPartName.length());
                        int partno = atoi(strPartNo.c_str());
                        if( partno > m_partno ) m_partno = partno;
		}
        }

	if( filename == m_curname ) {

		m_curname += ".part" + to_string(++m_partno, 2);

                try {
		        fs::rename(m_filepath + filename, m_filepath + m_curname);
	        }catch( const std::exception & ex){
	                std::cout <<  ex.what() << std::endl;
	        }
		filename += ".part" + to_string(++m_partno, 2);
		m_curname = filename;
	}
        else {
        	if(m_partno > 0 )
                	m_curname = filename + ".part" + to_string(++m_partno, 2);
                else
                        m_curname = filename;
        }
//std::cout << "strFileName:" << m_curname << std::endl;
	return true;
	
}

void fmt_log::refresh()
{
	fmt_log::check(true);
}


// 检查是否需要新生成文件
bool fmt_log::check(bool isTest)
{	
	if( m_lineno == 0 ) {
		if( isTest ) {
			return true;
		}

		fmt_log::generate_filename();
		std::string strFileName = m_filepath + ".temp." + m_curname;
		int ret = m_file.open(strFileName, "a");
		if( ret < 0 )
			return false;
		return true;
	}
	
	// 行数不只有一行
	// 首先检查时间间隔已经到达，然后再检查最大行数是否到达
	if( date_time::current() - m_createtime > m_interval ) {
		string strFileName = m_file.get_filename();
		m_file.close();
		// change file name away from temp log file
		filesystem::rename(strFileName, m_filepath + m_curname);
		m_lineno = 0;
		return this->check(isTest);
	}
	
	if( m_maxline > 0 && m_lineno > m_maxline ) {
		string strFileName = m_file.get_filename();
		m_file.close();
		// change file name away from temp log file
		filesystem::rename(strFileName, m_filepath + m_curname);
		m_lineno = 0;
		return this->check(isTest);
	}
	return true;		
}

} // namespace archy


