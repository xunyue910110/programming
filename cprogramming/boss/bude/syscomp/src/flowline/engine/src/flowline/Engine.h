#ifndef _ENGINE_H_
#define _ENGINE_H_
#include <config-all.h>
#include "flowline/FlowLineDao.h"
#include "flowline/adapter/AdapterFactory.h"

USING_NAMESPACE( std );
class Properties;
namespace flowline {
class Definition;

class Engine {
public:
    Engine(const Properties* env, FlowLineDao* dao, AdapterFactory* factory, const string& main = "" );
    virtual ~Engine();
    void run();
private:
    bool doCycle( Definition& def, int cycleLogId, const Cycle::Instance& ci );
    void recoverResources( Definition& def, int cycleLogId );
    void run( Definition& def );
    FlowLineDao* m_dao;
    AdapterFactory* m_factory;
    string m_main;
    const Properties* m_env;
};

};

#endif //_ENGINE_H_
