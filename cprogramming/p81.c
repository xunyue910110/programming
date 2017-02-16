/*
      Allocating a shared memory segment
 */
#include <iostream>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;
int
main( ) {
  key_t  key = 15;
  int    shmid_1, shmid_2;
  if ((shmid_1=shmget(key, 1000, 0640|IPC_CREAT)) == -1){
    perror("shmget shmid_1");
    return 1;
  }
  cout << "First shared memory identifier is : " << shmid_1 << endl;
  if ((shmid_2=shmget(IPC_PRIVATE, 20, 0640)) == -1){
    perror("shmget shmid_2");
    return 2;
  }
  cout << "Second shared memory identifier is: " <<  shmid_2 << endl;
  return 0;
}
