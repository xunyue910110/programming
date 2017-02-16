#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void bye1(void)
{
    fprintf(stdout, "bye1...");
}

void bye2(void)
{
    fprintf(stdout, "bye2...");
}

int main()
{

atexit(bye1);
atexit(bye2);
fprintf(stdout,"Hello World.");
//abort();
sleep(100);
exit(0);
}
