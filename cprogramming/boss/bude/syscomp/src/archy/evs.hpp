#ifndef ARCHY_EDVS_INCLUDE_FILE
#define ARCHY_EDVS_INCLUDE_FILE
#include "archy_config.hpp"
USING_STD_NAMESPACE;

namespace archy {

#define EVS5(in, out)  evs(in, out, 5)
#define DVS5(in, out)  dvs(in, out, 5)

void evs(const char* in, char* out);
void dvs(const char* in, char* out);
void evs(const char* in, char* out, int times);
void dvs(const char* in, char* out, int times);
string Str2HexNum(const char* in);
string HexNum2Str(const char* in);

string Evs5Num(const string& str) ;

string Dvs5Num(const string& str) ;

string Encode(const string& str) ;

string Decode(const string& str) ;

} //namespace archy 


#endif // ARCHY_EDVS_INCLUDE_FILE
