#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>
#include <utmp.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFFER_SIZE									4096
#define MAXPASS										128
#define AUTH_VECTOR_LEN								16

#define LINK_RADIUS_SUCCESS							0
#define LINK_RADIUS_CONFIG_FILE_OPEN_FAILD			1
#define LINK_RADIUS_AUTHINFO_UNAVAIL				2
#define LINK_RADIUS_AUTH_ERR						3
#define LINK_RADIUS_SERVER_FAILED_RESPOND			4
#define LINK_ERROR_WAITING_FOR_RESPONSE				5
#define LINK_RADIUS_RECV_PACKET_ERROR				6
#define LINK_RADIUS_PACKET_CORRUPTED				7
#define LINK_RADIUS_PACKET_FALIED_VERIFY			8
#define LINK_RADIUS_PACKET_ID_NOT_MATCH				9
#define LINK_RADIUS_SEND_ERROR						10
#define	LINK_RADIUS_CREATE_SOCKET_ERROR				11
#define LINK_RADIUS_BIND_ERROR						12
#define LINK_RADIUS_NO_SERVER_INFO					13
#define LINK_RADIUS_CHALLENGE						111


typedef struct radius_server_t 
{
  struct radius_server_t *next;
  struct in_addr ip;
  u_short port;
  char *hostname;
  char *secret;
  int timeout;
  int tries;
} radius_server_t;

typedef struct radius_conf_t 
{
  radius_server_t *server;
  int sockfd;
} radius_conf_t;


//extern "C" int init(char *filepath,radius_conf_t *conf);


//extern "C" int UserAuth(radius_conf_t *conf,char *username,
//					char *password,char *callstaID, char *a_state);

//extern "C" int  Second_Auth(radius_conf_t *conf, char *username,
//					 char *szNetDynamicPasswd, char *callstaID, char *a_state);
					   
					   
