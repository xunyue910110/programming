/*
      Displaying message queue status information
 */
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;
int
main( ){
  int             mid;
  key_t           key;
  struct msqid_ds buf;                         

  key = ftok(".", 'z');
  if ((mid = msgget(key, IPC_CREAT | 0660)) == -1) {   
    perror("Queue create");
    return 1;
  }
  msgctl(mid, IPC_STAT, &buf);
  cout << "Message Queue *Permission* Structure Information" << endl;
  cout << "Owner's user ID   \t" << buf.msg_perm.uid  << endl;
  cout << "Owner's group ID  \t" << buf.msg_perm.gid  << endl;
  cout << "Creator's user ID \t" << buf.msg_perm.cuid << endl;
  cout << "Creator's group ID\t" << buf.msg_perm.cgid << endl;
  cout << "Access mode in HEX\t" << hex <<  buf.msg_perm.mode << endl;
  cout << "\nAdditional Selected Message Queue Structure Information\n";
  cout << "Current # of bytes on queue  \t" << dec
                                           << buf.__msg_cbytes << endl;
  cout << "Current # of messages on queue\t" << buf.msg_qnum   << endl;
  cout << "Maximum # of bytes on queue   \t" << buf.msg_qbytes << endl;
  msgctl(mid, IPC_RMID, (struct msqid_ds *) 0 );
  return 0;
}
