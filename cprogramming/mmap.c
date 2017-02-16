#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
struct emp{
char name[20];
int id;
double salary;
};

int main()
{
    int fd = open("em.dat", O_CREAT|O_RDWR|O_TRUNC, 0666);
    if ( fd == -1)
    {
        perror("open"); return -1;
    }
    ftruncate(fd, sizeof(struct emp)*3);
    void *p = mmap(NULL, sizeof(struct emp)*3, PROT_WRITE|PROT_READ, 
                   MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    struct emp*pe = p;
    pe[0].id = 100;
    strcpy(pe[0].name, "zhangfei");
    pe[0].salary = 30000.0;

    pe[1].id = 200;
    strcpy(pe[1].name, "guanyu");
    pe[1].salary = 24000.0;
    
    pe[2].id = 300;
    strcpy(pe[2].name, "liubei");
    pe[2].salary = 10000.0;
    munmap(p, 4); 
}
