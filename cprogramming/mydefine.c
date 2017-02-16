#include <errno.h>
 #define writeRunlog(level, msg) \
    writeRunlog(level, "USRMSG:%s,FILE:%s,LINE:%d,ERRMSG:%s\n", msg, __FILE__, __LINE__, strerror(errno))

int main()
{
    writeRunlog(4, "Net_CreateAccept wrong,subSockId < 0");
}
