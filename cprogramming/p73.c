/*
     Using the semctl system call
 */
#include <iostream>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#define  NS    3                           
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
                                   // definition in <sys/sem.h>
#else
union semun {                      // We define:
  int val;                         // value  for SETVAL
  struct semid_ds *buf;            // buffer for IPC_STAT, IPC_SET
  unsigned short int *array;       // array  for GETALL, SETALL
  struct seminfo *__buf;           // buffer for IPC_INFO
};
#endif
using namespace std;
int
main( ){
  int             sem_id, sem_value, i;
  key_t           ipc_key;
  struct semid_ds sem_buf;
  unsigned short int  sem_array[NS] = {3, 1, 4};
  union semun     arg;
  ipc_key = ftok(".", 'S');

  if ((sem_id = semget(ipc_key, NS, IPC_CREAT | 0660)) == -1) {
    perror("semget: IPC_CREAT | 0660");
    return 1;
  }
  cout << "Semaphore identifier " << sem_id << endl;
  arg.buf = &sem_buf;                        
  if (semctl(sem_id, 0, IPC_STAT, arg) == -1) {
    perror("semctl: IPC_STAT");
    return 2;
  }
  cout << "Created " <<  ctime(&sem_buf.sem_ctime) << endl;
  arg.array = sem_array;                    
  if (semctl(sem_id, 0, SETALL, arg) == -1) {
    perror("semctl: SETALL");
    return 3;
  }
  for (i = 0; i < NS; ++i) {
    if ((sem_value = semctl(sem_id, i, GETVAL, 0)) == -1) {
      perror("semctl: GETVAL");
      return 4;
    }
    cout << "Semaphore " << i << " has value of " << sem_value << endl;
  }
  if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
    perror("semctl: IPC_RMID");
    return 5;
  }
  return 0;
}
