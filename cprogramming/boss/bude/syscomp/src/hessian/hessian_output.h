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

#ifndef HESSIAN_HESSIAN_OUTPUT_H
#define HESSIAN_HESSIAN_OUTPUT_H
#include <iconv.h>
#include "hessian/exceptions.h"
#include "hessian/wrappers.h"

#include <string>

using namespace hessian::exceptions;
using namespace hessian::wrappers;
using namespace std;

namespace hessian {
class IConverter;
///Creates output serialisation for Hessian requests.
class hessian_output {
public:
    ///Constructor.
    hessian_output(const char * charsetName);

    ///Destructor.
    ~hessian_output();

    /**
       * Starts the reply
       *
       * <p>A successful completion will have a single value:
       *
       * <pre>
       * r
       * </pre>
       */
    string start_reply();

    /**
     * Completes reading the reply
     *
     * <p>A successful completion will have a single value:
     *
     * <pre>
     * z
     * </pre>
     */
    string& complete_reply(string& reply);

    /**
     * Writes a header name.  The header value must immediately follow.
     *
     * <code><pre>
     * H b16 b8 foo <em>value</em>
     * </pre></code>
     */
    string& write_header(string& reply, const string& name);


    ///Start a Hessian method call.
    string start_call(const string& method_name);

    /**
      * Writes the call tag.  This would be followed by the
      * headers and the method tag.
      *
      * <code><pre>
      * c major minor
      * </pre></code>
      *
      * @param method the method name to call.
      */
    string start_call();

    ///Finish a Hessian method call.
    string& complete_call(string& call);

    ///Set a new parameter for the method call; calls writeObject().
    //		string& set_parameter(string& call, Object* object);
    string& write_method(string& call, const string& method_name);
    ///Write an Object to the call.
    string& write_object(string& call, Object* object);
protected:
    ///Write a Binary to the call.
    string& write_binary(string& call, Binary* object);

    ///Write a Boolean to the call.
    string& write_boolean(string& call, Boolean* object);

    ///Write a Date to the call.
    string& write_date(string& call, Date* object);

    ///Write a Double to the call.
    string& write_double(string& call, Double* object);

    ///Write a 32 bit Integer to the call.
    string& write_integer(string& call, Integer* object);

    ///Write a List to the call.
    string& write_list(string& call, List* object);

    ///Write a 64 bit Long to the call.
    string& write_long(string& call, Long* object);

    ///Write a Map to the call.
    string& write_map(string& call, Map* object);

    ///Write a Null to the call.
    string& write_null(string& call, Null* object);

    ///Write a Ref to the call.
    string& write_ref(string& call, Ref* object);

    ///Write a Remote to the call.
    string& write_remote(string& call, Remote* object);

    ///Write a String to the call.
    string& write_string(string& call, String* object);

    ///Write an Xml to the call.
    string& write_xml(string& call, Xml* object);

    string& write_fault(string& call, Fault* fault);

private:
    string& write_long64(string& call, long long value);
    string& write_string(string& call, const string& value);

    IConverter* m_converter;
};
}

#endif
