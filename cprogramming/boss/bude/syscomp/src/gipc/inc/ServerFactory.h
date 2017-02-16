#ifndef __SERVER_FACTORY_H__
#define __SERVER_FACTORY_H__

#include "Factory.h"

class Server;

/**
 * \class ServerFactory ServerFactory.h
 * \brief 创建服务端会话对象的工厂类
 *
 * 创建本地会话工厂类
 */
class ServerFactory : public Factory { 
  public:
    static ServerFactory* getInstance() { 
	return &theFactory;
    }
    Session* create(char const* host, int port);
    ServerFactory(Server&);

  protected:
    Server& server;
    static ServerFactory theFactory;
};

#endif
