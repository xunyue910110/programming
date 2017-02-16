#ifndef _EXPORTERAPPENDER_H_
#define _EXPORTERAPPENDER_H_
#include "log4cpp/Appender.h"
#include "hessian/wrappers.h"
#include <list>
#include <map>


USING_NAMESPACE(std);
using namespace hessian;

namespace log4cpp{
	class ExporterAppender : public Appender{
		public:
			ExporterAppender();
			virtual void init(const map < string, string > & configuration);
			virtual void write(const char * data, size_t size);
			virtual ~ExporterAppender() throw();
		private:
			const static string PROCESS_DIR;
			const static string ALERT_DIR;
			const static string ERROR_DIR;
			
			string m_workdir;
			string m_alertdir;
			string m_processdir;
			string m_errordir;
            string m_localCode;
			
			int m_batchsend;
			int m_alarmsperfile;			     
			
			bool isAlert(const string& type);
			string getIncompletedFile(const string& dn, const string& pid);
			
			void endProcess(const string& dn, const string& pid);
			void persistProcess(const string& dn, const string& pid, const char* data, size_t size);
			void persistAlert(const string& dn, const char* data, size_t size);
			
			void makeDirectory(const string &dir);
			void touchFile(const string& filename);
	};
};
#endif //_EXPORTERAPPENDER_H_
