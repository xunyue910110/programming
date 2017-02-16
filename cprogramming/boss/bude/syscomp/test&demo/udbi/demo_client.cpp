#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atmi.h"
             
/**    
 * ymclient: $(SRC_PATH)/ym_client.o
 * 
 * ym_client: ymclient
 * 	buildclient \
 * 	-o ym_client \
 * 	-f $(OBJ_PATH)/ym_client.o
 */
 
main(int argc, char *argv[])
{
    int iRet;
    char *rcvbuf, *sendbuf;
    long rcvlen;
                      
	sendbuf = (char *) tpalloc("STRING", NULL, 20+1);
	rcvbuf = (char *) tpalloc("STRING", NULL, 60+1);
	strcpy(sendbuf, "1100000001  服务测试成功，可以收工了");
	
	iRet = tpinit((TPINIT *)NULL);
    if (iRet < 0) {
        printf("tpinit error ... \n");
        return -1;
    }

	//xa support
    /*
    iRet = tpbegin((long)0, (long)0);
    if (iRet < 0) {
    	printf("tpbegin() error ... %d\n", iRet);
    	return -1;
    }
	*/
	
    iRet = tpcall("BOSS_SVC", (char *)sendbuf, 0, (char **)&rcvbuf, &rcvlen, (long)0);
    if (iRet < 0) {
        printf("tpcall error ... %d\n",iRet);
        return -1;
    }
    
    //xa support
    //tpcommit((long)0);
    //tpabort((long)0);

    printf("call seccess, ret msg: %s .\n", (char *)rcvbuf);

    iRet = tpterm();

    return 0;
}

