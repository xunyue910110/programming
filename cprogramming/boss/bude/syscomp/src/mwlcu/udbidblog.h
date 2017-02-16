#ifndef EBPM_UDBI_DB_ROLLBACK_LOG_H
#define EBPM_UDBI_DB_ROLLBACK_LOG_H

#include "udbi/udbi.h"
#include "lcu/lcu.h"
#include <string>
#include <sys/types.h>
#include <sstream>
#include <map>
#include <vector>

using namespace UDBI;
using namespace std;

//extern JConnect* pcon;

namespace ebpm4c{

class UDBIUndoLogDao;

#ifndef EBPM_SYSTEM_TIME_H
#define EBPM_SYSTEM_TIME_H

typedef struct //tagSystemTime
{
    unsigned short  year;
    unsigned char   month;
    unsigned char   dayOfWeek;
    unsigned char   day;
    unsigned char   hour;
    unsigned char   minute;
    unsigned char   second;
    unsigned int    microsecond;
} SystemTime;

#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif



#endif //EBPM_SYSTEM_TIME_H
/**
* 记录需要回退的流程运行的日志
*/
class UDBIUndoLogDao{
private:
   JConnect* pcon;
public:
   UDBIUndoLogDao(){
        pcon = jdb::getConnect();
   }
   virtual ~UDBIUndoLogDao(){}
   void   writelog(const int64_t& definitionid,string& definitionname,Context& ctx);

   int64_t readlog(vector<string>& nodeidlist ,string flowid,int servidx,int processid, string definitionName);
   bool   isUndoFinished(string flowid,int servidx,int processid,string procname);
   int    getLogServCount(string flowid);
   int    getLogSteps(string flowid,int servidx);
   void   setUndoSucess(string flowid,int servidx,int processid,const int64_t& def_id);

   inline string int64str(const int64_t& in){
          std::basic_stringstream <char> ss;
          string str;
          ss << in; ss >> str;
          return str;
   };

   inline const int64_t str2int64(const string& in){
      std::basic_stringstream <char> ss;
      int64_t r;
      ss << in; ss >> r;
      return r;
   };
   
   void  insertMonitorInfo(string proces,string server,string servidx,string staffid,string depatid,string eparchy_code,string city_code);
   
  
};

}

#endif // EBPM_UDBI_DB_ROLLBACK_LOG_H

