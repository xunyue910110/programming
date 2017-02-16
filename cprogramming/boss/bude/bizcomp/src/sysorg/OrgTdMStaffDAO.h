#ifndef __ORG_TD_M_STAFF_H_
#define __ORG_TD_M_STAFF_H_

#include "udbi/udbi.h"
#include "TdMStaffDAO.h"
#include <string.h>

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class ORG_TdMStaff
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMStaff {
public:
    ORG_TdMStaff();
    virtual ~ORG_TdMStaff();
    virtual void ORG_init();
public:
    string  StaffId;
    string  DepartId;
    string  StaffName;
    string  JobInfo;
    string  ManagerInfo;
    string  Sex;
    string  Email;
    string  UserPid;
    string  SerialNumber;
    string  CustId;
    string  DimissionTag;
    string  Birthday;
    int  StaffGroupId;
    string  CustHobyy;
    string  Remark;
    string  Rsvalue1;
    string  Rsvalue2;
    string  UpdateTime;
    string  UpdateStaffId;
    string  UpdateDepartId;
    //add by liufei
    string	b_DepartId;
    string	b_Sex;
    string	b_DimissionTag;
    string	b_UpdateStaffId;
    string	b_UpdateDepartId;
    string	AreaCode;
    string	StartStaffId;
    string	EndStaffId;
    string	ValidFlag;
};

/** @brief ORG_TdMStaff对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<ORG_TdMStaff> VPORG_TdMStaff;


/**=~===============SEGMENG4==========================*/

/** @class ORG_TdMStaffDAO
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMStaffDAO : public StaffDAO {
public:
     ORG_TdMStaffDAO();
     virtual ~ORG_TdMStaffDAO();
     VPORG_TdMStaff ORG_jselect(const string &selBy, ORG_TdMStaff &selORG_TdMStaff);
     VPORG_TdMStaff ORG_jselby(const string &selBy,  ORG_TdMStaff &selORG_TdMStaff);
     int ORG_jupdate(const string &updBy, ORG_TdMStaff &updORG_TdMStaff);
     int ORG_jdelete(const string &delBy, ORG_TdMStaff &delORG_TdMStaff);
     int ORG_jinsert(ORG_TdMStaff &insORG_TdMStaff);
     int ORG_jinsby(const string &insBy, ORG_TdMStaff &insORG_TdMStaff);
};

} // namespace SysMan

#endif
