#include "mt.h"

int CMsgQueue::init()
{
  return DTA_INIT();
}

int CMsgQueue::end()
{
  return DTA_EXIT();
}

int CMsgQueue::open(const char *queuename)
{
  return DTA_OPENQ(&hqueue,queuename);
}

int CMsgQueue::close()
{
  return DTA_CLOSEQ(hqueue);
}

int CMsgQueue::put(const char *message, long msglen, long priority)
{
  return DTA_PUTMSG(hqueue,message,msglen,priority);
}

int CMsgQueue::get(char *message, long &msglen, long &priority, int mode)
{
  return DTA_GETMSG(&hqueue,message,&msglen,&priority,mode);
}

int CMsgQueue::ask(long &maxmsgs,long &livmsgs)
{
  return DTA_ASKQ(hqueue,&maxmsgs,&livmsgs);
}
