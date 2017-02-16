#include "log4cpp/UdpAppender.h"
#include "cppsocket/stringbuffer.h"
#include <memory>
#include "hessian/hessian_output.h"
#include "hessian/hessian_input.h"
#include "hessian/string_input_stream.h"
#include "hessian/dump.h"
#include <iostream>


USING_NAMESPACE(std);
using namespace CPPSocket;
using namespace hessian;
using namespace log4cpp;

void UdpAppender::init(const map < string, string > & configuration) {    
    map < string, string >::const_iterator it1 = configuration.find("localcode");
    if(it1!=configuration.end()) {
        m_localCode = it1->second;
    } else {
        m_localCode = "GB18030";
    }
    /* 获得远程主机的地址和端口信息 */
    it1 = configuration.find("remotehost");
    if(it1 != configuration.end()) {
        remote_host = (*it1).second;
    } else {
        cerr<<"没有设置远程主机的名称或地址(remotehost)"<<endl;
        return;
    }
    
    it1 = configuration.find("port");
    if(it1 != configuration.end()) {
        const string& _port = (*it1).second;
        port = atoi(_port.c_str());
    } else {        
        cerr<<"没有设置远程主机的端口(port)"<<endl;
        return;
    }             
    m_inited = true;
}

void UdpAppender::write(const char* data, size_t size) {
    if(m_inited) {      
      try {
          if(!udpnode.isOpened())
              udpnode.open();
          hessian_output out(m_localCode.c_str());
          string call = out.start_call("log");
          call.append(data, size);
          out.complete_call(call);
          Address address(remote_host, port);
          udpnode.sendto(address, StringBuffer(call));
      } catch(io_exception& e) {
          cerr<< "[log4cpp] udp send error " << e.what()<<endl;
      } catch(CPPSocket::Exception& e) {
          cerr<< "[log4cpp] udp send error: method="<<e.getMethod()<<" code=" << e.getCode() << " message=" << e.getMessage() << endl;  
      } catch(exception& e) {
          cerr<< "[log4cpp] udp send error " << e.what()<<endl;
      }
   }
}

UdpAppender::~UdpAppender() throw() {
  if(udpnode.isOpened()) {
    try {
        udpnode.close();
    }catch(CPPSocket::Exception& e) {
        cerr<<"[log4cpp] udp close error: method="<<e.getMethod()<<" code=" << e.getCode() << " message=" << e.getMessage() << endl;  
    } catch(exception& e) {
        cerr<< "[log4cpp] udp send error " << e.what()<<endl;
    }
  }  
}
