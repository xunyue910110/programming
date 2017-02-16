#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
typedef  struct emp{
char name[256];
int age;
double salary;
} EMP;

int writetxt()
{
    char name[256] = "张飞";
    int age = 30;
    double salary = 20000.5;
    EMP emp = {"赵云", 20, 10000.5};
    char text[10];
    sprintf(text, "%s %d %g\n%s %d %g",
        name, age, salary,
        emp.name, emp.age, emp.salary);
    int fd = open("txt.txt", O_WRONLY|
        O_CREAT|O_TRUNC, 0666);
    if ( fd == -1)
    {
        perror("open");
        exit(-1);
    }
    if ( -1 == write(fd, text, strlen(text)))
    {
        perror("write");
        exit(-1);
    }
    close(fd);
    return 0;
}

int readtxt()
{
    char name[256];
    int age;
    double salary;    
    EMP emp;
    int fd = open("txt.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("open"); 
        exit(-1);
    }
    char text[1024]= {};
    if ( -1 == read(fd, text, 1024) )
    {
        perror("read");
        exit(-1);
    }
    close(fd);
    sscanf(text, "%s%d%lf\n%s%d%lf", name, &age, &salary,
        emp.name, &emp.age, &emp.salary);
    printf("%s %d %g\n%s %d %g", name, age, salary, 
        emp.name, emp.age, emp.salary);
    return 0;
}
int main()
{
    writetxt();
    readtxt();
}

