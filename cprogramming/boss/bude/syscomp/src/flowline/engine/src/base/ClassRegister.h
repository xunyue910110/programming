#ifndef _CLASSREGISTER_H_
#define _CLASSREGISTER_H_
#include "config-all.h"
#include <string>
#include <map>
#include <vector>
USING_NAMESPACE( std );

/**
 * 注册Class宏
 * @name 伪Class名,建议使用类似java的命名 com.linkage.project.module.class * 
 * @cppclass 真实的C++类名
 * @baseclass 公共的基类
 */
#define REGISTER_CLASS(name, cppclass, baseclass)  \
    baseclass* cppclass##Creator() { return new cppclass(); } \
    int cppclass##CreatorTemp = baseclass##ClassRegister::__registerClass(name, cppclass##Creator) 


/**
 * 在很多场景下, 我们需要定义一个抽象基类, 然后后不同的实现, 比如设计了一个DAO, 然后有
 * Oracle的实现, Sybase的实现, 在Java中很容易通过反射, 来动态创建类, 但在C++, 不可能
 * 下面我们是通过宏和模板的机制完成,注册和创建
 * 
 * #Dao.h
 * class Dao {
 *   //virtual ...
 * };
 * DECLARE_CLASS_REGISTER(Dao);
 *
 * #Dao.cpp 
 * DEFINE_CLASS_REGISTER(Dao); 
 * 
 * #SysbaseDao.cpp
 * class SysbaseDao : public Dao {
 *    //...
 * }
 * REGISTER_CLASS("com.linkage.prj1.m2.impl.SysbaseDao", SysbaseDao, Dao);
 * 
 * #OracleDao.cpp
 * class OracleDao : public Dao {
 *    //...
 * }
 * REGISTER_CLASS("com.linkage.prj1.m2.impl.OracleDao", OracleDao, Dao);
 * 
 * 
 * #Manager.cpp
 * 
 * 
 * void Manager::initDao() {
 *    //从配置文件or数据库中读取伪类名
 *    string implClass = //...
 *    m_dao = ClassRegister<Dao>::create(implClass);
 * }
 * 
 * 
 * @author luzl  
 */

#define DECLARE_CLASS_REGISTER(T) \
class T##ClassRegister { \
public: \
    typedef T* ( *ClassCreator ) (); \
    static int __registerClass( const string& name, ClassCreator creator ); \
    static T * create( const string & className );\
    static void getClassNames( vector<string>& classNames ); \
private: \
    static map < string, ClassCreator >& creators(); \
};

#define DEFINE_CLASS_REGISTER(T) \
 int T##ClassRegister::__registerClass( const string& name, T##ClassRegister::ClassCreator creator ) {\
    creators() [ name ] = creator;\
    return 0;\
}\
 T * T##ClassRegister::create( const string & className ) {\
    map < string, T##ClassRegister::ClassCreator >::iterator it = creators().find( className );\
    if ( it != creators().end() ) {\
        return it->second();\
    }\
    return 0;\
}\
 void T##ClassRegister::getClassNames( vector<string>& classNames ) {\
    map < string, T##ClassRegister::ClassCreator >::iterator it = creators().begin();\
    for ( ;it != creators().end();it++ ) {\
        classNames.push_back( it->first );\
    }\
}\
 map < string, T##ClassRegister::ClassCreator >& T##ClassRegister::creators() {\
    static map < string, T##ClassRegister::ClassCreator > s_creators;\
    return s_creators;\
}

#endif //_CLASSREGISTER_H_

