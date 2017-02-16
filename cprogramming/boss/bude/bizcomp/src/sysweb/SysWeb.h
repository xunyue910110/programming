#ifndef SysWeb_H
#define SysWeb_H

#include <iostream>
#include <string.h>
#include <stdio.h>

#include "config-all.h"
#include "mwci/CFmlBuf.h"

#include "udbi/udbi.h"
#include "udbi/MTable.h"
#include "udbi/jdb.h"
#include "udbi/JConnect.h"

using namespace std;
using namespace UDBI;

// 插入WEB信息
extern "C" void AddWebInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf);

// 查询WEB信息
extern "C" void GetWebInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf);

//===================================WebAdd================================================
/**
 *  插入WEB信息，输入参数结构体，用于相关函数的输入参数
 *  罗颖 2005-5-11 20:55 Created
 */
struct SysWebAddInPara
{
    string strRecvEparchyCode;      // 接受消息员工地州编码
    string strRecvCityCode;         // 接受消息员工业务区编码
    string strRecvDepartId;         // 接受消息员工部门编码
    string strRecvStaffId;          // 接受消息员工工号

    string strTradeEparchyCode;     // 发送消息员工地州编码，用于记录日志
    string strTradeCityCode;        // 发送消息员工业务区编码，用于记录日志
    string strTradeDepartId;        // 发送消息员工部门编码，用于记录日志
    string strTradeStaffId;         // 发送消息员工工号，用于记录日志

    int    iInfoContainerType;      // 容器类型，暂时只有3 (1:公告箱 2:新闻箱 3:消息箱 4:收件箱 5:发件箱 6:草稿箱 7:废纸篓)
    int    iInfoType;               // 信息类型，暂时只有4 (1:公告 2:新闻 3:邮件 4:消息)
    string strInfoTopic;            // 信息主题
    string strInfoContent;          // 信息内容
    string strInfoAttach;           // 相关文档
} ;


/**
 *  插入WEB信息，输出参数结构体，用于相关函数的输出参数，暂时没用，预留
 *  罗颖 2005-5-11 20:55 Created
 */
struct SysWebAddOutPara
{
} ;

//==================================WebGet================================================
/**
 *  查询WEB信息，输入参数结构体，用于相关函数的输入参数
 *  罗颖 2005-5-11 20:55 Created
 */
struct SysWebGetInPara
{
    string strTradeEparchyCode; // 交易地州编码
    string strTradeCityCode;    // 交易业务区编码
    string strTradeDepartId;    // 交易部门编码
    string strTradeStaffId;     // 交易员工编码

    int    iGetMode;            // 查询信息方式，暂时只有3 (0:所有 1:公告箱 2:新闻箱 3:消息箱 4:收件箱 5:发件箱 6:草稿箱 7:废纸篓)
} ;

/**
 *  查询WEB信息，输出参数结构体，用于相关函数的输出参数
 *  罗颖 2005-5-11 20:55 Created
 */
struct SysWebGetOutPara
{
    int    iInfoCount;          // 信息记录数
    string strInfo;             // 信息串，格式待定
} ;

//==========================================================================================

class SysWeb
{
  public:
    /**
     * @desc    插入WEB信息
     * @auth    罗颖               2005-5-11 20:53
     * @param   sSysWebAddInPara   输入参数结构体　
     * @param   sSysWebAddOutPara  输出参数结构体　
     * @return  int                操作记录数，无实际意义，失败时抛出例外
     */
    int AddWebInfo(const SysWebAddInPara& sSysWebAddInPara, SysWebAddOutPara& sSysWebAddOutPara);

    /**
     * @desc    查询WEB信息
     * @auth    罗颖               2005-5-11 20:54
     * @param   sSysWebGetInPara   输入参数结构体　
     * @param   sSysWebGetOutPara  输出参数结构体　
     * @return  int                操作记录数，无实际意义，失败时抛出例外
     */
    int GetWebInfo(const SysWebGetInPara& sSysWebGetInPara, SysWebGetOutPara& sSysWebGetOutPara);
};

#endif
