#include "hessian/iconverter.h"
#include <iconv.h>
#include <iostream>
#include <errno.h>
#include "hessian/exceptions.h"
using namespace hessian::exceptions;
namespace hessian {

IConverter::IConverter(const char *from_charset,const char *to_charset) {
    cd = iconv_open(to_charset,from_charset);
    if (cd==(iconv_t)-1)
        throw fault_exception("iconv_open", strerror(errno));
}

string IConverter::convert(const string& instr) {
    size_t inlen = instr.size();
    const char *in = instr.c_str();
    const char **pin = &in;
	char buffer[1024];
	
    string result;
    size_t rs = 0;
    do {
    	char* p = buffer;    
    	size_t outsize = sizeof(buffer) - 1;
#ifdef NEW_ICONV
    	rs = iconv(cd,(char **)pin,&inlen,&p,&outsize);
#else
    	rs = iconv(cd,pin,&inlen,&p,&outsize);
#endif
		//printf("%s,%s,%d,%d\n", in, buffer, inlen, outsize);				
		*p = '\0';
	    result.append(buffer);
    } while(rs > 0);
    
    if(rs == 0)
        return result;
    else {
        throw fault_exception("iconv", strerror(errno));
    }
}

IConverter::~IConverter() {
    iconv_close(cd);
}
}
/*
using namespace hessian;
int main() {
	try {
	string a("正在安装1");
	IConverter cv("gb2312", "utf-8");
	IConverter cv2("utf-8", "gb2312");
	cout << cv.convert(a) << endl;
	cout << cv2.convert(cv.convert(a)) << endl;
	string o = cv.convert(a);
	//for(int i=0; i<o.size(); i++)
		//printf("%x %x %x %x\n ", (int)o.at(i), (int)((unsigned char)o.at(i) < 0x80), (int)(o.at(i) & 0xe0), (int)(o.at(i) & 0xf0));
	cout << o << endl;
	return 0;
	}catch(const char* e) {
		cout << e << endl;
	}catch(...) {
		cout << "unknow" << endl;
	}
}*/
