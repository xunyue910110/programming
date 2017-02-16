#include "CustDAO.h"
#include <iostream>

/** @class CustServAcct::Cust CustDAO.h 
 *  @brief CUST表的映射实体
 *  本对象映射数据库CUST表的所有字段为数据成员，并对静态数据缓存字段
 *  的有意义解释映射进行封装。
 *  S_STATIC_DATA_MAP 为数据库访问接口组件中封装的静态数据缓存。
 */
CustServAcct::Cust::Cust()
{
}

CustServAcct::Cust::~Cust()
{
}

/** @class CustServAcct::CustDAO CustDAO.h
 *  @brief CUST表的数据访问接口
 *  本对象对数据库CUST表的数据库访问进行封装，提供所有CUST表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
CustServAcct::CustDAO::CustDAO():MTable("CUST")
{
	//
}

CustServAcct::CustDAO::~CustDAO()
{
}


CustServAcct::PCust CustServAcct::CustDAO::jselect(const string &selBy, Cust &selCust)
{
    CustServAcct::PCust tmpPCust; 

    if (selBy==SEL_BY_PK) {
        SetParam(":VCUST_ID", selCust.cust_id);
        SetParam(":VCUST_SEQ_NBR", selCust.cust_seq_nbr);
    } else if (selBy == SEL_BY_NBR) {
        SetParam(":VSOCIAL_TYPE_ID", selCust.social_type_id);
        SetParam(":VSOCIAL_ID", selCust.social_id);
    } else if (selBy == SEL_BY_VIP) {
        SetParam(":VVIP_FLAG", selCust.vip_flag);
        SetParam(":VVIP_NBR", selCust.vip_nbr);
    }
    
    ExecSelect(selBy);
    while (Fetch()) {
        Cust *pCust = new Cust();
        pCust->cust_id = GetString("CUST_ID");
        pCust->cust_seq_nbr = GetInt("CUST_SEQ_NBR");
        pCust->cust_name = GetString("CUST_NAME");
        pCust->cust_type_id = GetInt("CUST_TYPE_ID");
        pCust->sex = GetString("SEX");
        pCust->social_type_id = GetInt("SOCIAL_TYPE_ID");
        pCust->social_id = GetString("SOCIAL_ID");
        pCust->vip_flag = GetInt("VIP_FLAG");
        pCust->vip_nbr = GetString("VIP_NBR");
        pCust->state = GetString("STATE");
        pCust->state_date = GetString("STATE_DATE");
		
		cout << GetString("CUST_NAME") << endl;
		
        tmpPCust.push_back(pCust);
        cout << "ok" << endl;
    }
    
    return tmpPCust;
}

int CustServAcct::CustDAO::jupdate(const string &updBy, Cust &updCust)
{
    if (updBy==UPD_NBR_BY_PK) {
        // 由cust_id，cust_seq_nbr更新social_type_id，social_id
        SetParam(":VCUST_ID", updCust.cust_id);
        SetParam(":VCUST_SEQ_NBR", updCust.cust_seq_nbr);
        SetParam(":VSOCIAL_TYPE_ID", updCust.social_type_id);
        SetParam(":VSOCIAL_ID", updCust.social_id);
    } else if (updBy==UPD_TYPE_BY_SOCIAL) {
        // 由social_type_id，social_id更新cust_type_id
        SetParam(":VSOCIAL_TYPE_ID", updCust.social_type_id);
        SetParam(":VSOCIAL_ID", updCust.social_id);
        SetParam(":VCUST_TYPE_ID", updCust.cust_type_id);
    }
    ExecUpdate(updBy);

    return 0;
}

int CustServAcct::CustDAO::jdelete(const string &delBy, Cust &delCust)
{
	return 0;
}

int CustServAcct::CustDAO::jinsert(Cust &insCust)
{
    SetParam(":VCUST_ID", insCust.cust_id);
    SetParam(":VCUST_SEQ_NBR", insCust.cust_seq_nbr);
    SetParam(":VCUST_TYPE_ID", insCust.cust_type_id);
    SetParam(":VSOCIAL_TYPE_ID", insCust.social_type_id);
    SetParam(":VSOCIAL_ID", insCust.social_id);
    SetParam(":VCUST_NAME", insCust.cust_name);
    // SetParam(":VSEX", insCust.sex);
    SetParam(":VVIP_FLAG", insCust.vip_flag);
    SetParam(":VVIP_NBR", insCust.vip_nbr);
    SetParam(":VSTATE", insCust.state);
    SetParam(":VSTATE_DATE", insCust.state_date);
    
    ExecInsert();

    return 0;
}


