#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "BarrierStub.h"    
#include "LockStub.h"       
#include "MutexStub.h"      
#include "PrimitiveStub.h"  
#include "QueueStub.h"      
#include "SemaphoreStub.h"  
#include "SessionStub.h"    
#include "SharedMemoryStub.h" 
#include "ClientFactory.h"  

#if !defined(_WIN32)
#define NO_STRICMP 1
#endif


void input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    do {
	printf(prompt);
	*buf = '\0';
	fgets(buf, buf_size, stdin);
	p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
	*--p = '\0';
    }
}

#if defined(NO_STRICMP) 
#include <ctype.h>
inline int stricmp(const char* p, const char* q)
{
    while (toupper(*(unsigned char*)p) == toupper(*(unsigned char*)q)) { 
	if (*p == '\0') { 
	    return 0;
	}
	p += 1;
	q += 1;
    }
    return toupper(*(unsigned char*)p) - toupper(*(unsigned char*)q);
}
#endif

bool executeCommand(Session* session, char* cmd)
{
    char buf[32*1024];
    if (stricmp(cmd, "info") == 0) { 
	session->showServerInfo(buf, sizeof(buf));
	fputs(buf, stdout);
    } else if (stricmp(cmd, "shutdown") == 0) { 
	session->shutdownServer();
    } else if (stricmp(cmd, "id") == 0) { 
	session->getSessionID(buf, sizeof(buf));
	fputs(buf,stdout);
    } else { 
	fprintf(stderr, "Unknown command: '%s'\n", cmd);
	fprintf(stderr, "Available commands: exit, shutdown, info, help\n");
	return false;
    }
    return true;
}

void dialog(Session* session) 
{
    char cmd[1024];
    while (true) {
	input("> ", cmd, sizeof(cmd));
	if (stricmp(cmd, "exit") == 0) { 
	    break;
	} else if (stricmp(cmd, "help") == 0 || stricmp(cmd, "?") == 0) { 
	    printf("Commands: exit, shutdown, info, help\n");
	} else { 
	    executeCommand(session, cmd); 
	}
    }
}


int main(int argc, char* argv[]) 
{ 
    if (argc < 3) { 
	fprintf(stderr, "Usage: ServerMonitor HOSTNAME PORT [info|exit]");
	return 1;
    }
    char* hostname = argv[1];
    int   port = atoi(argv[2]);
    Factory* factory = ClientFactory::getInstance();
    Session* session = factory->create(hostname, port);
    if (argc > 3) { 
	return executeCommand(session, argv[3]);
    } else { 
	dialog(session);
	return 0;
    }
}

