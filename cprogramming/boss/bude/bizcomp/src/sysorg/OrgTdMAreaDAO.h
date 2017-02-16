#ifndef __ORG_TD_M_AREA_H_
#define __ORG_TD_M_AREA_H_

#include "udbi/udbi.h"
#include "TdMAreaDAO.h"

//const string  INSERT("INSERT");

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class ORG_TdMArea
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMArea {
public:
    ORG_TdMArea();
    virtual ~ORG_TdMArea();
    virtual void ORG_init();
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
    string  ValidFlag;
    string  Remark;
    string  Rsvalue1;
    string  Rsvalue2;
    string  UpdateTime;
    string  UpdateStaffId;
    string  UpdateDepartId;

    //add by liufei 20050107
    string	b_ThisTag;
    string	b_UseTag;
    string	b_ExtendTag;
    string	b_ValidFlag;
    string	b_UpdateStaffId;
    string	b_UpdateDepartId;
};

/** @brief ORG_TdMArea对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<ORG_TdMArea> VPORG_TdMArea;


/**=~===============SEGMENG4==========================*/

/** @class ORG_TdMAreaDAO
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMAreaDAO : public AreaDAO {
public:
     ORG_TdMAreaDAO();
     virtual ~ORG_TdMAreaDAO();
     VPORG_TdMArea ORG_jselect(const string &selBy, ORG_TdMArea &selORG_TdMArea);
     VPORG_TdMArea ORG_jselby(const string &selBy,  ORG_TdMArea &selORG_TdMArea);
     int ORG_jupdate(const string &updBy, ORG_TdMArea &updORG_TdMArea);
     int ORG_jdelete(const string &delBy, ORG_TdMArea &delORG_TdMArea);
     int ORG_jinsert(ORG_TdMArea &insORG_TdMArea);
     int ORG_jinsby(const string &insBy, ORG_TdMArea &insORG_TdMArea);
};

} // namespace SysMan

#endif
