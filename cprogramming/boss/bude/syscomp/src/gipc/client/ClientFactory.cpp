#define RIPC_IMPLEMENTATION

#include "ClientFactory.h"
#include "SessionStub.h"


//客户端工厂类实现

ClientFactory ClientFactory::theFactory;

Session* ClientFactory::create(char const* host, int port)
{ 
    return new SessionStub(host, port);
}
