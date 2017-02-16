#ifndef __TD_M_STAFF_H_
#define __TD_M_STAFF_H_

#include "config-all.h"
#include "udbi/udbi.h"


const string  SEL_IDX_TD_M_STAFF_1("SEL_IDX_TD_M_STAFF_1");
const string  SEL_IDX_TD_M_STAFF_2("SEL_IDX_TD_M_STAFF_2");
const string  SEL_PK_TD_M_STAFF("SEL_PK_TD_M_STAFF");
const string  SEL_STAFF_0("SEL_STAFF_0");
const string  SEL_STAFF_1("SEL_STAFF_1");
const string  SEL_STAFF_2("SEL_STAFF_2");
const string  SEL_STAFF_3("SEL_STAFF_3");
const string  SEL_STAFF_4("SEL_STAFF_4");
const string  SEL_STAFF_5("SEL_STAFF_5");
const string  SEL_STAFF_6("SEL_STAFF_6");
const string  SEL_STAFF_7("SEL_STAFF_7");
const string  UPD_IDX_TD_M_STAFF_1("UPD_IDX_TD_M_STAFF_1");
const string  UPD_IDX_TD_M_STAFF_2("UPD_IDX_TD_M_STAFF_2");
const string  UPD_PK_TD_M_STAFF("UPD_PK_TD_M_STAFF");
const string  UPD_UPD_STAFF_0("UPD_UPD_STAFF_0");
const string  UPD_DEL_STAFF_0("UPD_DEL_STAFF_0");
const string  DEL_IDX_TD_M_STAFF_1("DEL_IDX_TD_M_STAFF_1");
const string  DEL_IDX_TD_M_STAFF_2("DEL_IDX_TD_M_STAFF_2");
const string  DEL_PK_TD_M_STAFF("DEL_PK_TD_M_STAFF");
//const string  INSERT("INSERT");

/**=!===============SEGMENG2==========================*/
namespace SysMan {
	
using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Staff 
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class Staff {
public:
    Staff();
    virtual ~Staff();
    virtual void init();
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
};

/** @brief Staff对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<Staff> VPStaff;


/**=~===============SEGMENG4==========================*/

/** @class StaffDAO 
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class StaffDAO : public MTable {
public:
     StaffDAO();
     virtual ~StaffDAO();
     VPStaff jselect(const string &selBy, Staff &selStaff);
     VPStaff jselby(const string &selBy,  Staff &selStaff);
     int jupdate(const string &updBy, Staff &updStaff);
     int jdelete(const string &delBy, Staff &delStaff);
     int jinsert(Staff &insStaff);
     int jinsby(const string &insBy, Staff &insStaff);
};  
    
} // namespace SysMan

#endif
