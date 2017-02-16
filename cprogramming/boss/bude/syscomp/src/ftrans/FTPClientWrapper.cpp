
#include "FTPClientWrapper.h"

#include "ControlInfo.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include <signal.h>
#include <iostream>

#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)0xffffffff
#endif

#define MY_NO_FLAGS       0     /* No special flags specified */
#define RETRY_COUNT       20    /* recv retry count */
#define SLEEP_TIME        1     /* sleep time */
#define LINE_FEED         0x0A  /* Line-feed character */ //对应回车字符\r
#define CARRIAGE_RETURN   0x0D  /* Carriage-return character */ //对应换行字符\n
#define SPACE_CHARACTER   0x20  /* Space character */ //对应空格字符


FTPClientWrapper::FTPClientWrapper() 
{
  /* 默认超时设为60秒 */
  m_timeOut = 60;
  m_maxWaitTime = 60; 
  /* 默认为不支持断点续传 */
  m_isResume = NO;
  /* 获取主机字节序 */
  m_isBigOrder = isBigOrder();
}


//==============================================================================
// FTPClientWrapper::setOption
//
/**
   设置ftp选项
*/
//==============================================================================
int  FTPClientWrapper::setOption(FTPOPTION option, int value){  
  switch(option){
  /*超时设置*/
  case TIMEOUT:
    m_timeOut = value;
    break;
  case MAXWAITTIME:
    m_maxWaitTime = value;
    break;
  /*文件传输数据类型设置*/
  case DATATYPE:
    /* 设置传输数据类型 */
    if ( value == BINARY )
      return sendCommand("TYPE I\r\n"); 
    else if ( value == ASCII )
      return sendCommand("TYPE A\r\n");
    else {
      printf(" DATATYPE does not exist\n");
      return ERROR_FTP_INVALID_DATATYPE;
    }
  /*断点续传设置*/
  case RESUME:
    m_isResume = value;
    break;
  default:
    printf(" option does not exist\n");
    return ERROR_FTP_INVALID_OPTION;
  }
  
  return 0;
}


FTPClientWrapper::~FTPClientWrapper()
{

}


//==============================================================================
// FSystemAgent::setOption
//
/**
   登录到FTP Server
*/
//==============================================================================
int FTPClientWrapper::login(const char *host, const char *port, 
                            const char *user, const char *passwd)
{
  struct hostent          *lpHostEnt;  /* Internet host information structure*/
  struct sockaddr_in      sockAddr;    /* Socket address structure  */
  unsigned int           lFtpAddr;
  
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(atoi(port));

  lFtpAddr = inet_addr(host);
  if (lFtpAddr == INADDR_NONE) {
    if (!(lpHostEnt = gethostbyname(host))) {
      perror("gethostbyname");
      return ERROR_FTP_IPERROR;
    }
    sockAddr.sin_addr = *((struct in_addr *)*lpHostEnt->h_addr_list);
  }
  else
    sockAddr.sin_addr = *((struct in_addr *)&lFtpAddr);

  if ((m_hControlChannel = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket() create ");
    return ERROR_FTP_CONNECT;
  } 
  time_t ttTmp = time(NULL);
  if (connect(m_hControlChannel,reinterpret_cast<struct sockaddr *>(&sockAddr), sizeof(sockAddr)) < 0) 
  {
    close(m_hControlChannel);
    perror("connect()");
    //judge whether FTP error or network error by timeout value.
    if (time(NULL) - ttTmp < 5)
      return ERROR_FTP_CONNECT_TIMEOUT;
    //当IP地址不能连接时的返回
    else
      return ERROR_FTP_CONNECT;
  }
  
  int ret = waitResult(m_hControlChannel);
  if (ret != 220){
    close(m_hControlChannel);
    //by MaXu ,当ftp server端不响应的情况
    return ERROR_FTP_CONNECT;
  }
  
  /* 发送user命令 */
  char buf[256];
  snprintf(buf, sizeof(buf), (char*)"USER %s\r\n",user);
  
  int len = strlen(buf);
  ret = writeData(m_hControlChannel, buf, &len);
  if ( ret < 0 || len != (int)strlen(buf) ){
    close(m_hControlChannel);
    return ERROR_FTP_LOGIN;
  }

  ret = waitResult(m_hControlChannel);
  if (ret != 331){
    close(m_hControlChannel);
    return ERROR_FTP_LOGIN;
  }


  /*发送pass命令*/
  snprintf(buf, sizeof(buf), (char*)"PASS %s\r\n", passwd);
  len = strlen(buf);

  ret = writeData(m_hControlChannel,buf,&len);
  if ( ret < 0 || len != (int)strlen(buf) ) {
    close(m_hControlChannel);
    return ERROR_FTP_LOGIN;
  }
  ret = waitResult(m_hControlChannel);
//  cout << "ret" << ret<< endl;
  if (ret != 230){
    close(m_hControlChannel);
    return ERROR_FTP_LOGIN;
  }
  return 0;
}


//==============================================================================
// FSystemAgent::setOption
//
/**
   create listen socket (let it choose the port) & start the socket listening
   return:   FTP_SUCCEED 成功 ， FTP_FAIL 失败
*/
//==============================================================================
int FTPClientWrapper::createListenSocket()
{
  struct sockaddr_in sockAddr; 
  /* 分配套接口 */
  if ((m_hListenSocket = socket( AF_INET, SOCK_STREAM, 0 )) < 0){
    perror("socket()");
    return -1;
  }

  /* Let the system assign a socket address                */
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(0);           /* htons() is just a reminder.*/
  sockAddr.sin_addr.s_addr = INADDR_ANY;
  
  /* Bind the socket       */
  if (bind(m_hListenSocket, 
           reinterpret_cast<struct sockaddr *>(&sockAddr), sizeof(sockAddr)))
  {
    close(m_hListenSocket);
    perror("bind()");
    return -1;
  }

  int tmpint = fcntl(m_hListenSocket, F_GETFL, 0);
  
  if (fcntl(m_hListenSocket, F_SETFL, tmpint|O_NONBLOCK) < 0){
    close(m_hListenSocket);
    perror("fcntl");
    return -1;
  }
  /* Listen for the FTP server connection */
  if (listen(m_hListenSocket, 3)){
    close(m_hListenSocket);
    perror("listen()");
    return -1;
  }

  /* Ask the server to connect to the port monitored by the listener socket*/
  #if defined(_AIX) || defined(__linux__)
    socklen_t iLength = sizeof(sockAddr);
  #else
    int iLength = sizeof(sockAddr);
  #endif

  /* 发送PORT命令 */
  /* Get port number */
  #ifdef __sun__
  if (getsockname(m_hListenSocket, reinterpret_cast<struct sockaddr *>(&sockAddr),  
                  (socklen_t*)&iLength) < 0) {
  #else
  if (getsockname(m_hListenSocket, reinterpret_cast<struct sockaddr *>(&sockAddr),  
                  &iLength) < 0) {
  #endif
    close(m_hListenSocket);
    printf("getsockname");
    return -1;
  }
  int iPort = sockAddr.sin_port;

  /* Get local ip address  */
  #ifdef __sun__
  if (getsockname(m_hControlChannel, reinterpret_cast<struct sockaddr *>(&sockAddr),  
      (socklen_t*)&iLength) < 0) {
  #else
  if (getsockname(m_hControlChannel, reinterpret_cast<struct sockaddr *>(&sockAddr), 
      &iLength) < 0) {
  #endif
    close(m_hListenSocket);
    printf("getsockname");
    return -1;
  }
  
  char strCmd[128];
  //PORT h1,h2,h3,h4,p1,p2
  //where h1 is the high order 8 bits of the internet host  address.

  if (m_isBigOrder == 1)
    sprintf(strCmd, "PORT %d,%d,%d,%d,%d,%d\r\n",
      (sockAddr.sin_addr.s_addr>>24) & 0x000000ff,
      (sockAddr.sin_addr.s_addr>>16) & 0x000000ff,
      (sockAddr.sin_addr.s_addr>>8) & 0x000000ff,
      (sockAddr.sin_addr.s_addr) & 0x000000ff,
      iPort >> 8,
      iPort & 0xFF);
  else
    sprintf(strCmd, "PORT %d,%d,%d,%d,%d,%d\r\n",
      (sockAddr.sin_addr.s_addr) & 0x000000ff,
      (sockAddr.sin_addr.s_addr>>8) & 0x000000ff,
      (sockAddr.sin_addr.s_addr>>16) & 0x000000ff,
      (sockAddr.sin_addr.s_addr>>24) & 0x000000ff,
      iPort & 0xFF,
      iPort >> 8 );

  int len = strlen(strCmd);
  int ret = writeData(m_hControlChannel,strCmd,&len);
  if ( ret < 0 || len != (int)strlen(strCmd) ){
    close(m_hListenSocket);
    return -1;
  }

  /* here we should get the result: 200 PORT command successful. */
  ret = waitResult(m_hControlChannel);
  if (ret != 200){
    close(m_hListenSocket);
    return -1;
  } 
  
  return 0;
}

/*
 *get a file from remote ftp server
 *return 0 -- get successfully
 *return -1 -- some error !
 *return -2 -- remote file doesn't exist
 */
int FTPClientWrapper::getFile(const char* remote, const char* local)
{
    if((remote == NULL) || (strlen(remote) == 0)) {
        return ERROR_FTP_FILENAME;
    }
  
  if (createListenSocket() < 0) {
    return ERROR_FTP_LISTENSOCKET;
  }
  
  const char* cpLocalFile = (local == NULL ? remote : local);
  char buf[10240];

  if (m_isResume == YES) {
    struct stat filestat;
    if (stat(cpLocalFile, &filestat) < 0){
      filestat.st_size = 0;
    }
    snprintf(buf, sizeof(buf), (char*)"REST %d\r\n", (int)filestat.st_size);
  
    int len = strlen(buf);
    int ret = writeData(m_hControlChannel, buf, &len);
    if ( ret < 0 || len != (int)strlen(buf) ){
      close(m_hListenSocket);
      return ERROR_FTP_RESUME;
    }
    // 如果服务器支持断点续传，那么返回应答应是：
    // 350 Restarting at 0. Send STORE or RETRIEVE to initiate transfer.
    ret = waitResult(m_hControlChannel);
    if ( ret < 0 ) {
      close(m_hListenSocket);
      return ERROR_FTP_RESUME;      
    }
    if ( ret/100 != 3 ) {
      m_isResume = NO;
      printf("FTP server does not support rest command\n");
    } 
  }
  
  snprintf(buf, sizeof(buf), (char*)"RETR %s\r\n", remote);
  int len = strlen(buf);
  int ret = writeData(m_hControlChannel, buf, &len);
  if ( ret < 0 || len != (int)strlen(buf) ){
    close(m_hListenSocket);
    return ERROR_FTP_GETFILE;
  }
  m_226 = true;
________________________________________________________________________________  
  ret = waitResult(m_hControlChannel);
  if (ret != 150){
    close(m_hDataSocket);
    return ERROR_FTP_DATACONN;
  }

  ret = acceptSocket(m_hListenSocket, NULL, NULL);
  close(m_hListenSocket);
  if (ret > 0 ){
    m_hDataSocket = ret;
  }
  else {    
    ret = waitResult(m_hControlChannel);
    return ERROR_FTP_DATACONN;
  }


  
  int fd, flags;
  flags=(m_isResume == YES ? O_CREAT|O_APPEND|O_RDWR : O_CREAT|O_TRUNC|O_RDWR);
  if ((fd = open(cpLocalFile, flags, 0660)) < 0 )
  {
    close( m_hDataSocket) ;
    perror("open file");
________________________________________________________________________________
    return ERROR_FTP_GETDATA;
  }

//  int timer = time(0);
//  int totallen = 0;
//  int tmptime;

  int totallen = 0;
  time_t tmptime,timer = time(0);

  while(1){
    tmptime = time(0);
    if ( tmptime >= timer + m_maxWaitTime ){
      close(m_hDataSocket);
      printf("time out %d seconds when get a remote file\n", m_maxWaitTime);
      close(fd);
________________________________________________________________________________
      return ERROR_FTP_GETDATA;
    }
    len = 10240;
    ret = readData(m_hDataSocket, buf, &len);       
      
    if (ret != 0 )
      break;
    else if ( len == 0 )
      continue;
  
    timer = tmptime;
    if (write( fd, buf, len) < len) {
      perror("write file");
      close(fd);
      close(m_hDataSocket);
________________________________________________________________________________
      return ERROR_FTP_GETDATA;
    }
    totallen += len;
  }
________________________________________________________________________________
  close(fd);  
  close(m_hDataSocket);
  if(m_226) 
    ret = waitResult(m_hControlChannel);
  else
    ret = 226;

  if ( (ret == 226) || (ret == 0)) 
    return 0;
________________________________________________________________________________
  return ERROR_FTP_GETDATA;
}

/*
 *put a file to remote ftp server
 *return 0 -- get successfully
 *return -1 -- some error !
 */
int FTPClientWrapper::putFile(const char* local, const char* remote)
{
  if((local == NULL) || (strlen(local) == 0))
  {
    return ERROR_FTP_FILENAME;
  }
    
  if (createListenSocket() < 0)
    return ERROR_FTP_LISTENSOCKET;
 
  struct stat filestat;

  const char* cpRemoteFile = (remote == NULL ? local : remote);

  char buf[10240];
  sprintf( buf, "STOR %s\r\n", cpRemoteFile);
  int len = strlen(buf);
  int ret = writeData(m_hControlChannel,buf,&len);
  if ( ret < 0 || len != (int)strlen(buf) ) {
    close(m_hListenSocket);
    return ERROR_FTP_GETFILE;
  }

  ret = acceptSocket(m_hListenSocket, NULL, NULL);
  close(m_hListenSocket);
  if (ret > 0 )
    m_hDataSocket = ret;
  else {    
    ret = waitResult(m_hControlChannel);
    return ERROR_FTP_DATACONN;
  }

  ret = waitResult(m_hControlChannel);
  if (ret != 150){
    close(m_hDataSocket);
    return ERROR_FTP_DATACONN;
  }

  int fd;
  if ((fd = open(local, O_RDONLY, 0660)) < 0) {
    close(m_hDataSocket);
    perror("open file");
    return ERROR_FTP_GETDATA;
  }
  if ( fstat(fd, &filestat) < 0) {
    printf("File:%s stat error!\n",local);
    close(m_hDataSocket);
    close(fd);
    return ERROR_FTP_GETDATA;
  } 
  int totallen = filestat.st_size;
  int timer = time(0);
  ret = 0;
  int tmplen,tmptime;
  while(totallen > 0) {
    tmptime = time(0);
    if (tmptime >= timer + m_maxWaitTime ) {
      printf("time out %d seconds in putfile\n", m_maxWaitTime);
      ret = -1;
      break;
    }
    if (totallen < 256)
      len = totallen;
    else
      len = 256;

    ret = read( fd, buf, len);
    if (ret <= 0)
      break;
    else 
      tmplen = len = ret;
  
    ret = writeData(m_hDataSocket, buf, &tmplen);
    if ( ret < 0 || tmplen != len ){
      close(fd);
      close(m_hDataSocket);
      return ERROR_FTP_GETDATA;
    }
  
    timer = tmptime;
    totallen -= len;
  }

  close(fd);  
  close(m_hDataSocket);
  ret = waitResult(m_hControlChannel);
  if ( ret != 226 ) 
    return ERROR_FTP_GETDATA;   
  return 0;   
}

/********************************************************
* function: executeCommand
* purpose:  执行ftp服务器端命令
* return:   FTP_SUCCEED 成功 ，        FTP_FAIL 失败
*           CHDIR, CDUP, DEL, LS, PWD, MKDIR, RMDIR,SYSTYPE
*********************************************************/
int FTPClientWrapper::executeCommand(FTPCMDTYPE cmd, const char* cmdValue, 
                                     char* retValue, int retMaxSize)
{
  char buf[10240];
  switch(cmd) {
    // 改名
    case RENAME: { 
      snprintf(buf, sizeof(buf), (char*)"RNFR %s\r\n", cmdValue );//rename from
      int len = strlen(buf);
      int ret = writeData(m_hControlChannel, buf, &len);
      if ( ret < 0 || len != (int)strlen(buf) ){
        return ERROR_FTP_RENAME;
      }
      ret = waitResult(m_hControlChannel);
      if (ret != 350) {
        return ERROR_FTP_RENAME;
      }
      snprintf(buf, sizeof(buf), (char*)"RNTO %s\r\n", retValue );//rename to
      if ( sendCommand(buf) != 0 )
        return ERROR_FTP_RENAME;
      return 0;
    }
    // 获取远端文件状态
    case STATCMD: {
      snprintf(buf, sizeof(buf), (char*)"STAT %s\r\n", cmdValue);
      int len = strlen(buf);
      int ret = writeData(m_hControlChannel, buf, &len);
      if ( ret < 0 || len != (int)strlen(buf)) {
        return ERROR_FTP_STATFILE;
      }
      ret = waitResult(m_hControlChannel);
      if (ret !=  213) {
        return ERROR_FTP_STATFILE;
      }
      else {
        m_replyBuffer[strlen(m_replyBuffer) - 1 ] = 0;
        char* p = strchr(m_replyBuffer, '\n');
        if ( p == NULL )
          return ERROR_FTP_STATFILE;
        p++;
        char* q = strrchr(m_replyBuffer, '\n');
        if (q == NULL)
          return ERROR_FTP_STATFILE;
        
        *(q+1) = 0;
        if ( retMaxSize == 0 )
          strcpy(retValue, p);
        else
          strncpy(retValue, p, retMaxSize);
        return 0;
      }
    }
    /* 改变服务器端当前路径 */
    case CHDIR:
      snprintf(buf,sizeof(buf),(char*)"CWD %s\r\n", cmdValue);
      return sendCommand(buf);
    // 获取远端文件的大小
    case SIZE:{
      snprintf(buf, sizeof(buf), (char*)"SIZE %s\r\n", cmdValue);
      int len = strlen(buf);
      int ret = writeData(m_hControlChannel, buf, &len);
      if ( ret < 0 || len != (int)strlen(buf) ) {
        return ERROR_FTP_SIZE;
      }
      ret = waitResult(m_hControlChannel);
      if (ret/100 !=  2){
        if ( ret <= 0 )
          return ERROR_FTP_SIZE;
        return ERROR_FTP_FILE;
      }
      else {
        char* p = strchr(m_replyBuffer, ' ');
        if ( p == NULL ) return -1;
        while( *p > '9' || *p < '0') p++;
        if ( retMaxSize == 0 )
          strcpy(retValue, p );    
        else
          strncpy(retValue, p, retMaxSize);  
        return 0;
      }
    }
    /* 改变服务器端路径为当前路径的父目录 */
    case CDUP:
      return sendCommand("CDUP\r\n");
    /* 删除服务器端指定文件 */
    case DEL:
      snprintf(buf,sizeof(buf), (char*)"DELE %s\r\n", cmdValue);
//      return sendCommand(buf);
      if (sendCommand(buf) < 0)
        return ERROR_FTP_DELETE;
      return 0;
    /* 列出服务器端指定目录（默认为当前目录）的内容，并放入指定的文件中 */
    case LS:
    {
      if (createListenSocket() < 0) 
        return ERROR_FTP_LISTENSOCKET;
    
      if (cmdValue == NULL) {
        strcpy(buf, "LIST\r\n");
      }
      else {
        snprintf(buf, sizeof(buf), (char*)"LIST -l %s\r\n", cmdValue);
      }
    
      //2002.10.17 修正新疆不能采集GPRS话单
      #ifdef GPRS_XINJIANG
          strcpy(buf, "NLST\r\n");
      #endif
      printf("buf:%s\n",buf);
      //2002.10.17 修正新疆不能采集GPRS话单
      
      int len = strlen(buf);
      int ret = writeData(m_hControlChannel, buf, &len);
      if ( ret < 0 || len != (int)strlen(buf) ) {
        close(m_hListenSocket);
        return ERROR_FTP_LIST;
      }
    
      ret = acceptSocket(m_hListenSocket, NULL, NULL);
      close(m_hListenSocket);
      if (ret > 0 ){
        m_hDataSocket = ret;
      }
      else {    
        ret = waitResult(m_hControlChannel);
        return ERROR_FTP_DATACONN;
      }
    
      ret = waitResult(m_hControlChannel);
      if (ret != 150){
        close(m_hDataSocket);
        return ERROR_FTP_DATACONN;
      }
      int fd;
      if ((fd = open(retValue, O_CREAT|O_TRUNC|O_RDWR, 0660)) < 0){
        close(m_hDataSocket);
        perror("open file");
        return ERROR_FTP_OPENFILE;
      }
    
      int timer = time(0);
      int totallen = 0;
      int tmptime;
      while(1){
        tmptime = time(0);
        if ( tmptime >= timer + m_maxWaitTime ) {
          close(m_hDataSocket);
          printf("time out %d seconds when get a remote file\n", m_maxWaitTime);
          close(fd);
          return ERROR_FTP_GETFILE_TIMEOUT;
        }
        len = 10240;
        ret = readData(m_hDataSocket, buf, &len);
      
        if ( ret != 0 )
          break;
        else if ( len == 0 )
          continue;
      
        timer = tmptime;
        if (write(fd, buf, len) < len) {
          perror("write file");
          close(fd);
          close(m_hDataSocket);
          return ERROR_FTP_WRITEFILE;
        }
        totallen += len;
      }
    
      close(fd);  
      close(m_hDataSocket);
      ret = waitResult(m_hControlChannel);
      if ( ret == 226 ) return 0 ;
      return ERROR_FTP_LIST;
    }
    /* 显示服务器端当前目录的绝对路径 */
    case PWD:
      if (sendCommand("PWD\r\n"))
        return -1;
      {
        char* p1 = strchr(m_replyBuffer,'\"');
        *strchr(++p1,'\"') = 0;
        if ( retMaxSize == 0 )
          strcpy(retValue, p1);
        else
          strncpy(retValue, p1, retMaxSize);
      }
      return 0;
    /* 在服务器端创建目录 */
    case MKDIR:
      snprintf(buf, sizeof(buf), (char*)"MKD %s\r\n", cmdValue);
      return sendCommand(buf);
    /* 在服务器端删除目录  */
    case RMDIR:
      snprintf(buf, sizeof(buf), (char*)"RMD %s\r\n", cmdValue);
      return sendCommand(buf);
    /* 获得服务器端系统类型 */
    case SYSTYPE:
      if (sendCommand("SYST\r\n"))return -1;
      *strstr(m_replyBuffer,"\r\n") = 0;
      if ( retMaxSize == 0 )
        strcpy(retValue, m_replyBuffer+4);
      else
        strncpy(retValue, m_replyBuffer+4, retMaxSize);
      return 0;
    default:
      printf("valid command\n");
      return ERROR_FTP_INVALID_CMD;
  }       
}

/********************************************************
* function: listFile
* purpose:  文件列表
* return:   0 成功 ，        -1，-2 失败
*           CHDIR, CDUP, DEL, LS, PWD, MKDIR, RMDIR,SYSTYPE
*********************************************************/
int FTPClientWrapper::listFile(const string & dir, vector<string>& files,
                               const string & pattern)
{
  char cmdBuf[10240];
  
  if (createListenSocket() < 0) 
    return ERROR_FTP_LISTENSOCKET;  

  if (  (pattern.length() == 0)
      || (pattern == "*") )
  {
    sprintf(cmdBuf, "LIST\r\n"); 
  }
  else
  {
    sprintf(cmdBuf, "LIST %s\r\n", pattern.c_str()); 
  }

  int len = strlen(cmdBuf);
  int ret = writeData(m_hControlChannel, cmdBuf, &len);
  if ( ret < 0 || len != (int)strlen(cmdBuf) ){
    close(m_hListenSocket);
    return ERROR_FTP_LIST;
  }

  ret = acceptSocket(m_hListenSocket, NULL, NULL);
  close(m_hListenSocket);
  if (ret > 0 ){
    m_hDataSocket = ret;
  }
  else {    
    ret = waitResult(m_hControlChannel);
    return ERROR_FTP_DATACONN;
  }
________________________________________________________________________________
  ret = waitResult(m_hControlChannel);
  if (ret != 150){
    close(m_hDataSocket);
    return ERROR_FTP_DATACONN;
  }
________________________________________________________________________________
  static int nBytesRecv, iRetry, iNew, iOld, iPos1, iPos2;
  char dataBuf[1024];
  string strData, strTemp;
  int fileSize;
  
  files.clear();
  m_fileInfo.clear();
________________________________________________________________________________
  for (iRetry = 0; iRetry < RETRY_COUNT; )
  {
    nBytesRecv = recv(m_hDataSocket, (char *)dataBuf,
                        sizeof(dataBuf), MY_NO_FLAGS);

________________________________________________________________________________

    if (nBytesRecv == -1) {
      if (errno == 0) {
        continue;
      }

      if (errno == EWOULDBLOCK ) {
        nBytesRecv = 1;
        iRetry ++;
        sleep(SLEEP_TIME);  // added by zhoujie 2005/01/18
________________________________________________________________________________
        continue;
      }

      return ERROR_FTP_RECEIVE;
________________________________________________________________________________
    }
    else if (nBytesRecv == 0) {
      break;
    }
________________________________________________________________________________

________________________________________________________________________________
    *(dataBuf + nBytesRecv) = 0;
    strData = strData + dataBuf;
    for (iNew = 0, iOld = 0; ; ) {

      iNew = strData.find_first_of(LINE_FEED, iOld);

      if (iNew == (int)string::npos) {
          strData = strData.substr(iOld);
          break;
      }
      strTemp = strData.substr(iOld, iNew - iOld);
      iOld = iNew + 1;
      if (strTemp[0] != '-') {
          continue;
      }

      for (int i = 0 ; i < 4; i++)
      {
        iPos1 = strTemp.find_first_of(SPACE_CHARACTER);
        strTemp = strTemp.substr(iPos1);
        iPos1 = strTemp.find_first_not_of(SPACE_CHARACTER);
        strTemp = strTemp.substr(iPos1);
      }
      
      iPos1 = strTemp.find_first_of(SPACE_CHARACTER);
      fileSize = atoi(strTemp.substr(0, iPos1).c_str());

      iPos1 = strTemp.find_last_of(SPACE_CHARACTER);
      if (iPos1 != (int)string::npos) {
        strTemp = strTemp.substr(iPos1 + 1);
      }

      iPos2 = strTemp.find_first_of(CARRIAGE_RETURN);
      if (iPos2 != (int)string::npos) {
        strTemp = strTemp.substr(0, iPos2);
      }

      files.push_back(strTemp);
      m_fileInfo.insert(map<string, int>::value_type(strTemp, fileSize));
    }
  }
________________________________________________________________________________
  close(m_hDataSocket);   

  ret = waitResult(m_hControlChannel);
  if ( (ret != 226) &&  (ret != 0)) 
    return ERROR_FTP_RECEIVE;

  return 0;

}

/********************************************************
* function: sendCommand
* purpose:  发送FTP 命令到FTP Server
* return:   FTP_SUCCEED 成功 ，        FTP_FAIL 失败
*
*********************************************************/
int FTPClientWrapper::sendCommand(const char* command)
{
  int len = strlen(command);
  int ret = writeData(m_hControlChannel, (char*)command, &len);
  if ( ret < 0 || len != (int)strlen(command) ){
    return -1;
  }

  ret = waitResult(m_hControlChannel);
  
  if (ret/100 != 2){
    return -1;
  }
  return 0; 
}

/********************************************************
* function: logout
* purpose:  退出登陆
* return:   无
*
*********************************************************/
void FTPClientWrapper::logout() const
{
  
  close(m_hControlChannel); 

}

/*******************************************************
 *
 *      描述：主机字节序测试
 *      返回：1-高端字节序，0-低端字节序
 *
 ********************************************************/
int FTPClientWrapper::isBigOrder() const
{
  union{
    short s;
    char c[sizeof(short)];
  }un;
  
  un.s = 0x0102;
  /*printf("%s:",CPU_VENDOR_OS);*/
  if (sizeof(short) >= 2)    {
    if ( un.c[0] == 1 && un.c[1] == 2) {
      //printf("big-endian\n");
      return 1;
    }
    else if (un.c[0] == 2 && un.c[1] == 1) {
      //printf("little-endian\n");
      return 0;
    }
    else {
      //printf("unknown\n");
      return -1;
    }
  }
  else {
    printf("sizeof(short) = %d\n", sizeof(short));
    return -1;
  }
}

/* return: -1:写失败， 0 写成功 */
int FTPClientWrapper::writeData(int fd, char* buff, int *len) const
{
  fd_set writefds;
  fd_set errfds;
  struct timeval tv;
  int ret;
  
  FD_ZERO(&writefds);
  FD_SET(fd,&writefds);
  FD_ZERO(&errfds);
  FD_SET(fd,&errfds);
  tv.tv_sec = m_timeOut;
  tv.tv_usec = 0;
  if ( select(fd+1, NULL, &writefds, &errfds, &tv) ) {

    if (FD_ISSET(fd, &errfds)) {
      *len = 0;
      return -1;
    }
    else if (FD_ISSET(fd, &writefds)) {
      //当 FTP Server 异常中断时忽略 SIGPIPE 信号，避免异常终止
      signal(SIGPIPE, SIG_IGN);
      
      ret = write(fd, buff, *len);
      *len = 0;
      if ( ret > 0 ) {
        *len = ret;
        return 0;
      }
      else {
        return -1;
      }
    }
  }
  return -1;
}

/*return : -1,失败，0，成功或者超时，1，EOF */
int FTPClientWrapper::readData(int fd, char* buff, int *len) const
{
  fd_set readfds;
  fd_set errfds;
  struct timeval tv;
  int ret;
  
  FD_ZERO(&readfds);
  FD_SET(fd,&readfds);
  FD_ZERO(&errfds);
  FD_SET(fd,&errfds);
  tv.tv_sec = m_timeOut;
  tv.tv_usec = 0;
  if ( select(fd+1, &readfds, NULL, &errfds, &tv) ) {
    if (FD_ISSET(fd, &errfds)) {
      *len = 0;
________________________________________________________________________________
      return -1;
    }
    if (FD_ISSET(fd, &readfds)) {
      ret = read(fd, buff, *len);
      *len = 0;
      if ( ret == 0)
        return  1;
      if ( ret < 0) {
________________________________________________________________________________
        return  -1;
      }
      
      *len = ret;
      return 0;     
    } 
  }
  *len = 0;
  return 0;
}

/* 等待连接，返回：0--超时，-1，错误，连接套接字 */
int FTPClientWrapper::acceptSocket(int fd,struct sockaddr* addr,int* len) const
{
  fd_set readfds;
  fd_set errfds;
  struct timeval tv;
  
  FD_ZERO(&readfds);
  FD_SET(fd,&readfds);
  FD_ZERO(&errfds);
  FD_SET(fd,&errfds);
  tv.tv_sec = m_timeOut;
  tv.tv_usec = 0;

  if (select(fd+1, &readfds, NULL, &errfds, &tv)) {
    if (FD_ISSET(fd,&errfds)) {
      return -1;
    }else if (FD_ISSET(fd,&readfds)) {
      #if defined(_AIX) || defined(__linux__) || defined(__sun__)
        return accept(fd, addr, (socklen_t *)len);
      #else
        return accept(fd, addr, len);
      #endif
    } else{
      return 0;
    }
  }
  
  return 0;
}

/* return -1:失败， 0，超时， 其他，返回应答数字*/
int FTPClientWrapper::waitResult(int fd)
{
  int tmptime;
  int ret;
  memset(m_replyBuffer, 0, sizeof(m_replyBuffer));
  int totallen = 0;
  int len = 0;
  int timer = time(0);
  
  while(1) {
    tmptime = time(0);
    if (tmptime >= timer + m_maxWaitTime){
      return 0;
    }
    len = sizeof(m_replyBuffer) - totallen;
    ret = readData(fd, &m_replyBuffer[totallen], &len);
    if (ret != 0){
________________________________________________________________________________
      return -1;
    }
    else if ( len == 0 )
      continue;
    totallen += len;
#ifdef _DEBUG_ 
cout << ":" << m_replyBuffer << endl;
#endif
//________________________________________________________________________________
    /* 这里返回多行仔细考虑，多行时，第一行第四个字符是减号 */
    if ((m_replyBuffer[totallen-2] == '\r' && m_replyBuffer[totallen-1] == '\n')
        ||m_replyBuffer[totallen-1] == 0) {
      int retcode = atoi(  m_replyBuffer );
      if ( strlen( m_replyBuffer) < 4  || m_replyBuffer[3] != '-' ) {
        if ((retcode == 150)&&m_replyBuffer[totallen-24]=='2'
           &&m_replyBuffer[totallen-23]=='2'&&m_replyBuffer[totallen-22]=='6')
          m_226 = false;
        return atoi(m_replyBuffer);
      }
      
      char* p = m_replyBuffer;
      char* q;
      while(getLine(p, &q) == 0 ) {
        if ( strlen(q)>3 && *(q+3) == ' ' && atoi(q) == retcode ) {
          return retcode;
        }
        p = q;
      }         
    }
  }
}

int FTPClientWrapper::getLine( char* src, char** nextline) const
{
  char* p = src;
  for( ; *p != 0 ; p++ ) {
    if ( *p == '\n' && *(p+1) != 0 ) {
      *nextline = p + 1;
      return 0;
    }
  }
  return -1;
}

void FTPClientWrapper::getErrorText(const int code, string & msg) const
{
    switch(code) {
      case ERROR_FTP_CONNECT:
        msg = "socket connect failed";
        break;
      case ERROR_FTP_CONNECT_TIMEOUT:
        msg = "socket connect time out";
        break;
      case ERROR_FTP_LOGIN:
        msg = "login failed";
        break;
      case ERROR_FTP_LISTENSOCKET:
        msg = "create listen socket failed";
        break;
      case ERROR_FTP_DATACONN:
        msg = "data connect failed";
        break;
      case ERROR_FTP_FILE:
        msg = "file error";
        break;
      case ERROR_FTP_FILENAME:
        msg = "invalid filename";
        break;
      case ERROR_FTP_GETFILE:
        msg = "get file failed";
        break;
      case ERROR_FTP_RECEIVE:
        msg = "receive data failed";
        break;
      case ERROR_FTP_GETDATA:
        msg = "get data failed";
        break;
      case ERROR_FTP_RESUME:
        msg = "resume failed";
        break;
      case ERROR_FTP_DELETE:
        msg = "delete file failed";
        break;
      case ERROR_FTP_RENAME:
        msg = "rename file failed";
        break;
      case ERROR_FTP_LIST:
        msg = "list file failed";
        break;
      case ERROR_FTP_IPERROR:
        msg = "ftp ip error";
        break;
      case ERROR_FTP_INVALID_DATATYPE:
        msg = "invalid data type";
        break;      
      case ERROR_FTP_INVALID_OPTION:
        msg = "invalid option";
        break;      
      case ERROR_FTP_STATFILE:
        msg = "stat file failed";
        break;                      
      case ERROR_FTP_SIZE:
        msg = "get file size failed";
        break;      
      case ERROR_FTP_OPENFILE:
        msg = "open file failed";
        break;     
      case ERROR_FTP_GETFILE_TIMEOUT:
        msg = "get file time out";
        break;             
      case ERROR_FTP_WRITEFILE:
        msg = "write file failed";
        break;     
      case ERROR_FTP_INVALID_CMD:
        msg = "invalid command";
        break;    
      default:
        msg = "unknown error";
    }
}
