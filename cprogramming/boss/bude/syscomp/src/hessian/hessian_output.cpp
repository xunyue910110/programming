// Copyright (C) 2004 Aldratech Ltd. See the LICENSE file for licensing information.
/*
 This file is part of hessiancpp.
 
    hessiancpp is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
 
    hessiancpp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public License
    along with hessiancpp; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "hessian_output.h"
#include "iconverter.h"
using namespace hessian::exceptions;
using namespace hessian::wrappers;

namespace hessian {

hessian_output::hessian_output(const char * charsetName) {
    m_converter = new IConverter(charsetName,"UTF-8");
}

hessian_output::~hessian_output() {
    delete m_converter;
}

string hessian_output::start_reply() {
    string reply("r");
    reply.append(1, (char)1);
    reply.append(1, (char)0);
    return reply;
}

string& hessian_output::complete_reply(string& reply) {
    return reply.append(1, 'z');
}

string& hessian_output::write_header(string& reply, const string& name) {
    reply.append(1, 'H');
    return write_string(reply, name);
}

string hessian_output::start_call() {
    string mc("c");
    mc.append(1, (char)1);
    mc.append(1, (char)0);
    return mc;
}
string& hessian_output::write_method(string& call, const string& method_name) {
    unsigned short mlen = method_name.length();
    unsigned char mlen16 = mlen >> 8;
    unsigned char mlen8 = mlen & 0x00FF;
    call.append(1, 'm');
    call.append(1, mlen16);
    call.append(1, mlen8);
    call.append(method_name);
    return call;
}

string hessian_output::start_call(const string& method_name) {
    string mc = start_call();
    write_method(mc, method_name);
    return mc;
}

string& hessian_output::complete_call(string& call) {
    call.append(1, 'z');
    return call;
}

//	string& hessian_output::set_parameter(string& call, Object* object) {
//		return write_object(call, object);
//	}

string& hessian_output::write_binary(string& call, Binary* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'B');
    const string& value = object->value();
    size_t slen = value.size();
    unsigned char b16 = slen >> 8;
    unsigned char b8 = slen & 0x00FF;
    call.append(1, b16);
    call.append(1, b8);
    call.append(value);
    return call;
}

string& hessian_output::write_boolean(string& call, Boolean* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, object->value() ? 'T' : 'F');
    return call;
}

string& hessian_output::write_date(string& call, Date* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'd');
    return write_long64(call, object->value());
}

string& hessian_output::write_double(string& call, Double* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'D');
    double dvalue = object->value();
    // tranfer 64-bit double to a 64-bit long
    long long *value = (long long*) & dvalue;
    return write_long64(call, *value);
}

string& hessian_output::write_integer(string& call, Integer* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'I');
    int value = object->value();

    int b32 = value >> 24;
    int b24 = (value >> 16) & 0x000000FF;
    int b16 = (value >> 8) & 0x000000FF;
    int b8 = value & 0x000000FF;

    call.append(1, (char)b32);
    call.append(1, (char)b24);
    call.append(1, (char)b16);
    call.append(1, (char)b8);
    return call;
}

string& hessian_output::write_list(string& call, List* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'V');
    list<Object*>& l = object->value();
    for (list<Object*>::iterator i = l.begin(); i != l.end(); i++) {
        write_object(call, *i);
    }
    call.append(1, 'z');
    return call;
}

string& hessian_output::write_long(string& call, Long* object) {
    if (object == NULL) {
        return write_null(call, NULL);
    }
    call.append(1, 'L');
    return write_long64(call, object->value());
}

string& hessian_output::write_map(string& call, Map* object) {
    call.append(1, 'M');
    Map::basic_type& m = object->value();
    for (Map::basic_type::iterator i = m.begin(); i != m.end(); i++) {
        //writeObject(call, const_cast<String*>(&((*i).first)));
        call.append(1, 'S');
        write_string(call, (*i).first);
        write_object(call, (*i).second);
    }
    call.append(1, 'z');
    return call;
}

string& hessian_output::write_long64(string& call, long long value) {
    long long b64 = (value >> 56) & 0x00000000000000FF;
    long long b56 = (value >> 48) & 0x00000000000000FF;
    long long b48 = (value >> 40) & 0x00000000000000FF;
    long long b40 = (value >> 32) & 0x00000000000000FF;
    long long b32 = (value >> 24) & 0x00000000000000FF;
    long long b24 = (value >> 16) & 0x00000000000000FF;
    long long b16 = (value >> 8) & 0x00000000000000FF;
    long long b8 = value & 0x00000000000000FF;

    call.append(1, (char)b64);
    call.append(1, (char)b56);
    call.append(1, (char)b48);
    call.append(1, (char)b40);
    call.append(1, (char)b32);
    call.append(1, (char)b24);
    call.append(1, (char)b16);
    call.append(1, (char)b8);
    return call;
}


string& hessian_output::write_null(string& call, Null* object) {
    call.append(1, 'N');
    return call;
}


string& hessian_output::write_object(string& call, Object* object) {
    if (object->classname() == "Binary") {
        return write_binary(call, dynamic_cast<Binary*>(object));
    }
    if (object->classname() == "Boolean") {
        return write_boolean(call, dynamic_cast<Boolean*>(object));
    }
    if (object->classname() == "Date") {
        return write_date(call, dynamic_cast<Date*>(object));
    }
    if (object->classname() == "Double") {
        return write_double(call, dynamic_cast<Double*>(object));
    }
    if (object->classname() == "Integer") {
        return write_integer(call, dynamic_cast<Integer*>(object));
    }
    if (object->classname() == "List") {
        return write_list(call, dynamic_cast<List*>(object));
    }
    if (object->classname() == "Long") {
        return write_long(call, dynamic_cast<Long*>(object));
    }
    if (object->classname() == "Map") {
        return write_map(call, dynamic_cast<Map*>(object));
    }
    if (object->classname() == "Null") {
        return write_null(call, dynamic_cast<Null*>(object));
    }
    if (object->classname() == "Ref") {
        return write_ref(call, dynamic_cast<Ref*>(object));
    }
    if (object->classname() == "Remote") {
        return write_remote(call, dynamic_cast<Remote*>(object));
    }
    if (object->classname() == "String") {
        return write_string(call, dynamic_cast<String*>(object));
    }
    if (object->classname() == "Xml") {
        return write_xml(call, dynamic_cast<Xml*>(object));
    }
    if (object->classname() == "Fault") {
        return write_fault(call, dynamic_cast<Fault*>(object));
    }
    // throw exception, should not get here, really
    throw io_exception(string("hessian_output::write_object(): come on, really, what kind of Object is ").append(object->classname()));
}

string& hessian_output::write_ref(string& call, Ref* object) {
    return write_null(call, NULL);
}


string& hessian_output::write_remote(string& call, Remote* object) {
    return write_null(call, NULL);
}

string& hessian_output::write_string(string& call, String* object) {
    call.append(1, 'S');
    return write_string(call, object->value());
}

string& hessian_output::write_xml(string& call, Xml* object) {
    call.append(1, 'X');
    return write_string(call, object->value());
}

string& hessian_output::write_fault(string& reply, Fault* fault) {
    reply.append(1, 'f');

    reply.append(1, 'S');
    write_string(reply, "code");
    reply.append(1, 'S');
    write_string(reply, fault->code());

    reply.append(1, 'S');
    write_string(reply, "message");
    reply.append(1, 'S');
    write_string(reply, fault->message());

    return reply;
}

string& hessian_output::write_string(string& call, const string& value) {
    string ivalue = m_converter->convert(value);
    //计算长度
    unsigned short slen = 0;
    for (int i = 0; i < ivalue.size(); i++) {
        unsigned char ch = ivalue.at(i);
        slen ++;

        if (ch < 0x80) {
            ;
        } else if ((ch & 0xe0) == 0xc0) {
            i+=1;
        } else if ((ch & 0xf0) == 0xe0) {
            i+=2;
        } else
            throw	io_exception("bad utf-8 encoding");
    }
    unsigned char b16 = slen >> 8;
    unsigned char b8 = slen & 0x00FF;
    call.append(1, b16);
    call.append(1, b8);
    call.append(ivalue);
    return call;
}
}
