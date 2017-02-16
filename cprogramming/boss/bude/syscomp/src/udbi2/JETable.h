/** 
 * @file JETable.h
 * SQL语句操作接口类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __JETABLE_H_
#define __JETABLE_H_

#include <string>
#include <vector>

#include "JTable.h"
#include "JCursor.h"

namespace UDBI {
using std::string;
using std::vector;

//! SQL语句操作接口类，封装参数设置、语句执行、查询结果访问功能
class JETable : public JTable {
public:

    virtual void BindParam(const string& par, int& val, short* isNull=0)=0;
    virtual void BindParam(const string& par, double& val, short* isNull=0)=0;
    virtual void BindParam(const string& par, char* buf, int buflen, short* isNull=0)=0;

    //! 2005.11.21 WangFJ.
    virtual void BindParam(const string& par, JCursor* pcur)=0;

};

} // namespace UDBI

#endif
