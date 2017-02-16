#ifndef __TD_M_DEPART_H_
#define __TD_M_DEPART_H_

#include "udbi/udbi.h"
#include "udbi/MTable.h"

const string  SEL_IDX_TD_M_DEPART_1("SEL_IDX_TD_M_DEPART_1");
const string  SEL_IDX_TD_M_DEPART_2("SEL_IDX_TD_M_DEPART_2");
const string  SEL_IDX_TD_M_DEPART_3("SEL_IDX_TD_M_DEPART_3");
const string  SEL_IDX_TD_M_DEPART_4("SEL_IDX_TD_M_DEPART_4");
const string  SEL_PK_TD_M_DEPART("SEL_PK_TD_M_DEPART");
const string  SEL_DEPART_0("SEL_DEPART_0");
const string  SEL_DEPART_1("SEL_DEPART_1");
const string  SEL_DEPART_2("SEL_DEPART_2");
const string  SEL_DEPART_3("SEL_DEPART_3");
const string  SEL_DEPART_4("SEL_DEPART_4");
const string  SEL_DEPART_5("SEL_DEPART_5");
const string  SEL_DEPART_6("SEL_DEPART_6");
const string  SEL_DEPART_7("SEL_DEPART_7");
const string  SEL_DEPART_8("SEL_DEPART_8");
const string  SEL_DEPART_9("SEL_DEPART_9");
const string  SEL_DEPART_10("SEL_DEPART_10");
const string  SEL_DEPART_11("SEL_DEPART_11");
const string  UPD_IDX_TD_M_DEPART_1("UPD_IDX_TD_M_DEPART_1");
const string  UPD_IDX_TD_M_DEPART_2("UPD_IDX_TD_M_DEPART_2");
const string  UPD_IDX_TD_M_DEPART_3("UPD_IDX_TD_M_DEPART_3");
const string  UPD_IDX_TD_M_DEPART_4("UPD_IDX_TD_M_DEPART_4");
const string  UPD_PK_TD_M_DEPART("UPD_PK_TD_M_DEPART");
const string  UPD_UPD_DEPART_0("UPD_UPD_DEPART_0");
const string  UPD_DEL_DEPART_0("UPD_DEL_DEPART_0");
const string  DEL_IDX_TD_M_DEPART_1("DEL_IDX_TD_M_DEPART_1");
const string  DEL_IDX_TD_M_DEPART_2("DEL_IDX_TD_M_DEPART_2");
const string  DEL_IDX_TD_M_DEPART_3("DEL_IDX_TD_M_DEPART_3");
const string  DEL_IDX_TD_M_DEPART_4("DEL_IDX_TD_M_DEPART_4");
const string  DEL_PK_TD_M_DEPART("DEL_PK_TD_M_DEPART");
//const string  INSERT("INSERT");

/**=!===============SEGMENG2==========================*/
namespace SysMan {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class Depart
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class Depart {
public:
    Depart();
    virtual ~Depart();
    virtual void init();
public:
    string  DepartId;
    string  DepartCode;
    string  DepartName;
    string  DepartKindCode;
    string  DepartFrame;
    string  Validflag;
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
};

/** @brief Depart对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<Depart> VPDepart;


/**=~===============SEGMENG4==========================*/

/** @class DepartDAO
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class DepartDAO : public MTable {
public:
     DepartDAO();
     virtual ~DepartDAO();
     VPDepart jselect(const string &selBy, Depart &selDepart);
     VPDepart jselby(const string &selBy,  Depart &selDepart);
     int jupdate(const string &updBy, Depart &updDepart);
     int jdelete(const string &delBy, Depart &delDepart);
     int jinsert(Depart &insDepart);
     int jinsby(const string &insBy, Depart &insDepart);
};

} // namespace SysMan

#endif
