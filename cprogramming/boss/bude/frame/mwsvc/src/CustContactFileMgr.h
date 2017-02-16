#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/dir.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include "string.h"
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

typedef map<string, string, less<string> > strMap;
typedef strMap::iterator strMapIt;
    
class CfgFile
{
public:
    CfgFile( ){open("../etc/CustContactFile.cfg");};
    ~CfgFile( ){};
    bool open(const char* pcfgpath);
    string read(const char*psect, const char*pkey);
protected:
    bool do_open(const char* pcfgpath);
    strMap c_cfgmap;
};

class CustContactFileMgr
{
public:
    int startContact(string eparchyCode, string cityCode, string contactStartTime,
                        string contactMode, string inMode, string channelID, string contacterID,
                        string custID, string contactDesc);

    int appendTrade2Contact(string eparchyCode, string cityCode, string contactTime,
                            string contactMode, string inMode, string channelID,
                            string contacterID, string custID, string contactTradeType,
                            string contactTradeID, string contactDesc);

    int finishContact(string eparchyCode, string cityCode, string contactFinishTime,
                        string channelID, string contacterID, string custID);
                        
    ~CustContactFileMgr();
                        
private:
    string strPath;
    long lTimeOut;
    string strPid;
    long lSizeOut;
    
    string strCurFileName;
    fstream fCustContact;
    long lCustContactCount;
    
    void MakeCustContactFile(string strEparchyCode, string strContent);
} ;

static CustContactFileMgr custContactFileMgr; 