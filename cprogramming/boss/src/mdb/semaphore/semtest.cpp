#include "SysSemaphore.h"
#include "DoubleLock.h"
#include "IpcDoubleLock.h"
#include "Mdb_Exception.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  
  int opType=0;
  if(argc>=2)
  {
    opType=atoi(argv[1]);
  }
  
    int rsrctype = 1;
    char *rsrcid = "td_user";
    DoubleLock *m_dblock1,*m_dblock2,*m_dblock3;
    const char *dbname = "testmdb";
    int semid = 0;

    m_dblock1 = new IpcDoubleLock(dbname);
    m_dblock2 = new IpcDoubleLock(dbname);
    m_dblock3 = new IpcDoubleLock(dbname);
    cout<<"open.... "<<endl;flush(cout);
    m_dblock1->open(rsrctype, rsrcid);
    m_dblock2->open(rsrctype, rsrcid);
    m_dblock3->open(rsrctype, rsrcid);
  if(opType==0)
  {
    cout<<"m_dblock1->getReadLock begin...."<<endl;flush(cout);
    m_dblock1->getReadLock();
    cout<<"m_dblock2->getWriteLock begin...."<<endl;flush(cout);
    m_dblock2->getWriteLock();
    cout<<"m_dblock3->getReadLock begin...."<<endl;flush(cout);
    m_dblock3->getReadLock();
    cout<<"getRead finished!"<<endl;flush(cout);
    m_dblock1->releaseReadLock();
    m_dblock2->releaseWriteLock();
    m_dblock3->releaseReadLock();
  }else
  {
    cout<<"m_dblock1->getReadLock begin...."<<endl;flush(cout);
    m_dblock1->getReadLock();
    cout<<"m_dblock3->getReadLock begin...."<<endl;flush(cout);
    m_dblock3->getReadLock();
    cout<<"m_dblock2->getWriteLock begin...."<<endl;flush(cout);
    m_dblock1->releaseReadLock();
    m_dblock3->releaseReadLock();
    m_dblock2->getWriteLock();
    cout<<"getRead finished!"<<endl;flush(cout);
    m_dblock2->releaseWriteLock();
  }    
    delete m_dblock1;
    delete m_dblock2;
    delete m_dblock3;
    exit(0);
}

