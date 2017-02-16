#ifndef _DUMP_H_
#define _DUMP_H_
#include "hessian/wrappers.h"

namespace hessian {
using namespace wrappers;

template <typename T>
inline void dump(T t) {
    cout << t->classname() << "\t: " << t->value() << endl;
}

inline void dump(Object* o);

inline void dumplist(const list<Object*>& l) {
    cout << "List {" << endl;
    for (list<Object*>::const_iterator i = l.begin(); i != l.end(); i++) {
        cout << "\t";
        dump(*i);
    }
    cout << "}" << endl;
}

inline void dumpmap(Map::basic_type l) {
    for (Map::basic_type::iterator i = l.begin(); i != l.end(); i++) {
        cout << "{" << endl << "key ";
        cout << (*i).first << endl;
        cout << "value ";
        dump((*i).second);
        cout << "}" << endl;
    }
}

inline void dump(Object* o) {
    if (Binary* _obj = dynamic_cast<Binary*>(o)) {
        cout << "binary length " << _obj->value().length() << endl;
        return ;
    }
    if (Boolean* _obj = dynamic_cast<Boolean*>(o)) {
        dump(_obj);
        return ;
    }
    if (Date* _obj = dynamic_cast<Date*>(o)) {
        dump(_obj);
        return ;
    }
    if (Double* _obj = dynamic_cast<Double*>(o)) {
        dump(_obj);
        return ;
    }
    if (Integer* _obj = dynamic_cast<Integer*>(o)) {
        dump(_obj);
        return ;
    }
    if (List* _obj = dynamic_cast<List*>(o)) {
        dumplist(_obj->value());
        return ;
    }
    if (Long* _obj = dynamic_cast<Long*>(o)) {
        dump(_obj);
        return ;
    }
    if (Map* _obj = dynamic_cast<Map*>(o)) {
        dumpmap(_obj->value());
        return ;
    }
    if (Null* _obj = dynamic_cast<Null*>(o)) {
        dump(_obj);
        return ;
    }
    if (Ref* _obj = dynamic_cast<Ref*>(o)) {
        dump(_obj);
        return ;
    }
    if (Remote* _obj = dynamic_cast<Remote*>(o)) {
        dump(_obj);
        return ;
    }
    if (String* _obj = dynamic_cast<String*>(o)) {
        dump(_obj);
        return ;
    }
    if (Xml* _obj = dynamic_cast<Xml*>(o)) {
        dump(_obj);
        return ;
    }
    if (Fault* _obj = dynamic_cast<Fault*>(o)) {
        dump(_obj);
        return ;
    }
    cout << "Warning: dump does not handle " << o->classname() << endl;
}
}

#endif /*_DUMP_H_*/
