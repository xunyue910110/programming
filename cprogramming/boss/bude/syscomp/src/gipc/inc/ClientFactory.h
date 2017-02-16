#ifndef __CLIENT_FACTORY_H__
#define __CLIENT_FACTORY_H__

#include "Factory.h"

/**
 * \class ClientFactory ClientFactory.h
 * \brief 创建客户端会话对象的工厂类
 *
 * 创建远程客户会话的工厂类
 */
class ClientFactory : public Factory { 
  public:
    static ClientFactory* getInstance() { 
	return &theFactory;
    }
    Session* create(char const* host, int port);
    
  protected:
    static ClientFactory theFactory;
};

#endif
