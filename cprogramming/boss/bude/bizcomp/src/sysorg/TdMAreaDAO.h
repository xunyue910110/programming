#ifndef __TD_M_AREA_H_
#define __TD_M_AREA_H_

#include "udbi/udbi.h"
#include "udbi/MTable.h"

const string  SEL_IDX_TD_M_AREA_1("SEL_IDX_TD_M_AREA_1");
const string  SEL_IDX_TD_M_AREA_2("SEL_IDX_TD_M_AREA_2");
const string  SEL_IDX_TD_M_AREA_3("SEL_IDX_TD_M_AREA_3");
const string  SEL_PK_TD_M_AREA("SEL_PK_TD_M_AREA");
const string  SEL_AREA_0("SEL_AREA_0");
const string  SEL_AREA_1("SEL_AREA_1");
const string  SEL_AREA_2("SEL_AREA_2");
const string  SEL_AREA_4("SEL_AREA_4");
const string  SEL_AREA_3("SEL_AREA_3");
const string  SEL_AREA_5("SEL_AREA_5");
const string  UPD_IDX_TD_M_AREA_1("UPD_IDX_TD_M_AREA_1");
const string  UPD_IDX_TD_M_AREA_2("UPD_IDX_TD_M_AREA_2");
const string  UPD_IDX_TD_M_AREA_3("UPD_IDX_TD_M_AREA_3");
const string  UPD_PK_TD_M_AREA("UPD_PK_TD_M_AREA");
const string  UPD_UPD_AREA_0("UPD_UPD_AREA_0");
const string  UPD_DEL_AREA_0("UPD_DEL_AREA_0");
const string  DEL_IDX_TD_M_AREA_1("DEL_IDX_TD_M_AREA_1");
const string  DEL_IDX_TD_M_AREA_2("DEL_IDX_TD_M_AREA_2");
const string  DEL_IDX_TD_M_AREA_3("DEL_IDX_TD_M_AREA_3");
const string  DEL_PK_TD_M_AREA("DEL_PK_TD_M_AREA");
//const string  INSERT("INSERT");

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Area
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class Area {
public:
    Area();
    virtual ~Area();
    virtual void init();
public:
    string  AreaCode;
    string  AreaName;
    string  AreaFrame;
    string  ThisTag;
    string  UseTag;
    string  ExtendTag;
    int  OrderNo;
    string  UserAreaCode;
    string  ParentAreaCode;
    string  StartDate;
    string  EndDate;
    int  AreaLevel;
    string  Validflag;
    string  Remark;
    string  Rsvalue1;
    string  Rsvalue2;
    string  UpdateTime;
    string  UpdateStaffId;
    string  UpdateDepartId;
};

/** @brief Area对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<Area> VPArea;


/**=~===============SEGMENG4==========================*/

/** @class AreaDAO
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class AreaDAO : public MTable {
public:
     AreaDAO();
     virtual ~AreaDAO();
     VPArea jselect(const string &selBy, Area &selArea);
     VPArea jselby(const string &selBy,  Area &selArea);
     int jupdate(const string &updBy, Area &updArea);
     int jdelete(const string &delBy, Area &delArea);
     int jinsert(Area &insArea);
     int jinsby(const string &insBy, Area &insArea);
};

} // namespace SysMan

#endif
