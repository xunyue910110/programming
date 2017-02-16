#include "dump.h"
#include <list>
#include <fstream>
#include "hessian_output.h"
#include "hessian_input.h"
#include "string_input_stream.h"
using namespace std;
using namespace hessian;


#include "dump.h"
#include <list>
#include <fstream>
#include "hessian_output.h"
#include "hessian_input.h"
#include "string_input_stream.h"
using namespace std;
using namespace hessian;

/* 在Makefile 中 -liconv */
int main() {
        cout << "---hello--" <<endl;
    hessian_output out("GB18030");//GB18030
    string call = out.start_call("hello");
    std::list<Object*> lt;
    lt.push_back(new String("luzl1"));
    lt.push_back(new String("lu2"));
    out.write_object(call, new List(lt));
    out.write_object(call, new Integer(2));
    
    std::map<string,Object*> mp;
    mp["key1"] = new String("luzl");
    mp["key2"] = new String("lueq");  
    
    mp.insert(std::make_pair("key3",new String("luzl3")));
    mp.insert(std::make_pair("key4",new String("luzl4")));
        
    out.write_object(call, new Map(mp));
    out.complete_call(call);
    cout << call << endl;

    ofstream of("a.dat");
    of << call << endl;

    return 0;
}

int _main() {
    try {
        ifstream of("a.dat");
        string data;
        char ch;
        while(of.get(ch))
            data.push_back(ch);
        hessian_input *p;
        hessian_input in(auto_ptr<input_stream>(new string_input_stream(data)),"GB18030");//gb2312
        //
        in.read_call();
        while(true) {
            string h = in.read_header();
            if(h.size()) {
                cout << h << " " << (dynamic_cast<String*>(auto_ptr<Object>(in.read_object()).get())) -> value() << endl;
            } else {
                break;
            }
        }
        cout << in.read_method() << endl;
        Object* result = in.read_object();
        auto_ptr<Object> autorelease(result);
       // if (List* l = dynamic_cast<List*>(result)) {
            // print list
        //    dumplist(*l);
        //}
        cout << ((Double*)in.read_object())->value() << endl;
        in.complete_call();
    } catch(std::exception& e) {
        cout << e.what() << endl;
    }
    return 0;
}
