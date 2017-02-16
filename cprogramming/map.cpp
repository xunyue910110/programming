#include <map>
#include <string>
#include <unordered_map>

using namespace std;
#include <stdio.h>

int main()
{
    unordered_map<int, string> mapint;
    mapint[2] = "zhaojun";
    mapint[1] = "luoyan";
    mapint[5] = "zhenxin";
    mapint[3] = "googl";
    
    unordered_map<int, string>::iterator it;
    for (it = mapint.begin(); it != mapint.end(); ++it)
        printf("%d\n, %s\n", it->first, it->second.c_str());
}
