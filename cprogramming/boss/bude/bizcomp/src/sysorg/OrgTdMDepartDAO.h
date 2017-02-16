#ifndef __ORG_TD_M_DEPART_H_
#define __ORG_TD_M_DEPART_H_

#include "udbi/udbi.h"
#include "TdMDepartDAO.h"

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class ORG_TdMDepart
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMDepart {
public:
    ORG_TdMDepart();
    virtual ~ORG_TdMDepart();
    virtual void init();
public:
    string  DepartId;
    string  DepartCode;
    string  DepartName;
    string  DepartKindCode;
    string  DepartFrame;
    string  ValidFlag;
    string  AreaCode;
    string  ParentDepartId;
    string  ManagerId;
    int  OrderNo;
    string  UserDepartCode;
    string  StartDate;
    string  EndDate;
    int  DepartLevel;
    string  Remark;
    string  Rsvalue1;
    string  Rsvalue2;
    string  Rsvalue3;
    string  Rsvalue4;
    string  UpdateTime;
    string  UpdateStaffId;
    string  UpdateDepartId;

    //add by liufei
    string	b_DepartKindCode;
    string	b_ValidFlag;
    string	b_AreaCode;
    string	b_ParentDepartId;
    string	b_ManagerId;
    string	b_UpdateStaffId;
    string	b_UpdateDepartId;
};

/** @brief ORG_TdMDepart对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<ORG_TdMDepart> VPORG_TdMDepart;


/**=~===============SEGMENG4==========================*/

/** @class ORG_TdMDepartDAO
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class ORG_TdMDepartDAO : public DepartDAO {
public:
     ORG_TdMDepartDAO();
     virtual ~ORG_TdMDepartDAO();
     VPORG_TdMDepart ORG_jselect(const string &selBy, ORG_TdMDepart &selORG_TdMDepart);
     VPORG_TdMDepart ORG_jselby(const string &selBy,  ORG_TdMDepart &selORG_TdMDepart);
     int ORG_jupdate(const string &updBy, ORG_TdMDepart &updORG_TdMDepart);
     int ORG_jdelete(const string &delBy, ORG_TdMDepart &delORG_TdMDepart);
     int ORG_jinsert(ORG_TdMDepart &insORG_TdMDepart);
     int ORG_jinsby(const string &insBy, ORG_TdMDepart &insORG_TdMDepart);
};

} // namespace SysMan

#endif
