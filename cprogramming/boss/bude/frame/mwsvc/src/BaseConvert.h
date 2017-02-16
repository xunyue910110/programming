#ifndef BASECONVERT_H
#define BASECONVERT_H

#include <sstream>

template<class out_type,class in_value>

out_type convert(const in_value & t)

{
    stringstream stream;
    stream<<t;//向流中传值
    out_type result;//这里存储转换结果
    stream>>result;//向result中写入值
    return result;
};

#endif //BASECONVERT_H
