#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "Session.h"

/**
 * \class Factory Factory.h
 * \brief IPC 会话工厂,该类创建会话实例
 *
 * <code>src/client/ClientFactory::getInstance()</code> or
 * <code>src/server/CServerFactory::getInstance()</code> methods.
 */
class Factory { 
  public:
    /**
     * 创建会话实例 对于Client会话 同服务建立起连接
     * @param address server host address (ignored for local sessions)
     * @param port server port (ignored for local sessions)
     * @return session object
     */
    virtual Session* create(char const* host, int port) = 0;
};

#endif
