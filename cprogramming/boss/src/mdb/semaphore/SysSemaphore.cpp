// ############################################
// Source file : SysSemaphore.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "SysSemaphore.h"
#include <iostream>
//##ModelId=481027F403CD
SysSemaphore::SysSemaphore(char *fileName, int resrcid, int nsems_in)
{
    m_semID = 0;
    m_nsems = nsems_in;
    m_resrcId = resrcid;
    m_pkey = ftok(fileName, resrcid);
    if(m_pkey < 0)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:create m_pkey error !!! ");
    }
}

//##ModelId=481027F403D1
SysSemaphore::~SysSemaphore()
{
}

//##ModelId=481027F403D2
int SysSemaphore::sem_create(int value[], int n)
{
    int ret,i;
    union semun {
        int val;
        struct m_semID_ds *buf;
        ushort *array;
    } arg;
    if(m_pkey < 0 )
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_create():m_pkey equal '-1' or has been changed !");
    }

    m_semID = semget(m_pkey, m_nsems, IPC_CREAT|IPC_EXCL|0600);         // 创建可读写的信号灯
    if(m_semID < 0)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_create():sem_create->semget:get m_semID error (m_semID < 0)!!!");
    }
    if (m_nsems != n)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_create():m_nsems != n!!!");
    }
    for(i=0;i<m_nsems;i++)
    {
        arg.val = value[i];
        ret = semctl(m_semID, i, SETVAL, arg);
        if(ret == -1)
        {
            semctl(m_semID, 0, IPC_RMID);
            throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_create():sem_create->semctl error");
        }
    }
    return m_semID;
}

//##ModelId=481027F403D3
int SysSemaphore::sem_open()
{
    if(m_pkey < 0 )
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_open(): m_pkey equal '-1' or has been changed !");
    }
    m_semID = semget(m_pkey, 0, 0);
    return m_semID;
}

//##ModelId=481027F403D4
bool SysSemaphore::sem_destroy()
{
    int ret;

    if(m_pkey < 0 )
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_destroy():m_pkey equal '-1' or has been changed !");
    }
    ret = semctl(m_semID, 0, IPC_RMID);
    if (ret == -1)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_destroy():sem_destroy->semctl error");
    }
    return true;
}

//##ModelId=481027F403D5
bool SysSemaphore::sem_wait(int sem_num,int value)
{
    struct sembuf sb;
    int ret;

    sb.sem_num = sem_num;
    sb.sem_op  = -1*value;
    sb.sem_flg = SEM_UNDO;

    ret = semop(m_semID, &sb, 1);
    if(ret == -1)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_wait():sem_wait->semop error !!");
    }
    return true;
}

//##ModelId=481027F403D7
bool SysSemaphore::sem_post(int sem_num,int value)
{
    struct sembuf sb;
    int ret;

    sb.sem_num = sem_num;
    sb.sem_op  = value;
    sb.sem_flg = SEM_UNDO;
    
    ret = semop(m_semID, &sb, 1);
    if(ret == -1)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_post():sem_post->semop error !!");
    }
    return true;
}

//##ModelId=481027F403D9
bool SysSemaphore::sem_trywait(int sem_num,int value)
{
    struct sembuf sb;
    int ret;

    sb.sem_num = sem_num;
    sb.sem_op  = -1*value;
    sb.sem_flg = IPC_NOWAIT;

    ret = semop(m_semID, &sb, 1);
    if(ret != 0)
    {
        throw Mdb_Exception(__FILE__,__LINE__,"In SysSemaphore:sem_trywait():sem_trywait->semop error !!");
    }
    return true;
}

//##ModelId=482155240290
int SysSemaphore::sem_getvalue(int sem_num)
{
	int ret;
	ret = semctl(m_semID, sem_num, GETVAL);
	if(ret == -1) 
		throw Mdb_Exception(__FILE__,__LINE__,"In Semaphone:sem_getvalue():sem_getvalue->semctl error !!");
	return ret;
}
//获取等待信号灯值增加的进程数
int  SysSemaphore::sem_getwcnt(int sem_num)
{
  
	int ret;
	ret = semctl(m_semID, sem_num, GETNCNT);
	if(ret == -1) 
		throw Mdb_Exception(__FILE__,__LINE__,"In Semaphone:sem_getwcnt():sem_getvalue->semctl error !!");
	return ret;
  
}
