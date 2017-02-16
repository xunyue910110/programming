#ifndef __PARAM_H_
#define __PARAM_H_

#include "udbi-inter.h"

namespace UDBI {
	
enum PARAM_TYPE { 
	PTYPE_INT=1, PTYPE_BUF=2, PTYPE_CSTR=3, PTYPE_STR=4, PTYPE_INT_ARRAY=5, PTYPE_STR_ARRAY=6 
};
                   
/** \class Param
 *  \brief SQL语句绑定参数数据结构
 */
struct Param {
    char paramName[60];
    PARAM_TYPE paramType;
    int reqType;
    void *pval;
    int valLen;
    int valArrayLen;
    OCIBind *bindhp;
};

} // namespace UDBI

#endif;
