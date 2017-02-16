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
/*
	Code in this file is partially based on code
	from the Micro-Hessian Java implementation.
*/

#include "hessian_input.h"
#include "iconverter.h"
using namespace hessian::wrappers;

namespace hessian {
hessian_input::hessian_input(auto_ptr<input_stream> is, const char * charsetName) : _is(is), _peek(-1) {
    m_converter = new IConverter("UTF-8", charsetName);
}

/// Destructor.
hessian_input::~hessian_input() {
    delete m_converter;
}

/**
 * Starts reading the call
 *
 * <pre>
 * c major minor
 * </pre>
 */
int hessian_input::read_call() throw(io_exception) {
    int tag = read();

    if (tag != 'c')
        throw expect("hessian call", tag);

    int major = read();
    int minor = read();

    return (major << 16) + minor;
}

/**
 * Starts reading the call
 *
 * <p>A successful completion will have a single value:
 *
 * <pre>
 * m b16 b8 method
 * </pre>
 */
string hessian_input::read_method() throw(io_exception) {
    int tag = read();

    if (tag != 'm')
        throw expect("hessian method", tag);

    int b16 = read();
    int b8 = read();
    int len = (b16 << 8) + b8;

    return read_string_impl(len);
}

/**
 * Completes reading the call
 *
 * <p>A successful completion will have a single value:
 *
 * <pre>
 * z
 * </pre>
 */
void hessian_input::complete_call() throw(io_exception) {
    int tag = read();

    if (tag == 'z') {}
    else
        throw expect("hessian completeCall", tag);
}


/**
   * Reads a header, returning null if there are no headers.
   *
   * <pre>
   * H b16 b8 value
   * </pre>
   */
string hessian_input::read_header()  throw(io_exception) {
    int tag = read();

    if (tag == 'H') {
        int b16 = read();
        int b8 = read();
        int len = (b16 << 8) + b8;

        return read_string_impl(len);
    } else {
        _peek = tag;
        return string();
    }
}

void hessian_input::start_reply() throw(io_exception) {
    int tag = read();

    if (tag != 'r')
        throw expect("hessian reply", tag);

    int major = read();
    int minor = read();
    if (major & minor)
        ;	// avoid warning
}

void hessian_input::complete_reply() throw(io_exception) {
    int tag = read();

    if (tag != 'z')
        throw expect("end of reply", tag);
}

//	Object* hessian_input::get_result() throw(io_exception) {
//		return read_object();
//	}

Object* hessian_input::read_object() throw(io_exception) {
    int tag = read();
    return read_object(tag);
}

Object* hessian_input::read_object(int tag) throw(io_exception) {
    switch (tag) {
    case 'B':
        return new Binary(read_bytes(tag));
    case 'T':
    case 'F':
        return new Boolean(read_boolean(tag));
    case 'd':
        return new Date(read_date(tag));
    case 'D':
        return new Double(read_double(tag));
    case 'f':
        return new Fault(read_fault(tag));
    case 'I':
        return new Integer(read_int(tag));
    case 'V': {
        List* l = new List();
        read_list(tag, l->value());
        return l;
    }
    case 'L':
        return new Long(read_long(tag));
    case 'M': {
        Map* m = new Map();
        read_map(tag, m->value());
        return m;
    }
    case 'N':
        return new Null();
    case 'R':
        return new Ref(read_ref(tag));
    case 'S':
        return new String(read_string(tag));
    case 'X':
        return new Xml(read_xml(tag));
    default:
        throw io_exception(string("hessian_input::readObject(): tag ").append(1, (char) tag).append(" cannot be handled"));
    }
}


bool hessian_input::read_boolean(int tag) throw(io_exception) {
    switch (tag) {
    case 'T':
        return true;
    case 'F':
        return false;
    default:
        throw expect("bool", tag);
    }
}

int hessian_input::read_int(int tag) throw(io_exception) {
    if (tag != 'I')
        throw expect("integer", tag);

    int b32 = read() & 0xFF;
    int b24 = read() & 0xFF;
    int b16 = read() & 0xFF;
    int b8 = read() & 0xFF;

    return((b32 << 24) + (b24 << 16) + (b16 << 8) + b8) & 0x00000000FFFFFFFF;
}

long long hessian_input::read_long(int tag) throw(io_exception) {
    if (tag != 'L')
        throw expect("long", tag);

    return read_long64();
}

double hessian_input::read_double(int tag) throw(io_exception) {
    if (tag != 'D')
        throw expect("double", tag);
    long long lval = read_long64();
    double* dval = (double*) & lval;
    return *dval;
}

long long hessian_input::read_date(int tag) throw(io_exception) {
    if (tag != 'd')
        throw expect("date", tag);
    return read_long64();
}

long long hessian_input::read_long64() throw(io_exception) {
    unsigned long long b64 = read() & 0xFFULL;
    unsigned long long b56 = read() & 0xFF;
    unsigned long long b48 = read() & 0xFF;
    unsigned long long b40 = read() & 0xFF;
    unsigned long long b32 = read() & 0xFF;
    unsigned long long b24 = read() & 0xFF;
    unsigned long long b16 = read() & 0xFF;
    unsigned long long b8 = read() & 0xFF;
    long long value = (b64 << 56) +
                      (b56 << 48) +
                      (b48 << 40) +
                      (b40 << 32) +
                      (b32 << 24) +
                      (b24 << 16) +
                      (b16 << 8) +
                      b8;
    return value;
}

string hessian_input::read_string(int tag) throw(io_exception) {
    if (tag == 'N')
        return string("");
    if (tag != 'S')
        throw expect("string", tag);

    int b16 = read();
    int b8 = read();

    int len = (b16 << 8) + b8;

    return read_string_impl(len);
}

string hessian_input::read_bytes(int tag) throw(io_exception) {
    if (tag == 'N')
        return("");
    if (tag != 'B')
        throw expect("bytes", tag);

    int b16 = read();
    int b8 = read();

    int len = (b16 << 8) + b8;

    string bos;
    for (int i = 0; i < len; i++)
        bos.append(1, (char)read());
    return bos;
}

string hessian_input::read_xml(int tag) throw(io_exception) {
    if (tag == 'N') {
        return("");
    }
    if (tag != 'X') {
        throw expect("xml", tag);
    }
    int b16 = read();
    int b8 = read();

    int len = (b16 << 8) + b8;

    return read_string_impl(len);
}

void hessian_input::read_list(int tag, List::basic_type& l) throw(io_exception) {
    if (tag == 'N') {
        return;
    }
    if (tag != 'V') {
        throw expect("list", tag);
    }
    int list_length = 0;
    int meta = read();
    // parse optional (meta) type and/or length
    while (meta == 't' || meta == 'l') {
        if (meta == 't') {
            // type
            string list_type = read_string('S');
        }
        if (meta == 'l') {
            // length
            int b32 = read();
            int b24 = read();
            int b16 = read();
            int b8 = read();
            list_length = (b32 << 24) + (b24 << 16) + (b16 << 8) + b8;
        }
        meta = read();
    }
    // read in the list data    
    while (meta != 'z') {	// list ends with 'z'
        // read object
        Object* obj = read_object(meta);
        // add to list
        l.push_back(obj);
        // next please
        meta = read();
    }
}

void hessian_input::read_map(int tag, Map::basic_type& m) throw(io_exception) {
    if (tag == 'N') {
        return;
    }
    if (tag != 'M') {
        throw expect("map", tag);
    }
    int meta = read();
    // parse optional (meta) type
    while (meta == 't') {
        if (meta == 't') {
            // type
            string map_type = read_string('S');
        }
        meta = read();
    }
    // read in the map data
    while (meta != 'z') {	// map ends with 'z'
        // read key object
        string key(read_string(meta));
        // read value object
        meta = read();
        Object* val = read_object(meta);
        // add to list
        m.insert(std::make_pair(key, val));
        // next please
        meta = read();
    }
}

int hessian_input::read_ref(int tag) throw(io_exception) {
    if (tag != 'R') {
        throw expect("ref", tag);
    }
    int b32 = read() & 0xFF;
    int b24 = read() & 0xFF;
    int b16 = read() & 0xFF;
    int b8 = read() & 0xFF;

    return((b32 << 24) + (b24 << 16) + (b16 << 8) + b8) & 0x00000000FFFFFFFF;
}

string hessian_input::read_string_impl(int length) throw(io_exception) {
    string sb;

    for (int i = 0; i < length; i++) {
        unsigned char ch = read();
        sb.append(1, (char) ch);

        if (ch < 0x80)
            ;
        else if ((ch & 0xe0) == 0xc0) {
            int ch1 = read();
            sb.append(1, (char) ch1);
        } else if ((ch & 0xf0) == 0xe0) {
            int ch1 = read();
            int ch2 = read();
            sb.append(1, (char) ch1);
            sb.append(1, (char) ch2);
        } else
            throw	io_exception("bad utf-8 encoding");
    }

    return m_converter->convert(sb);
}

Fault hessian_input::read_fault(int tag) throw(io_exception) {
    if (tag != 'f')
        throw expect("fault", tag);

    // skip over "code"
    int _tag = read();
    read_string(_tag);
    // read code value
    _tag = read();
    String code(read_string(_tag));

    // skip over "message"
    _tag = read();
    read_string(_tag);
    // read message value
    _tag = read();
    String message(read_string(_tag));

    // skip over "detail"
    _tag = read();
    if(_tag != 'z') {
        read_string(_tag);

        //read stack trace
        _tag = read();
        Map destroyer;
        read_map(_tag, destroyer.value());
        // the Map above acts goes out of scope below
        // and deletes itself to prevent a lot of leaks
    } else {
        _peek = _tag;
    }
    const string& mesg = message.value();
    // hack: exception type encoded in the message
    unsigned long exception_type_start = mesg.find(']');
    if (exception_type_start != string::npos) {
        string exception_type(mesg.substr(0, exception_type_start));
        string exception_mesg(mesg.substr(exception_type_start + 1));
        return Fault(exception_type, exception_mesg);
    }
    // end hack
    return Fault(code.value(), message.value());
}

io_exception hessian_input::expect(const string& expect, int ch) {
    if (ch < 0)
        return io_exception("expected " + expect + " at end of file");
    else
        return io_exception("expected " + expect + " at " + (char) ch);
}

int hessian_input::read() {
    if (_peek >= 0) {
        int value = _peek;
        _peek = -1;
        return value;
    }

    int ch = (unsigned char)_is->read();

    return ch;
}
}

