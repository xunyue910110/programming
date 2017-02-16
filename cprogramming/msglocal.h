#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SEED 'g'
#define SERVER 1L

typedef struct {
long msg_to;
long msg_fm;
char buffer[BUFSIZ];
}MESSAGE;
