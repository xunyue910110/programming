#include "tcp.hpp"
#include "archy_config.hpp"
#include "CAppMain.hpp"
USING_STD_NAMESPACE;
int testtcpconnect();
class CTestMain:public archy::CAppMain
{
protected:
	int Run();
};

CTestMain theApp;
BOOL Connect(UINT TimeoutSec);
int CTestMain::Run()
{
	testtcpconnect();
//	Connect(10);
	return 0;
	
}
int testtcpconnect()
{
	archy::tcp_socket t;
	cout << "start" << endl;

	int iRet = t.connect("192.158.101.11", 343, 10);
	cout << "end:" << iRet << endl;
	cout << "start" << endl;
	iRet = t.connect("192.168.11.30", 21, 3);
	cout << "end:" << iRet << endl;
	return 0;
}
/*
BOOL Connect(UINT TimeoutSec)
{
    SOCKET client;
    ULONG NonBlk = 1;
    int Ret;
    DWORD Err;

    // Open socket
    client = socket(AF_INET, SOCK_STREAM, 0);

    // Set to Non-blocking mode
    ioctlsocket(client, FIONBIO, &NonBlk);

    // Some address and port.
    SOCKADDR_IN  sin;
    DWORD             Addr;
    Addr = inet_addr("192.100.100.102");
    
     sin.sin_family          = AF_INET;
    sin.sin_port          = htons(80);
    sin.sin_addr.s_addr = Addr;

     // This call will return immediately, coz our socket is non-blocking
     Ret = connect(client, (const sockaddr*)&sin, sizeof(sin));

     // If connected, it will return 0, or error
    if (Ret == SOCKET_ERROR)
    {
           Err = WSAGetLastError();
           // Check if the error was WSAEWOULDBLOCK, where we'll wait.
           if (Err == WSAEWOULDBLOCK)
          {
                  printf("\nConnect() returned WSAEWOULDBLOCK. Need to Wait..");
                  fd_set         Write, Err;
                  TIMEVAL      Timeout;

                  FD_ZERO(&Write);
                  FD_ZERO(&Err);
                  FD_SET(client, &Write);
                  FD_SET(client, &Err);

                  Timeout.tv_sec  = TimeoutSec;
                  Timeout.tv_usec = 0; // your timeout

                  Ret = select (0,                // ignored
                                      NULL,           // read,
                                      &Write,        // Write Check
                                      &Err,            // Error check
                                      &Timeout);

                  if (Ret == 0)
                          printf("\nConnect Timeout (%d Sec).", TimeoutSec);
                  else
                  {
                         if (FD_ISSET(client, &Write))
                         {
                                  printf("\nConnected...");
                                  return TRUE;
                         }
                         if (FD_ISSET(client, &Err))
                         {
                                  printf("\nSelect() Error.");
                                  return FALSE;
                         }
                   }
            }
            else
                    printf("\nConnect Error %d", WSAGetLastError());
      }
      else
      {
               printf("\nWoooo!! got connected with NO Waiting!!");
               return TRUE;
       }
       return FALSE;
}
*/
