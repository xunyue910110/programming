#define RIPC_IMPLEMENTATION

#include "ServerFactory.h"
#include "Server.h"

ServerFactory ServerFactory::theFactory(Server::theServer); //初始话

Session* ServerFactory::create(char const* host, int port) { 
    return server.create(host, port);
}

ServerFactory::ServerFactory(Server& theServer) : server(theServer) {}
