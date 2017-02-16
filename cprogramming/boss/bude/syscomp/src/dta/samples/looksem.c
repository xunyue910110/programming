#include "dtacommon.h"
#include "dta.h"

int main(int argc,char *argv[])
{
  char dtapath[100];
  char fullfilename[100];
  int semid;
  union semun argr;
  union semun argw;
  union semun argm;

  if(getenv("DTAPATH")==NULL)
  {
    RETURN_ON_FAIL(DTARC_DTAPATH_ERROR);
  }
  strcpy(dtapath,getenv("DTAPATH"));
  sprintf(fullfilename,"%s/queues/%s.q",dtapath,argv[1]);
  if((semid=semget(ftok(fullfilename,1),3,0666))<0)
  {
    RETURN_ON_FAIL(DTARC_SEMOPEN_ERROR);
  }
  if(argc>2)
  {
    argr.val=0;
    semctl(semid,0,SETVAL,argr);
  }
  if(argc>3)
  {
    argw.val=0;
    semctl(semid,1,SETVAL,argw);
  }
  if(argc>4)
  {
    argm.val=0;
    semctl(semid,2,SETVAL,argm);
  }
  printf("rlock:%d\n",semctl(semid,0,GETVAL));
  printf("wlock:%d\n",semctl(semid,1,GETVAL));
  printf("mlock:%d\n",semctl(semid,2,GETVAL));
  return 0;
}
