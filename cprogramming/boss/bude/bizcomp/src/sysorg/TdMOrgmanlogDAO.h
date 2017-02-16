#ifndef __TD_M_ORGMANLOG_H_
#define __TD_M_ORGMANLOG_H_

#include "udbi/udbi.h"
#include "udbi/MTable.h"

const string  SEL_IDX_TD_M_ORGMANLOG_1("SEL_IDX_TD_M_ORGMANLOG_1");
const string  SEL_IDX_TD_M_ORGMANLOG_2("SEL_IDX_TD_M_ORGMANLOG_2");
const string  SEL_IDX_TD_M_ORGMANLOG_3("SEL_IDX_TD_M_ORGMANLOG_3");
const string  SEL_PK_TD_M_ORGMANLOG("SEL_PK_TD_M_ORGMANLOG");
const string  UPD_IDX_TD_M_ORGMANLOG_1("UPD_IDX_TD_M_ORGMANLOG_1");
const string  UPD_IDX_TD_M_ORGMANLOG_2("UPD_IDX_TD_M_ORGMANLOG_2");
const string  UPD_IDX_TD_M_ORGMANLOG_3("UPD_IDX_TD_M_ORGMANLOG_3");
const string  UPD_PK_TD_M_ORGMANLOG("UPD_PK_TD_M_ORGMANLOG");
const string  DEL_IDX_TD_M_ORGMANLOG_1("DEL_IDX_TD_M_ORGMANLOG_1");
const string  DEL_IDX_TD_M_ORGMANLOG_2("DEL_IDX_TD_M_ORGMANLOG_2");
const string  DEL_IDX_TD_M_ORGMANLOG_3("DEL_IDX_TD_M_ORGMANLOG_3");
const string  DEL_PK_TD_M_ORGMANLOG("DEL_PK_TD_M_ORGMANLOG");
//const string  INSERT("INSERT");

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class TdMOrgmanlog
 *  @brief TD_M_ORGMANLOG表的映射实体
 *  本对象映射数据库TD_M_ORGMANLOG表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class TdMOrgmanlog {
public:
    TdMOrgmanlog();
    virtual ~TdMOrgmanlog();
    virtual void init();
public:
    string  LogId;
    string  OperAreaCode;
    string  SubsysCode;
    string  OpertypeCode;
    string  StartValue1;
    string  EndValue1;
    string  StartValue2;
    string  EndValue2;
    string  StartValue3;
    string  EndValue3;
    string  OperInfo;
    string  Remark;
    string  Rsvalue1;
    string  Rsvalue2;
    string  OperDate;
    string  OperStaffId;
    string  OperDepartId;
};

/** @brief TdMOrgmanlog对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<TdMOrgmanlog> VPTdMOrgmanlog;


/**=~===============SEGMENG4==========================*/

/** @class TdMOrgmanlogDAO
 *  @brief TD_M_ORGMANLOG表的映射实体
 *  本对象映射数据库TD_M_ORGMANLOG表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class TdMOrgmanlogDAO : public MTable {
public:
     TdMOrgmanlogDAO();
     virtual ~TdMOrgmanlogDAO();
     VPTdMOrgmanlog jselect(const string &selBy, TdMOrgmanlog &selTdMOrgmanlog);
     VPTdMOrgmanlog jselby(const string &selBy,  TdMOrgmanlog &selTdMOrgmanlog);
     int jupdate(const string &updBy, TdMOrgmanlog &updTdMOrgmanlog);
     int jdelete(const string &delBy, TdMOrgmanlog &delTdMOrgmanlog);
     int jinsert(TdMOrgmanlog &insTdMOrgmanlog);
     int jinsby(const string &insBy, TdMOrgmanlog &insTdMOrgmanlog);
};

} // namespace SysMan

#endif
