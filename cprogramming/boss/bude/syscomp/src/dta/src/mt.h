#ifndef MT_H
#define MT_H

#include "dta.h"

class CMsgQueue
{
 public:
  int init();
  int end();
  int open(const char *queuename);
  int close();
  int put(const char *message, long msglen, long priority);
  int get(char *message, long &msglen, long &priority, int mode);
  int ask(long &maxmsgs,long &livmsgs);

  //reserved
  //int emptyq();
  //int cleanq();
  //int delmsg(long msgn);
  //int sendmsg(const char *remotequeue, const char *message, long msglen, long priority);
 protected:
  DTAHQUEUE hqueue;
};

#endif  // MT_H
