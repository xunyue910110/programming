/**
*   批价参数查询接口实现：MDB管理部分.
*/
#include "MdbParamManager.h"
#include "PM_Rate.h"
#include "RateConstDef.h"
#include "Mdb_UserTp.h"
#include "Mdb_UserTp_Param.h"
#include "Mdb_UserCondTp_Param.h"
//#include "Mdb_AccntInfo.h"
#include "Mdb_Acct_Unipay.h"
#include "Mdb_PayRelation_Acct.h"
#include "Mdb_PayRelation_All.h"
//#include "Mdb_UserInfo_VpnId.h"
#include "Mdb_UserInfo_VpnId_All.h"
//#include "Mdb_FeeChgLimit.h"
#include "Mdb_UserCondTp.h"
#include "NodeBase.h" 
#include "RateGlobalInfo.h"
//包月费相关
#include "Mdb_AdcECuserNum.h"
#include "Mdb_AdcECAbnormalState.h"
#include "Mdb_SpCdrInfo.h" 

/*bool PM_Rate::setMdbInfo(const MdbInfo &r_mdbInfo)
{
  if(m_pMdbManager->initialization(r_mdbInfo)==false)
  {
    return false;
  }
  return true;
}*/

//以下是从内存中取信息，返回值为int型
//-1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
//r_flag : true 正常  false 资料错误
int  PM_Rate::getUserTp(const T_SERVER_ID    &r_userId,
                        const T_OTHER_PARTY  &r_otherPary,
                        const T_DATE_TIME    &r_beginTime,
                              vector<UserTp> &r_userTpList,
                              bool           &r_flag,
                              char           r_idType)
{ //函数内不负责清空 r_userTpList
  Mdb_UserTp         t_po;
  vector<Mdb_UserTp> t_rpos;
  int                t_iRet=0,t_iRet2=0;
  bool               t_flag=true;
  r_flag = true;
  //1. 取用户资费实例
  t_po.m_record.m_userId = r_userId;
  t_po.m_record.m_idType = r_idType;
  t_iRet=m_pMdbManager->searchMulti<Mdb_UserTp>(t_po,t_rpos);
  if(t_iRet>0)
  {
    UserTp t_userTp;
    //vector<PO_UserTpParam_def*> t_userTpParamDefs;
    for(vector<Mdb_UserTp>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
    {
      t_userTp.m_tpObjId       = t_itr->m_record.m_tpObjId;
      t_userTp.m_tpId          = t_itr->m_record.m_tpId;
      t_userTp.m_beginTime     = t_itr->m_record.m_beginTime;
      t_userTp.m_endTime       = t_itr->m_record.m_endTime;
      t_userTp.m_idType        = t_itr->m_record.m_idType;
      t_userTp.m_userId        = t_itr->m_record.m_userId;
      t_userTp.m_productId     = t_itr->m_record.m_productId;
      t_userTp.m_servBund_Id   = t_itr->m_record.m_servBund_Id;
      t_userTp.m_servId        = t_itr->m_record.m_servId;
      t_userTp.m_feeBund_Id    = t_itr->m_record.m_feeBund_Id;
      t_userTp.m_relationVpnId = t_itr->m_record.m_relatin_vpnId;
      t_userTp.m_subTpType     = TP_TYPE_NORMAL;
      t_userTp.m_cellFlag      = FLAG_NO;
      t_userTp.m_vpnFlag       = VPN_FLAG_OUT;
      t_userTp.m_memberType_A  = VPN_MEMTYPE_NULL;
      t_userTp.m_memberType_B  = VPN_MEMTYPE_NULL;
      t_userTp.m_paramList.clear();
      /* 该为在资费在TPProc中求取
      //2.求取资费政策参数定义
      t_userTpParamDefs.clear();
      if(getUserTpParamDefInfo(t_itr->m_record.m_tpId,t_userTpParamDefs)==true)
      { //有用户资费参数定义，需要将用户资费参数实例求取出来 2009-3-21 10:42
        //3.求取用户资费参数实例
        if(getUserTpParam(t_userTp.m_tpObjId,t_userTp.m_paramList)<0)
        {
          //查找错误
          t_iRet2 = -1;
          return t_iRet2;
        }
        for(vector<PO_UserTpParam_def*>::iterator r_paramDefItr=t_userTpParamDefs.begin();
            r_paramDefItr!=t_userTpParamDefs.end();++r_paramDefItr)
        {
          t_flag = false;
          //4.根据资费政策参数定义设置用户资费参数实例一些属性
          for(vector<UserTpParam>::iterator r_paramItr=t_userTp.m_paramList.begin();
              r_paramItr!=t_userTp.m_paramList.end();++r_paramItr)
          {
            if(r_paramItr->m_tpParamId == (*r_paramDefItr)->m_record.m_tpParamId )
            {
              t_flag = true;
              r_paramItr->m_relatinType = (*r_paramDefItr)->m_record.m_relatinType;
              r_paramItr->m_valueType   = (*r_paramDefItr)->m_record.m_valueType;
              break;
            }else continue;
          }
          if(t_flag == false)
          { //用户资费参数错误(缺资费参数资料)
            r_flag = false;
            return t_iRet2;
          }
        }
      }
      */
      //5.如果是关联群，则设置计费方和对方角色
      if(t_userTp.m_relationVpnId!=NULL_VPN_ID)
      {
        T_SERVER_ID t_lastVpnId;
        list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr1,t_vpnInfoItr2;
        //Commented By Sunam 2010/1/14 16:29:11
        /*
        list<Mdb_VpnInfo_UserId> t_vpnInfoList1,t_vpnInfoList2;
        t_vpnInfoList1.clear();t_vpnInfoList2.clear();
        t_iRet2 = getVpnInfoByUserId(t_userTp.m_userId,
                                    VPN_MEMIDTYPE_USER,
                                    r_beginTime,
                                    VPN_MEMTYPE_NULL,
                                    t_lastVpnId,
                                    t_vpnInfoList1);
        if(t_iRet2<0) return t_iRet2;
        t_iRet2 = getVpnInfoByNumber(r_otherPary,
                                     VPN_MEMIDTYPE_USER,
                                     r_beginTime,
                                     VPN_MEMTYPE_NULL,
                                     t_vpnInfoList2);
        if(t_iRet2<0) return t_iRet2;
        */
        
        //Modified By Sunam 2010/1/14 16:30:11
        //for(t_vpnInfoItr1=t_vpnInfoList1.begin();t_vpnInfoItr1!=t_vpnInfoList1.end();++t_vpnInfoItr1)
        for(t_vpnInfoItr1 =NodeBase::m_globalInfo->m_vpnInfoList_a.begin();
            t_vpnInfoItr1!=NodeBase::m_globalInfo->m_vpnInfoList_a.end();
            ++t_vpnInfoItr1)
        {
          if(t_userTp.m_relationVpnId== t_vpnInfoItr1->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr1->m_record.m_beginTime ))
          {
            t_userTp.m_memberType_A = t_vpnInfoItr1->m_record.m_memberType;
			  // modified by chenm 2009-4-15 8:58:10
			  //t_vpnId_A               = t_vpnInfoItr1->m_record.m_lastVpnId;
			  t_userTp.m_lastVpnParentIDA = t_vpnInfoItr1->m_record.m_lastVpnId;
			  // over 2009-4-15 8:58:10
            break;
          }
        }
        //Modified By Sunam 2010/1/14 16:31:00
        //for(t_vpnInfoItr2 =t_vpnInfoList2.begin();t_vpnInfoItr2!=t_vpnInfoList2.end();++t_vpnInfoItr2)
        for(t_vpnInfoItr2 =NodeBase::m_globalInfo->m_vpnInfoList_b.begin();
            t_vpnInfoItr2!=NodeBase::m_globalInfo->m_vpnInfoList_b.end();
            ++t_vpnInfoItr2)
        {
          if( t_userTp.m_relationVpnId==t_vpnInfoItr2->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr2->m_record.m_beginTime ))
          {
            // modified by chenm 2009-4-15 10:45:10
            //if(t_vpnId_A == t_vpnInfoItr2->m_record.m_lastVpnId)
            t_userTp.m_lastVpnParentIDB = t_vpnInfoItr2->m_record.m_lastVpnId;
            if(t_userTp.m_lastVpnParentIDA == t_vpnInfoItr2->m_record.m_lastVpnId)	
            // over 2009-4-15 10:45:18
            {
              t_userTp.m_vpnFlag = VPN_FLAG_SMALL;
            }else
            {
              t_userTp.m_vpnFlag = VPN_FLAG_BIG;
            }
            t_userTp.m_memberType_B = t_vpnInfoItr2->m_record.m_memberType;
            break;
          }
        }
      }
      r_userTpList.push_back(t_userTp);
    }
  }

  //二:取用户后台绑定的资费实例
  int t_iRet1=0;
  t_iRet2=0;
  
  Mdb_UserCondTp         t_poCond;
  vector<Mdb_UserCondTp> t_rpoConds;  
  t_poCond.m_record.m_userId = r_userId;
  t_poCond.m_record.m_idType = r_idType;
  t_iRet1=m_pMdbManager->searchMulti<Mdb_UserCondTp>(t_poCond,t_rpoConds);
  if(t_iRet1>0)
  {
    UserTp t_userTp;
    for(vector<Mdb_UserCondTp>::iterator t_itrCond=t_rpoConds.begin();
        t_itrCond!=t_rpoConds.end();++t_itrCond)
    {
      t_userTp.m_tpObjId       = t_itrCond->m_record.m_tpObjId;
      t_userTp.m_tpId          = t_itrCond->m_record.m_tpId;
      t_userTp.m_beginTime     = t_itrCond->m_record.m_beginTime;
      t_userTp.m_endTime       = t_itrCond->m_record.m_endTime;
      t_userTp.m_idType        = t_itrCond->m_record.m_idType;
      t_userTp.m_userId        = t_itrCond->m_record.m_userId;
      t_userTp.m_productId     = 0;
      t_userTp.m_servBund_Id   = 0;
      t_userTp.m_servId        = 0;
      t_userTp.m_feeBund_Id    = 0;
      t_userTp.m_relationVpnId = t_itrCond->m_record.m_relatin_vpnId;
      t_userTp.m_subTpType     = TP_TYPE_NORMAL;
      t_userTp.m_cellFlag      = FLAG_NO;
      t_userTp.m_vpnFlag       = VPN_FLAG_OUT;
      t_userTp.m_memberType_A  = VPN_MEMTYPE_NULL;
      t_userTp.m_memberType_B  = VPN_MEMTYPE_NULL;
      t_userTp.m_paramList.clear();
      
      //5.如果是关联群，则设置计费方和对方角色
      if(t_userTp.m_relationVpnId!=NULL_VPN_ID)
      {
        T_SERVER_ID t_lastVpnId;
        list<Mdb_VpnInfo_UserId> t_vpnInfoList1,t_vpnInfoList2;
        list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr1,t_vpnInfoItr2;
        t_vpnInfoList1.clear();t_vpnInfoList2.clear();
        t_iRet2 = getVpnInfoByUserId(t_userTp.m_userId,
                                    VPN_MEMIDTYPE_USER,
                                    r_beginTime,
                                    VPN_MEMTYPE_NULL,
                                    t_lastVpnId,
                                    t_vpnInfoList1);
        if(t_iRet2<0) return t_iRet2;
        t_iRet2 = getVpnInfoByNumber(r_otherPary,
                                     VPN_MEMIDTYPE_USER,
                                     r_beginTime,
                                     VPN_MEMTYPE_NULL,
                                     t_vpnInfoList2);
        if(t_iRet2<0) return t_iRet2;
        
        for(t_vpnInfoItr1=t_vpnInfoList1.begin();
            t_vpnInfoItr1!=t_vpnInfoList1.end();
            ++t_vpnInfoItr1)
        {
          if(t_userTp.m_relationVpnId== t_vpnInfoItr1->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr1->m_record.m_beginTime ))
          {
            t_userTp.m_memberType_A = t_vpnInfoItr1->m_record.m_memberType;
			  // modified by chenm 2009-4-15 8:58:10
			  //t_vpnId_A               = t_vpnInfoItr1->m_record.m_lastVpnId;
			  t_userTp.m_lastVpnParentIDA = t_vpnInfoItr1->m_record.m_lastVpnId;
			  // over 2009-4-15 8:58:10
            break;
          }
        }
        for(t_vpnInfoItr2 =t_vpnInfoList2.begin();
            t_vpnInfoItr2!=t_vpnInfoList2.end();
            ++t_vpnInfoItr2)
        {
          if( t_userTp.m_relationVpnId==t_vpnInfoItr2->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr2->m_record.m_beginTime ))
          {
            // modified by chenm 2009-4-15 10:45:10
            //if(t_vpnId_A == t_vpnInfoItr2->m_record.m_lastVpnId)
            t_userTp.m_lastVpnParentIDB = t_vpnInfoItr2->m_record.m_lastVpnId;
            if(t_userTp.m_lastVpnParentIDA == t_vpnInfoItr2->m_record.m_lastVpnId)	
            // over 2009-4-15 10:45:18
        {
              t_userTp.m_vpnFlag = VPN_FLAG_SMALL;
            }else
          {
              t_userTp.m_vpnFlag = VPN_FLAG_BIG;
            }
            t_userTp.m_memberType_B = t_vpnInfoItr2->m_record.m_memberType;
            break;
          }
        }
      }
      r_userTpList.push_back(t_userTp);
    }
  }  
  
  t_iRet = t_iRet+t_iRet1;
  
  return t_iRet;
}

void  PM_Rate::getUserCondTp(const T_SERVER_ID     &r_userId,     
                   			 const T_OTHER_PARTY   &r_otherPary,  
                   			 const T_DATE_TIME     &r_beginTime,  
                         	 vector<UserTp>  	   &r_userTpList,      
                         	 char            	   r_idType)
{
  Mdb_UserCondTp         t_po;
  vector<Mdb_UserCondTp> t_rpos;
  int                t_iRet=0,t_iRet2=0;
  bool               t_flag=true;
  
  //1. 取用户资费实例
  t_po.m_record.m_userId = r_userId;
  t_po.m_record.m_idType = r_idType;
  t_iRet=m_pMdbManager->searchMulti<Mdb_UserCondTp>(t_po,t_rpos);
  if(t_iRet>0)
  {
    UserTp t_userTp;
    
    for(vector<Mdb_UserCondTp>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
    {
      t_userTp.m_tpObjId       = t_itr->m_record.m_tpObjId;
      t_userTp.m_tpId          = t_itr->m_record.m_tpId;
      t_userTp.m_beginTime     = t_itr->m_record.m_beginTime;
      t_userTp.m_endTime       = t_itr->m_record.m_endTime;
      t_userTp.m_idType        = t_itr->m_record.m_idType;
      t_userTp.m_userId        = t_itr->m_record.m_userId;
      t_userTp.m_productId     = 0;
      t_userTp.m_servBund_Id   = 0;
      t_userTp.m_servId        = 0;
      t_userTp.m_feeBund_Id    = 0;
      t_userTp.m_relationVpnId = t_itr->m_record.m_relatin_vpnId;
      t_userTp.m_subTpType     = TP_TYPE_NORMAL;
      t_userTp.m_cellFlag      = FLAG_NO;
      t_userTp.m_vpnFlag       = VPN_FLAG_OUT;
      t_userTp.m_memberType_A  = VPN_MEMTYPE_NULL;
      t_userTp.m_memberType_B  = VPN_MEMTYPE_NULL;
      t_userTp.m_paramList.clear();


      if(t_userTp.m_relationVpnId!=NULL_VPN_ID)
      {
        T_SERVER_ID t_lastVpnId;
        list<Mdb_VpnInfo_UserId> t_vpnInfoList1,t_vpnInfoList2;
        list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr1,t_vpnInfoItr2;
        t_vpnInfoList1.clear();t_vpnInfoList2.clear();
        t_iRet2 = getVpnInfoByUserId(t_userTp.m_userId,
                                    VPN_MEMIDTYPE_USER,
                                    r_beginTime,
                                    VPN_MEMTYPE_NULL,
                                    t_lastVpnId,
                                    t_vpnInfoList1);
        if(t_iRet2<0) return ;
        t_iRet2 = getVpnInfoByNumber(r_otherPary,
                                     VPN_MEMIDTYPE_USER,
                                     r_beginTime,
                                     VPN_MEMTYPE_NULL,
                                     t_vpnInfoList2);
        if(t_iRet2<0) return ;
        
        for(t_vpnInfoItr1=t_vpnInfoList1.begin();
            t_vpnInfoItr1!=t_vpnInfoList1.end();
            ++t_vpnInfoItr1)
        {
          if(t_userTp.m_relationVpnId== t_vpnInfoItr1->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr1->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr1->m_record.m_beginTime ))
          {
            t_userTp.m_memberType_A = t_vpnInfoItr1->m_record.m_memberType;
			t_userTp.m_lastVpnParentIDA = t_vpnInfoItr1->m_record.m_lastVpnId;
            break;
          }
        }
        for(t_vpnInfoItr2 =t_vpnInfoList2.begin();
            t_vpnInfoItr2!=t_vpnInfoList2.end();
            ++t_vpnInfoItr2)
        {
          if( t_userTp.m_relationVpnId==t_vpnInfoItr2->m_record.m_userId &&
             (t_userTp.m_beginTime   >= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_endTime ||
              t_userTp.m_beginTime   <= t_vpnInfoItr2->m_record.m_beginTime &&
              t_userTp.m_endTime     >= t_vpnInfoItr2->m_record.m_beginTime ))
          {
            t_userTp.m_lastVpnParentIDB = t_vpnInfoItr2->m_record.m_lastVpnId;
            if(t_userTp.m_lastVpnParentIDA == t_vpnInfoItr2->m_record.m_lastVpnId)	
        {
              t_userTp.m_vpnFlag = VPN_FLAG_SMALL;
            }else
          {
              t_userTp.m_vpnFlag = VPN_FLAG_BIG;
            }
            t_userTp.m_memberType_B = t_vpnInfoItr2->m_record.m_memberType;
            break;
          }
        }
      }
      
      r_userTpList.push_back(t_userTp);
    }
  }
}

  //取用户参数信息
int  PM_Rate:: getUserTpParam(const T_TPOBJ_ID          &r_tpObjId,
                                    vector<UserTpParam> &r_userTpParamList)
{
  int t_iRet=0;
  Mdb_UserTp_Param t_po;
  vector<Mdb_UserTp_Param> t_rpos;
  t_po.m_record.m_tpObjId = r_tpObjId;
  t_iRet=m_pMdbManager->searchMulti<Mdb_UserTp_Param>(t_po,t_rpos);
  if(t_iRet<0) return t_iRet;
  r_userTpParamList.clear();  
  UserTpParam  t_userTpParam;
  if (t_rpos.size() == 0)
  {
  	Mdb_UserCondTp_Param t_poCond;
  	vector<Mdb_UserCondTp_Param> t_rpoConds;
  	t_poCond.m_record.m_tpObjId = r_tpObjId;
  	t_iRet=m_pMdbManager->searchMulti<Mdb_UserCondTp_Param>(t_poCond,t_rpoConds);
  	if(t_iRet<0) return t_iRet;
	  for(vector<Mdb_UserCondTp_Param>::iterator t_itrCond=t_rpoConds.begin();
	      t_itrCond!=t_rpoConds.end(); ++t_itrCond)
	  {
	    t_userTpParam.m_tpObjId = t_itrCond->m_record.m_tpObjId;
	    t_userTpParam.m_tpParamId = t_itrCond->m_record.m_tpParamId;
	    t_userTpParam.m_tpValue = t_itrCond->m_record.m_tpValue;
	    t_userTpParam.m_beginTime = t_itrCond->m_record.m_beginTime;
	    t_userTpParam.m_endTime = t_itrCond->m_record.m_endTime;
	    r_userTpParamList.push_back(t_userTpParam);
	  }  	
	}
	else
	{
	  for(vector<Mdb_UserTp_Param>::iterator t_itr=t_rpos.begin();
	      t_itr!=t_rpos.end();++t_itr)
	  {
	    t_userTpParam.m_tpObjId = t_itr->m_record.m_tpObjId;
	    t_userTpParam.m_tpParamId = t_itr->m_record.m_tpParamId;
	    t_userTpParam.m_tpValue = t_itr->m_record.m_tpValue;
	    t_userTpParam.m_beginTime = t_itr->m_record.m_beginTime;
	    t_userTpParam.m_endTime = t_itr->m_record.m_endTime;
	    r_userTpParamList.push_back(t_userTpParam);
	  }
	}
  return t_iRet;
}
                



//根据计费方用户标识和对方号码取对应的用户群套餐
int  PM_Rate::getUserVpnTp(const T_SERVER_ID     &r_userId,
                           const T_OTHER_PARTY   &r_otherPary,
                           const T_DATE_TIME     &r_beginTime,
                                 vector<UserTp>  &r_userTpList)
{
  int t_iRet=0,t_resultSize=0,t_vpn_flag;
  T_MEMBER_TYPE t_memberType_a,t_memberType_b;
  T_SERVER_ID   t_lastVpnId;
  bool t_flag = true;
  
  //Commented By Sunam 2010/1/14 16:23:23
  //1. 根据计费方用户标识，取群标识
  /*
  list<Mdb_VpnInfo_UserId>   t_vpnInfoList_a;
  t_iRet = getVpnInfoByUserId(r_userId,VPN_MEMIDTYPE_USER,r_beginTime,
                              VPN_MEMTYPE_NULL,t_lastVpnId,t_vpnInfoList_a);
  if(t_iRet<0) return t_iRet;
  
  list<Mdb_VpnInfo_UserId>   t_vpnInfoList_b;
  t_iRet = getVpnInfoByNumber(r_otherPary,VPN_MEMIDTYPE_USER,r_beginTime,
                              VPN_MEMTYPE_NULL,t_vpnInfoList_b);
  if(t_iRet<0) return t_iRet;
  */
  //Modified By Sunam 2010/1/14 16:23:34
  //for(list<Mdb_VpnInfo_UserId>::iterator t_vpnItr1=t_vpnInfoList_a.begin();
  //    t_vpnItr1!=t_vpnInfoList_a.end();++t_vpnItr1)
  for(list<Mdb_VpnInfo_UserId>::iterator t_vpnItr1=NodeBase::m_globalInfo->m_vpnInfoList_a.begin();
      t_vpnItr1!=NodeBase::m_globalInfo->m_vpnInfoList_a.end();++t_vpnItr1)
  {
    t_memberType_a = t_vpnItr1->m_record.m_lastMemberType;
    t_resultSize   = r_userTpList.size();
    //根据VPNID取用户套餐
    t_iRet = getUserTp(t_vpnItr1->m_record.m_userId,r_otherPary,r_beginTime,r_userTpList,t_flag);
    if(t_iRet<0) return t_iRet;
    t_memberType_b = VPN_MEMTYPE_NULL;
    t_vpn_flag     = VPN_FLAG_OUT;
    //Modified By Sunam 2010/1/14 16:26:40
    //for(list<Mdb_VpnInfo_UserId>::const_iterator t_vpnItr3=t_vpnInfoList_a.begin();
    //    t_vpnItr3!=t_vpnInfoList_a.end();++t_vpnItr3)
    for(list<Mdb_VpnInfo_UserId>::const_iterator t_vpnItr3=NodeBase::m_globalInfo->m_vpnInfoList_a.begin();
        t_vpnItr3!=NodeBase::m_globalInfo->m_vpnInfoList_a.end();++t_vpnItr3)
    {
      if(!(t_vpnItr3->m_record.m_lastVpnId == t_vpnItr1->m_record.m_lastVpnId))
      { //不是该小集团的不能算
        continue;
      }
      //Modified By Sunam 2010/1/14 16:24:20
      //for(list<Mdb_VpnInfo_UserId>::const_iterator t_vpnItr2=t_vpnInfoList_b.begin();
      //  t_vpnItr2!=t_vpnInfoList_b.end();++t_vpnItr2)
      for(list<Mdb_VpnInfo_UserId>::const_iterator t_vpnItr2=NodeBase::m_globalInfo->m_vpnInfoList_b.begin();
          t_vpnItr2!=NodeBase::m_globalInfo->m_vpnInfoList_b.end();++t_vpnItr2)
    {
      if(t_vpnItr1->m_record.m_userId == t_vpnItr2->m_record.m_userId)
      { //在一个群内，则设置群标识
          if(t_vpnItr1->m_record.m_lastVpnId == t_vpnItr2->m_record.m_lastVpnId)
          {
        t_memberType_b = t_vpnItr2->m_record.m_lastMemberType;
            t_vpn_flag = VPN_FLAG_SMALL;
            break;
          }else if(t_vpn_flag == VPN_FLAG_OUT)
          {
            t_memberType_b = t_vpnItr2->m_record.m_lastMemberType;
            t_vpn_flag = VPN_FLAG_BIG;
      }
    }
      }
      if(t_vpn_flag == VPN_FLAG_SMALL) break;
    }
    for(int t_i=t_resultSize;t_i<r_userTpList.size();++t_i)
    {
      r_userTpList[t_i].m_memberType_A = t_memberType_a;
      //集团间的资费，在 tf_f_feepolicy 中的 rela_user_id 中填写对方的集团ID
      //在getUserTp中根据 rela_user_id 可以获取对方的角色类型，这里不在重置
      if (r_userTpList[t_i].m_memberType_B == VPN_MEMTYPE_NULL)
        r_userTpList[t_i].m_memberType_B = t_memberType_b;
      r_userTpList[t_i].m_subTpType    = TP_TYPE_VPN;
      r_userTpList[t_i].m_vpnFlag      = t_vpn_flag;
    }
  }
  return t_resultSize;
}

//根据用户标识,取群记录信息
int PM_Rate::getVpnInfoByUserId(const T_SERVER_ID     &r_userId,
                                const T_MEMBER_TYPE   &r_idType,
                                const T_DATE_TIME     &r_beginTime,
                                const T_MEMBER_TYPE   &r_memberType,
                                const T_SERVER_ID     &r_lastVpnId,
                                      list<Mdb_VpnInfo_UserId> &r_vpnInfoList)
{
  Mdb_VpnInfo_UserId         t_po;
  vector<Mdb_VpnInfo_UserId> t_rpos;
  int t_iRet=0,t_iRet2=0;
  //static int s_loopTimes=0;
  
  //++s_loopTimes;
  //if(s_loopTimes >= 100 ) {--s_loopTimes;return -1;}
  
  t_rpos.clear();
  t_po.m_record.m_memberId     = r_userId;
  t_po.m_record.m_memberIdType = r_idType;
  t_iRet=m_pMdbManager->searchMulti<Mdb_VpnInfo_UserId>(t_po,t_rpos);
  //cout<<t_iRet<<endl;
  if(t_iRet>0)
  {
    for(vector<Mdb_VpnInfo_UserId>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
    {
      if(t_itr->m_record.m_beginTime > r_beginTime ||
         t_itr->m_record.m_endTime   < r_beginTime)
      {
        continue;
      }
      if(r_memberType == VPN_MEMTYPE_NULL)
      {
        t_itr->m_record.m_lastMemberType = t_itr->m_record.m_memberType;
        t_itr->m_record.m_lastVpnId      = t_itr->m_record.m_userId;
      }else
      {
        t_itr->m_record.m_lastMemberType=r_memberType;
        t_itr->m_record.m_lastVpnId     = r_lastVpnId;
      }
      r_vpnInfoList.push_back(*t_itr);
      t_iRet2=getVpnInfoByUserId(t_itr->m_record.m_userId,
                                VPN_MEMIDTYPE_USER,r_beginTime,
                                t_itr->m_record.m_lastMemberType,
                                t_itr->m_record.m_lastVpnId,
                                r_vpnInfoList);
      if(t_iRet2<0){/*--s_loopTimes;*/ return t_iRet2;}
    }
  }
  //--s_loopTimes;
  return t_iRet;
}
//根据号码,取群记录信息
//Modified By Sunam 2010/1/14 16:35:17
/*
int PM_Rate::getVpnInfoByNumber(const T_OTHER_PARTY   &r_otherPary,
                                const T_MEMBER_TYPE   &r_idType,
                                const T_DATE_TIME     &r_beginTime,
                                const T_MEMBER_TYPE   &r_memberType,
                                      list<Mdb_VpnInfo_UserId> &r_vpnInfoList)
*/
int PM_Rate::getVpnInfoByNumber(const T_OTHER_PARTY   &r_otherPary,
                                const T_MEMBER_TYPE   &r_idType,
                                const T_DATE_TIME     &r_beginTime,
                                const T_MEMBER_TYPE   &r_memberType,
                                      list<Mdb_VpnInfo_UserId> &r_vpnInfoList,
                                            bool      r_other_needLoop)
{
  Mdb_VpnInfo_Number         t_po;
  vector<Mdb_VpnInfo_UserId> t_rpos;
  Mdb_VpnInfo_UserId         t_vpnInfo_userId;
  int t_iRet=0;
  T_OTHER_PARTY  t_tmpOtherParty;
  T_SERVER_ID    t_subVpnId;
  //Modified By Sunam 2010/1/14 16:37:03
  //if(isMobilePhone(r_otherPary.c_str()))
  if(isMobilePhone(r_otherPary.c_str()) || (r_other_needLoop==false))
  {
    //江苏联通有总机（手机号码）未配全，需特殊处理下 2009-3-3 16:40 gaojf
    // return getVpnInfoByNumberEqual(r_otherPary,r_idType,r_beginTime,
    //                                r_memberType,t_subVpnId,r_vpnInfoList);
    if(strncmp(r_otherPary.c_str(),"132516",6)==0 ||
       strncmp(r_otherPary.c_str(),"132516",6)==0 )
    {
      t_iRet = getVpnInfoByNumberEqual(r_otherPary,r_idType,r_beginTime,
                                       r_memberType,t_subVpnId,r_vpnInfoList);
      if(t_iRet<0) return t_iRet;
      t_tmpOtherParty = r_otherPary.substr(0,6);
      t_iRet = getVpnInfoByNumberEqual(t_tmpOtherParty,r_idType,r_beginTime,
                                       r_memberType,t_subVpnId,r_vpnInfoList);
    }else
    {
      return getVpnInfoByNumberEqual(r_otherPary,r_idType,r_beginTime,
                                     r_memberType,t_subVpnId,r_vpnInfoList);
    }
  }else
  {
    //T_OTHER_PARTY  t_tmpOtherParty;
    for(int t_i=r_otherPary.length();t_i>=3;--t_i)
    {
      t_tmpOtherParty = r_otherPary.substr(0,t_i);
      t_iRet = getVpnInfoByNumberEqual(t_tmpOtherParty,r_idType,r_beginTime,
                                       r_memberType,t_subVpnId,r_vpnInfoList);
      if(t_iRet<0) return t_iRet;
    }
  }
  return r_vpnInfoList.size();
}

//完全匹配方式：根据号码,取群记录信息
int PM_Rate::getVpnInfoByNumberEqual(const T_OTHER_PARTY   &r_otherPary,
                                     const T_MEMBER_TYPE   &r_idType,
                                     const T_DATE_TIME     &r_beginTime,
                                     const T_MEMBER_TYPE   &r_memberType,
                                     const T_SERVER_ID     &r_lastVpnId,
                                           list<Mdb_VpnInfo_UserId> &r_vpnInfoList)
{
  Mdb_VpnInfo_Number         t_po;
  vector<Mdb_VpnInfo_Number> t_rpos;
  Mdb_VpnInfo_UserId         t_vpnInfo_userId;
  int t_iRet=0,t_iRet2=0;
  
  t_rpos.clear();
  t_po.m_record.m_memberNumber = r_otherPary;
  t_po.m_record.m_memberIdType = r_idType;
  t_iRet=m_pMdbManager->searchMulti<Mdb_VpnInfo_Number>(t_po,t_rpos);
  if(t_iRet>0)
  {
    for(vector<Mdb_VpnInfo_Number>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
    {
      if(t_itr->m_record.m_beginTime > r_beginTime ||
         t_itr->m_record.m_endTime   < r_beginTime)
      {
        continue;
      }
      if(r_memberType == VPN_MEMTYPE_NULL)
      {
        t_itr->m_record.m_lastMemberType = t_itr->m_record.m_memberType;
        t_itr->m_record.m_lastVpnId      = t_itr->m_record.m_userId;
      }else
      {
        t_itr->m_record.m_lastMemberType=r_memberType;
        t_itr->m_record.m_lastVpnId     =r_lastVpnId;
      }
      t_vpnInfo_userId = *t_itr;
      r_vpnInfoList.push_back(t_vpnInfo_userId);
      t_iRet2=getVpnInfoByUserId(t_itr->m_record.m_userId,
                                VPN_MEMIDTYPE_USER,r_beginTime,
                                t_itr->m_record.m_lastMemberType,
                                t_itr->m_record.m_lastVpnId,
                                r_vpnInfoList);
      if(t_iRet2<0) return (t_iRet+t_iRet2);
    }
  }
  return t_iRet;  
}

///*2008-10-24 9:26:45
// * 根据帐户ID，取对应的帐户信息 
// * @param r_acctId     帐户Id
// * @param r_acctInfo   帐户信息
// * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
// */           
//int PM_Rate::getAcctInfo(const T_SERVER_ID &r_acctId,
//                               IdInfo      &r_acctInfo)
//{
//  int t_iRet=0;
//  
//  Mdb_AccntInfo t_po,t_rpo;
//  
//  t_po.m_record.m_idAttr[0] = r_acctId;
//  t_iRet=m_pMdbManager->search<Mdb_AccntInfo>(t_po,t_rpo);
//  if(t_iRet<=0) return t_iRet;
//  r_acctInfo = t_rpo.m_record;
//  return t_iRet;
//}

/*
 * 根据三户ID，取对应的帐前调帐
 * @param r_id              三户ID
 * @param r_idType          ID类型
 * @param r_cycId           帐期
 * @param r_adjustBefores   调帐信息
 * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */           
int PM_Rate::getAdjustBefore(const T_SERVER_ID &r_id,
                             const ID_TYPE     &r_idType,
                             const int         &r_cycId,
                          vector<AdjustBefore> &r_adjustBefores)
{
  int t_iRet = 0;
  
  Mdb_AdjustBefore          t_po;
  vector<Mdb_AdjustBefore>  t_rpos;
  
  t_po.m_record.m_id      = r_id;
  t_po.m_record.m_idType  = r_idType;
  t_po.m_record.m_startCycId = r_cycId;
  r_adjustBefores.clear();
  
  t_iRet=m_pMdbManager->searchMulti<Mdb_AdjustBefore>(t_po,t_rpos);
  if(t_iRet<=0) return t_iRet;
  for(vector<Mdb_AdjustBefore>::iterator t_itr = t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
    if(t_itr->m_record.m_recvTag != '0')
    { //该条调帐记录无效，则直接跳过
      continue;
    }
    r_adjustBefores.push_back(t_itr->m_record);
  }
  return t_iRet;
}

/*
 * 根据用户ID，取对应的付费关系
 * @param r_userId          用户ID
 * @param r_cycId           帐期
 * @param r_payRelations    付费关系信息
 * @param r_acctIdVector    帐户标识集.
 * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */         
int PM_Rate::getUserPayRelation(const T_SERVER_ID   &r_userId ,
                                const int           &r_cycId  ,
                                vector<PayRelation> &r_payRelations,
                                vector<T_SERVER_ID> &r_acctIdVector)
{
  
  int t_iRet=0;
  Mdb_PayRelation t_po;
  vector<Mdb_PayRelation> t_rpos;
  vector<T_SERVER_ID>::iterator itera;
  	
  t_po.m_record.m_userId = r_userId;
  t_po.m_record.m_startCycId = r_cycId;
  r_payRelations.clear();
  
  t_iRet=m_pMdbManager->searchMulti<Mdb_PayRelation>(t_po,t_rpos);
  if(t_iRet<=0) return t_iRet;
  for(vector<Mdb_PayRelation>::iterator t_itr = t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
  	/*if (r_cycId>0&&r_cycId<t_itr->m_record.m_startCycId)//modify by xuf 2009-8-24 17:09:30
    { //该条帐务关系未生效
      continue;
    }
  	//排重插入
		for (itera=r_acctIdVector.begin(); itera!=r_acctIdVector.end(); itera++)
		{
			if (t_itr->m_record.m_acctId == (*itera))
			{
				break;
			}
		}
		
		if (itera == r_acctIdVector.end())
		{
			r_acctIdVector.push_back(t_itr->m_record.m_acctId);
		}
    */
			
    //modify by xuf 2009-8-24 17:14:52			
    //if(t_itr->m_record.m_actTag != '1' || r_cycId<t_itr->m_record.m_startCycId|| r_cycId>t_itr->m_record.m_endCycId)
    //{ //该条帐务关系无效，则直接跳过
    //  continue;
    //}
      
     r_payRelations.push_back(t_itr->m_record);

  }
  //对用户付费关系进行排序
  ::partial_sort(r_payRelations.begin(),r_payRelations.end(),
                 r_payRelations.end());

  //if (r_cycId<=0)
  //  return r_acctIdVector.size();	
    
  return (r_payRelations.size());
}

/*
 * 根据用户ID，取对应的所有付费关系
 * @param r_userId          用户ID
 * @param r_cycId           帐期
 * @param r_payRelations    付费关系信息
 * @param r_acctIdSet       所有账户ID
 * @param r_acctIds    			所有有效账户ID
 
 * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */         
int PM_Rate::getUserPayRelation(const T_SERVER_ID   &r_userId ,
                                const int           &r_cycId  ,
                                vector<Paions.end());

  //if (r_cycId<=0)
  //  return r_acctIdVector.size();	
    
  return (r_payRelations.size());
}

/*
 * 鏍规嵁鐢ㄦ埛ID锛屽彇瀵瑰簲鐨勬墍鏈変粯璐瑰叧绯