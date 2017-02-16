#ifndef CDATALIST_HEADER_INCLUDED
#define CDATALIST_HEADER_INCLUDED

#include <vector>
#include "CVar.h"
//USEING_NAMESPACE(std);
using  std::vector;
class CDataList : public vector<CVar>
{
public:

    CVar & operator [](int n){                                  //下标运算符重载
        if( static_cast<unsigned int>(n) >= this->size() )
            this->resize(n + 1);
        return *(this->begin() + n);
    }
    
    friend ostream & operator << (ostream & os , CDataList & list)
    {
        for( int i = 0;  static_cast<unsigned int>(i) < list.size(); i++ ){
            os << list[i] << ",";
        }
        return os << '\n';
    }
};

#endif  /* CDATALIST_HEADER_INCLUDED */
