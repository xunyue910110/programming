#ifndef _FIELD_H_
#define _FIELD_H_
#include "config-all.h"
#include "flowline/DataSet.h"
#include <string>
USING_NAMESPACE(std);

namespace flowline {
struct Field {
    enum {
        T_NUMBER = 'N',
        T_STRING = 'S',
        T_DATETIME = 'T'
    };
    string name;
    string description;
    int type;
    int length;
    int precision;
    string format;

    dataset::Field& toDataSetField(dataset::Field& field) const {
        field.setFieldName(name.c_str());
        if( type  == flowline::Field::T_NUMBER) {
            if ( precision == 0 ) {
                if ( length <= 9 )
                    field.setDataType(dataset::FTINT);
                else if(length <= 18)
                    field.setDataType(dataset::FTINT64);
                else
                    field.setDataType(dataset::FTSTRING);
                    field.setLength(22);
                    //THROW_P1(FlowLineException, "%s, 整型长度超过18位", name.c_str());
            } else {
                field.setDataType(dataset::FTDOUBLE);
            }
        } else if(type == flowline::Field::T_DATETIME) {
            field.setDataType(dataset::FTSTRING);
            field.setLength(14);//YYYYMMDDhhmmss
            field.setTag(flowline::TAG::DATATIME);
        } else {
            field.setDataType(dataset::FTSTRING);
            field.setLength(length);
        }
        return field;
    }
};
}
#endif //_FIELD_H_
