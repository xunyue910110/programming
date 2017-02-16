#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void errExitEN(int errcode, const char* errmsg)
{
    errno = errcode;
    perror(errmsg);
    exit(-1);
}

void errExit(const char* errmsg)
{
    perror (errmsg);
    exit(-1);
}
