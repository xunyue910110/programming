// ############################################
// Source file : SysSemaphore.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef SYSSEMAPHORE_H_HEADER_INCLUDED_B7E91B27
#define SYSSEMAPHORE_H_HEADER_INCLUDED_B7E91B27

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include "Mdb_Exception.h"

//##ModelId=481027F403CC
class SysSemaphore
{
  public:
    //SysSemaphore(){} //必须带参数生成对象
    //##ModelId=481027F403CD
    SysSemaphore(char *fileName, int resrcid, int nsems_in);
    
    //void SysSemaphore::setval(char *pname, char *resrcid, int resrctype, int nsems_in)
    //// cmd参数命令：
    //// IPC_STAT   对此集合取semid_ds结构，并存放在由arg.buf指向的结构中。
    //// IPC_SET    按由arg.buf指向的结构中的值设置与此集合相关结构中的下列三个字段值：
    ////                sem_perm.uid,sem_perm.gid和sem_perm.mode。此命令只能由下列两种进程执行：一种是其有
    ////                效用户ID等于sem_perm.cuid或sem_perm.uid的进程;另一种是具有超级用户特权的进程。
    //// IPC_RMID   从系统中删除该信号量集合。这种删除是立即的。仍在使用此信号量的
    ////                其他进程在它们下次意图对此信号量进行操作时，将出错返回EIDRM。此命令只能由下列两
    ////                种进程执行：一种是具有效用户ID等于sem_perm.cuid或sem_perm.uid的进程；另一种是具有超
    ////                级用户特权的进程。
    //// GETVAL     返回成员semnum的semval值。semval：信号灯的当前值。
    //// SETVAL     设置成员semnum的semval值。该值由arg.val指定。
    //// GETPID     返回成员semnum的sempid值。
    //// GETNCNT    返回成员semnum的semncnt值。semncnt：等待semval变为大于其当前值的线程数。
    //// GETZCNT    返回成员semnum的semzcnt值。semzcnt：等待semval变为0的线程数。
    //// GETALL     取该集合中所有信号量的值，并将它们存放在由arg.array指向的数组中。
    //// SETALL     按arg.array指向的数组中的值设置该集合中所有信号量的值。

    ////   操作允许权        八进制整数
    ////    用户可读           0400
    ////    用户可写           0200
    ////    同组可读           0040
    ////    同组可写           0020
    ////    其它可读           0004
    ////    其它可写           0002

    //// 操作权可相加而派生，如用户可"读"、"写"的权限为0400|0200=0600。
    //// 控制命令可取IPC_CREAT或IPC_EXCL。

    //##ModelId=481027F403D1
    ~SysSemaphore();

    //##ModelId=481027F403D2
    // 创建信号灯
    int sem_create(int value[], int n);

    //##ModelId=481027F403D3
    // 打开信号灯
    int sem_open();

    //##ModelId=481027F403D4
    // 销毁信号灯
    bool sem_destroy();

    //##ModelId=481027F403D5
    // 信号量值减少1，等待一个信号灯，上锁
    bool sem_wait(int sem_num,int value=1);

    //##ModelId=481027F403D7
    // 信号量值增加1，挂出一个信号灯，解锁
    bool sem_post(int sem_num,int value=1);

    //##ModelId=481027F403D9
    // 信号量值减少1，等待一个信号灯，上锁（不等待）
    bool sem_trywait(int sem_num,int value=1);

    //##ModelId=482155240290
    // 获取信号灯的值
    int  sem_getvalue(int sem_num);
    
    //获取等待信号灯值增加的进程数
    int  sem_getwcnt(int sem_num);
  private:
    //##ModelId=481027F403DB
    // 用于确定信号量集中信号量个数
    int m_nsems;

    //##ModelId=481027F403DC
    // 信号量集标志符
    int m_semID;

    //##ModelId=481027F403DF
    // IPC关键字
    key_t m_pkey;

    int m_resrcId;


};



#endif /* SYSSEMAPHORE_H_HEADER_INCLUDED_B7E91B27 */
