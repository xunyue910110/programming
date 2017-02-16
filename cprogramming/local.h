#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PUBLIC "/tmp/PUBLIC"
#define B_SIZ (PIPE_BUF/2)

struct message {
char fifo_name[B_SIZ];
char cmd_line[B_SIZ];
};
