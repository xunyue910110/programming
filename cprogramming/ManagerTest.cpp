#include <iostream>
using namespace std;

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "Manager.h"

pthread_t pths[3];
void *start_routine(void* arg)
{
    //pthread_detach(pthread_self());
    Manager::GetInstance().Print();
}

void TestMultiThreads()
{
    for (int i = 0; i < 3; i++)
    {
        if ( 0 != pthread_create(&pths[i],NULL, start_routine, NULL))
            perror("pthread_create"),exit(0);
    }
}
int main()
{
    TestMultiThreads();
    for (int i = 0; i < 3; i++)
        pthread_join(pths[i], NULL);
    
    //MeyersSingleton::ReleaseInstance();
    //sleep(10);
    
    return 0;
}
