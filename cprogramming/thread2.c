/*
 *  ============================================================================
 *   Name        : thread.c
 *    Author      : zhujy
 *     Version     :
 *      Copyright   : Your copyright notice
 *       Description : Hello World in C, Ansi-style
 *        ============================================================================
 *         */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//初始化了一个MUTEX锁

int count = 0;

void *func1(void *arg)
{

    int *a = (int *) arg;
    printf("thread%d start\n", *a);
    int i;
    for (i = 0; i < 10; i++)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);//给mutex加锁,这是一条原子操作，不可能出现两个线程同时执行这个代码
        printf("thread%d is running\n", *a);
        count++;//这段代码受到保护，永远只有一个线程可以操作
        printf("%d\n", count);
        pthread_mutex_unlock(&mutex);//给mutex解锁
    }
    printf("thread%d end\n", *a);
    *(int*)arg = count;

    pthread_exit((void*)count);
}

int main(int arg, char * args[])
{
    printf("process start\n");
    pthread_t thr_d1, thr_d2;
    int i[2];
    i[0] = 1;
    i[1] = 2;


    pthread_create(&thr_d1, NULL, func1, &i[0]);
    pthread_create(&thr_d2, NULL, func1, &i[1]);
    int count1;
    int count2;
    pthread_join(thr_d1, (void**)&count1);
    pthread_join(thr_d2, (void**)&count2);

    printf("process end\n");
    printf("i[0]=%d, i[1]=%d\n", i[0], i[1]);
    printf("count1=%d, count2=%d\n", count1, count2);
    return 0;
}

