#ifndef SysUpdate_H
#define SysUpdate_H

#include <iostream>
#include <string.h>
#include <stdio.h>

#include "config-all.h"
#include "mwci/CFmlBuf.h"

using namespace std;

// 得到FTP服务器配置信息
extern "C" void E_GetFtpHost(CFmlBuf &InBuf, CFmlBuf &OutBuf);

// 得到更新模块信息
extern "C" void E_GetUpdateModInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf);

// 员工更新模块日志
extern "C" void E_StaffUpdateModLog(CFmlBuf &InBuf, CFmlBuf &OutBuf);

#endif
