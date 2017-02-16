#ifndef __TF_F_CUSTOMER_H_
#define __TF_F_CUSTOMER_H_

#include "udbi/udbi.h"
#include "udbi/MTable.h"

const string  SEL_IDX_TF_F_CUSTOMER_CITYCODE("SEL_IDX_TF_F_CUSTOMER_CITYCODE");
const string  SEL_IDX_TF_F_CUSTOMER_CUSTNAME("SEL_IDX_TF_F_CUSTOMER_CUSTNAME");
const string  SEL_IDX_TF_F_CUSTOMER_PSPTID("SEL_IDX_TF_F_CUSTOMER_PSPTID");
const string  SEL_PK_TF_F_CUSTOMER("SEL_PK_TF_F_CUSTOMER");
const string  UPD_IDX_TF_F_CUSTOMER_CITYCODE("UPD_IDX_TF_F_CUSTOMER_CITYCODE");
const string  UPD_IDX_TF_F_CUSTOMER_CUSTNAME("UPD_IDX_TF_F_CUSTOMER_CUSTNAME");
const string  UPD_IDX_TF_F_CUSTOMER_PSPTID("UPD_IDX_TF_F_CUSTOMER_PSPTID");
const string  UPD_PK_TF_F_CUSTOMER("UPD_PK_TF_F_CUSTOMER");
const string  DEL_IDX_TF_F_CUSTOMER_CITYCODE("DEL_IDX_TF_F_CUSTOMER_CITYCODE");
const string  DEL_IDX_TF_F_CUSTOMER_CUSTNAME("DEL_IDX_TF_F_CUSTOMER_CUSTNAME");
const string  DEL_IDX_TF_F_CUSTOMER_PSPTID("DEL_IDX_TF_F_CUSTOMER_PSPTID");
const string  DEL_PK_TF_F_CUSTOMER("DEL_PK_TF_F_CUSTOMER");
//const string  INSERT("INSERT");
const string  INS_STAFF_0("INS_STAFF_0");

/**=!===============SEGMENG2==========================*/
namespace CustDom {

using namespace std;
using namespace UDBI;

/**=~===============SEGMENG3==========================*/

/** @class TfFCustomer
 *  @brief TF_F_CUSTOMER表的映射实体
 *  本对象映射数据库TF_F_CUSTOMER表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class TfFCustomer {
public:
    TfFCustomer();
    virtual ~TfFCustomer();
    virtual void init();
public:
    int  PartitionId;
    string  CustId;
    string  CustName;
    string  CustType;
    string  CustState;
    string  PsptTypeCode;
    string  PsptId;
    string  ReopenTag;
    string  EparchyCode;
    string  CityCode;
    string  CustPasswd;
    string  ScoreValue;
    int  CreditClass;
    string  BasicCreditValue;
    string  CreditValue;
    string  RemoveTag;
    string  RemoveDate;
    string  DevelopDepartId;
    string  DevelopStaffId;
    string  IndepartId;
    string  InStaffId;
    string  InDate;
    string  Remark;
    string  Prevaluec1;
    string  Prevaluec2;
    string  Prevaluec3;
    string  Prevaluec4;
    string  Prevaluec5;
    string  Prevaluec6;
    string  Prevaluec7;
    string  Prevaluec8;
    string  Prevaluec9;
    string  Prevaluec10;
};

/** @brief TfFCustomer对象指针的vector类型定义
 *  本向量类型定义主要用于查询时返回多条记录的情况，
 *  每条记录作为一个对象缓存在向量中
 */
typedef vector<TfFCustomer> VPTfFCustomer;


/**=~===============SEGMENG4==========================*/

/** @class TfFCustomerDAO
 *  @brief TF_F_CUSTOMER表的映射实体
 *  本对象映射数据库TF_F_CUSTOMER表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
class TfFCustomerDAO : public MTable {
public:
     TfFCustomerDAO();
     virtual ~TfFCustomerDAO();
     VPTfFCustomer jselect(const string &selBy, TfFCustomer &selTfFCustomer);
     VPTfFCustomer jselby(const string &selBy,  TfFCustomer &selTfFCustomer);
     int jupdate(const string &updBy, TfFCustomer &updTfFCustomer);
     int jdelete(const string &delBy, TfFCustomer &delTfFCustomer);
     int jinsert(TfFCustomer &insTfFCustomer);
     int jinsby(const string &insBy, TfFCustomer &insTfFCustomer);
};

} // namespace CustDom

#endif
