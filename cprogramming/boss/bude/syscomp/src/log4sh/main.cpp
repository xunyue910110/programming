#include "base/config-all.h"
#include "log4cpp/Logger.h"
#include "base/StringUtil.h"
#include <string>
#include <unistd.h>
using namespace log4cpp;
USING_NAMESPACE(std);
const char* example = 
"#info\n" 
"log4sh -f message\n"
"\n"
"#state\n" 
"log4sh -s x1=y1,x2=y2\n"
"\n"
"#alert grade = [1~4]\n"
"log4sh -a grade code message\n"
"\n"
"#pBegin\n"
"log4sh -b  [x1=y1,x2=y2]\n"
"\n"
"#pBeginInput\n"
"log4sh -bi fullname  [x1=y1,x2=y2]\n"
"\n"
"#pEndInput\n"
"log4sh -ei fullname count [x1=y1,x2=y2]\n"
"\n"
"#pBeginOutput\n"
"log4sh -bo fullname  [x1=y1,x2=y2]\n"
"\n"
"#pEndOutput\n"
"log4sh -eo fullname count [x1=y1,x2=y2]\n"
"\n"
"#pInput\n"
"log4sh -i fullname count [x1=y1,x2=y2]\n"
"\n"
"#pOutput\n"
"log4sh -o fullname count [x1=y1,x2=y2]\n"
"\n"
"#pLinkOut\n"
"log4sh -l source target\n"
"\n"
"#pEnd\n"
"log4sh -e  [x1=y1,x2=y2]\n"
"\n"
"example:\n"
"LOG4SH_DN=eboss.cbs.uniacct.xxx.ch0\n"
"\n"
"log4sh -a 1 10001 \"open error\"\n"
"\n"
"#process\n"
"LOG4SH_PID =`log4sh -b`\n"
"log4sh -i file1 1000\n"
"log4sh -o file2 1000\n"
"log4sh -e\n";

#define assertArgs(expr) {if(!(expr)) {fprintf(stderr, "错误的参数, 正确语法:\n%s", example); return -1;}}
#define assertPid() {if(getenv("LOG4SH_PID") == 0) {fprintf(stderr, "环境变量中找不到LOG4SH_PID\n"); return -1;}}
int main(int argc, char* argv[]) {
	const char* dn = getenv("LOG4SH_DN");
	
	if(dn == 0) {
		fprintf(stderr, "环境变量中找不到LOG4SH_DN\n"); 
		return  -1;
	}
	
	assertArgs(argc >= 2);
	
	Logger& logger = Logger::getLogger(dn);	

    if(strcmp(argv[1], "-f") == 0) {	
    	assertArgs(argc == 3);
    	logger.info(argv[2]);
    } else if(strcmp(argv[1], "-s") == 0) {	
    	assertArgs(argc == 3);
    	vector<string> strs;
    	StringUtil::split(argv[2], ",", strs, true);
    	NameValueList nvl;
    	vector<string> nv;
    	for(int i=0; i<strs.size(); i++) {
    	    nv.clear();
    	    StringUtil::split(strs[i], "=", nv, true);
    	    if(nv.size() == 2) {
    	      nvl.push_back(NameValue(nv[0], nv[1]));
    	    }
    	}      	
    	logger.state(nvl);
    } else if(strcmp(argv[1], "-a") == 0) {	
    	assertArgs(argc == 5);
    	logger.alert((Grade::Type)atoi(argv[2]), atoi(argv[3]), argv[4]);
    } else if(strcmp(argv[1], "-ok") == 0) {	
    	assertArgs(argc == 2 || argc == 3);
    	if(argc == 2)
    		logger.ok();
    	else 
    		logger.ok(atoi(argv[2]));    		
    } else if(strcmp(argv[1], "-b") == 0) {
    	assertArgs(argc == 2 || argc == 3);
    	string pid;
    	if(argc == 2)
    		pid = logger.pBegin();
    	else
    		pid = logger.pBegin(argv[2]);
    	cout << pid << endl;
    } else if(strcmp(argv[1], "-bi") == 0) {	
    	assertPid();
    	assertArgs(argc == 3 || argc == 4);
    	if(argc == 3)
    		logger.pBeginInput(getenv("LOG4SH_PID"), argv[2]);
    	else
    		logger.pBeginInput(getenv("LOG4SH_PID"), argv[2], argv[3]);
    } else if(strcmp(argv[1], "-ei") == 0) {	
    	assertPid();					
    	assertArgs(argc == 4 || argc == 5);
    	if(argc == 4)
    		logger.pEndInput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]));
    	else
    		logger.pEndInput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]), argv[4]);
	} else if(strcmp(argv[1], "-bo") == 0) {	
		assertPid();	
		assertArgs(argc == 3 || argc == 4);
    	if(argc == 3)
    		logger.pBeginOutput(getenv("LOG4SH_PID"),argv[2]);
    	else
    		logger.pBeginOutput(getenv("LOG4SH_PID"),argv[2], argv[3]);				
	} else if(strcmp(argv[1], "-eo") == 0) {
		assertPid();	
		assertArgs(argc == 4 || argc == 5);
    	if(argc == 4)
    		logger.pEndOutput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]));
    	else
    		logger.pEndOutput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]), argv[4]);					
	} else if(strcmp(argv[1], "-i") == 0) {	
		assertPid();	
		assertArgs(argc == 4 || argc == 5);
    	if(argc == 4)
    		logger.pInput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]));
    	else
    		logger.pInput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]), argv[4]);				
	} else if(strcmp(argv[1], "-o") == 0) {	
		assertPid();	
		assertArgs(argc == 4 || argc == 5);
    	if(argc == 4)
    		logger.pOutput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]));
    	else
    		logger.pOutput(getenv("LOG4SH_PID"),argv[2], atoi(argv[3]), argv[4]);									
	} else if(strcmp(argv[1], "-l") == 0) {	
		assertPid();	
		assertArgs(argc == 4);
    	logger.pLinkOut(getenv("LOG4SH_PID"),argv[2], argv[3]);
	} else if(strcmp(argv[1], "-e") == 0) {
		assertPid();
		assertArgs(argc == 2 || argc == 3);
    	if(argc == 2)
    		logger.pEnd(getenv("LOG4SH_PID"));
    	else
    		logger.pEnd(getenv("LOG4SH_PID"),argv[2]);
	} else {			
		return -1;
    }	
    return 0;				
}
