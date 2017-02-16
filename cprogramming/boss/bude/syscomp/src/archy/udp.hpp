#ifndef ARCHY_UDP_WRAPED_CLASS_HEADER
#define ARCHY_UDP_WRAPED_CLASS_HEADER
#include "tcp.hpp"

namespace archy {

class udp_socket:public tcp_socket
{
public:
	udp_socket():tcp_socket(){}
	udp_socket(SOCKET iSocket):tcp_socket(iSocket){}

	udp_socket(SOCKET iSocket, struct sockaddr_in addr)
	:tcp_socket(iSocket, addr)
	{}
	
	int connect(const char *strServer, int iPort)
	{ return tcp_socket::connect(strServer, iPort, true); }

	

};

} //namespace archy

#endif

