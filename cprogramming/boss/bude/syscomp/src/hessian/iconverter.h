#ifndef _ICONVERTER_H_
#define _ICONVERTER_H_
#include <iconv.h>
#include <string>
namespace hessian {
using namespace std;
class IConverter {
    iconv_t cd;
public:
    IConverter(const char *from_charset,const char *to_charset);

    string convert(const string& instr);

    ~IConverter();
};
}
#endif //_ICONVERTER_H_
