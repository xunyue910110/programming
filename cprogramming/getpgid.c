#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
void
main(void){
    int i;
    printf("\n\nInitial process \t PID %6d \t PPID %6d \t GID %6d\n\n",
    getpid(), getppid(), getpgid(0));

    for (i = 0; i < 3; i++)
    if (fork() == 0)
        printf("New process \t\t PID %6d \t PPID %6d \t GID %6d\n\n",getpid(), getppid(), getpgid(0));
}
