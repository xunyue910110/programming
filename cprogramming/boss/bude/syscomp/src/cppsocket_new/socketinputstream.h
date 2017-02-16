/* 2007-07-30 add by juyf@lianchuang.com*/

#ifndef _CPP_SOCKET_INPUTSTREAM_H
#define _CPP_SOCKET_INPUTSTREAM_H

#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "cppsocket/tcpconnection.h"
#include "cppsocket/stringbuffer.h"

USING_NAMESPACE(std);
using namespace hessian;
using namespace CPPSocket;

  
namespace nettopo {

/* 从socket连接中read数据 */
class SocketInputStream : public input_stream {
public:
    SocketInputStream(TCPConnection& con) : m_connnection(con), m_cursor(0), m_eof(false) {}
    /// Read a character from the stream.
    int read() throw(io_exception) {
        try {
            string& str = m_buffer;
            if(m_cursor >= str.size()) {
                m_cursor = 0;
                int n = m_connnection.receive(m_buffer);
                if(n <= 0) {
                    m_eof = true;
                    throw io_exception("socket_input_stram::read(): read past end of stream");
                }
            }
            return str[m_cursor++];
        } catch(CPPSocket::Exception& e) {
            throw io_exception(string("SocketInputStream::read") + e.getMessage());
        }
    }
    /// Test stream for EOF.
    bool eof() throw(io_exception) {
        return m_eof;
    }
    /// Destructor.
    ~SocketInputStream() throw() {       
    }
private:
    TCPConnection& m_connnection;
    StringBuffer m_buffer;
    size_t m_cursor;
    bool m_eof;
};


};
#endif