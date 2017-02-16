#ifndef _UDPAPPENDER_H_
#define _UDPAPPENDER_H_
#include "log4cpp/Appender.h"
#include "cppsocket/udpnode.h"

using CPPSocket::UDPNode;

namespace log4cpp{
  
  class MyUDPNode : public UDPNode {
    public:
      bool isOpened() throw() {
        return socket.isOpened();
      }
  };
  
	class UdpAppender : public Appender{
		public:
		  UdpAppender() : m_inited(false) {}
			virtual void init(const map < string, string > & configuration);
			virtual void write(const char * data, size_t size);
			virtual ~UdpAppender() throw();
		private:
			int port;
			string remote_host;			
      string m_localCode;
      bool m_inited;
			MyUDPNode udpnode;
	};
};
#endif //_UDPAPPENDER_H_
