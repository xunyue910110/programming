################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = PM_Rate.cpp       PM_Rate_Mdb.cpp  PM_RatePolicy.cpp \
                   PO_RedoUserInfo.cpp PO_EffectFeeCount.cpp \
                   PO_InterRoamInFeeBaseTp.cpp PO_BaseTp.cpp     PO_FeebaseTP.cpp     \
                   PO_SpecialInfo_comp.cpp   PO_CondRuleExists.cpp \
                   PO_UserTpParam_def.cpp PO_SpecInfo_org.cpp  PO_AttrFactors.cpp  \
                   Mdb_VpnInfo_Number.cpp  Mdb_PayRelation_Acct.cpp \
                   Mdb_VpnInfo_UserId.cpp     Mdb_UserTp_Param.cpp Mdb_UserFeeSwap.cpp \
                   PO_BaseCellTp.cpp  PO_DetailBillItem.cpp PO_DefaultFeePolicy.cpp \
                   PO_AdjustReason.cpp   PO_FeePolicy_Rent.cpp PO_FeePolicyRemidInfo.cpp \
                   Mdb_PayRelation.cpp Mdb_PayRelation_All.cpp PO_Holiday.cpp \
                   Mdb_AdjustBefore.cpp Mdb_UserInfo_VpnId_All.cpp \
                   Mdb_AcctBakBill.cpp   Mdb_Acct_Unipay.cpp \
                   PO_FeePolicy.cpp PO_FeePolicyComp.cpp PO_Price.cpp PO_PriceComp.cpp \
        		   PO_EffectDiscnt.cpp PO_CondRuleRela.cpp PO_CondRuleScope.cpp \
        		   PO_CondRuleScopeL.cpp PO_CondRuleScopeR.cpp PO_CondRuleScopeLR.cpp \
        		   PO_CondRuleIn.cpp PO_ObjectBill.cpp PO_ObjectCdr.cpp PO_ObjectUser.cpp \
        		   PO_ObjectAddup.cpp  PO_AddupItem.cpp PO_Item.cpp PO_CompositItem.cpp \
        		   PO_Condition.cpp PO_Object.cpp PO_PriceParamImpl.cpp PO_PriceExtend.cpp \
                   PO_AddupCycleRule.cpp PO_EffectAddup.cpp PO_EventFeePolicy.cpp \
                   PO_DetailItem.cpp CompositItemMgr.cpp PO_EventElem.cpp PO_ProductRel.cpp \
                   PO_InfoElem.cpp PO_PartyPrice.cpp PO_FeePolicyAttr.cpp PO_VpnRelFeePolicy.cpp\
                   PO_SectionRelFix.cpp PO_DiscntStatecode.cpp Mdb_UserCondTp_Param.cpp PO_BaseTpGroup.cpp\
                   PO_FeepolicyMutexRela.cpp PO_RateEventExclusiveRela.cpp \
                   Mdb_AdcECAbnormalState.cpp Mdb_AdcECuserNum.cpp Mdb_SpCdrInfo.cpp PO_SpRelaDiscntDetail.cpp\
                   PO_SpPunish.cpp PO_SpTariff.cpp PO_SpDiscount.cpp 
                   
                   
SUBMIT_HDRS      =

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = librateparam.a
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
#DESTLIB = $(WORK_HOME)/lib
DESTLIB = ../lib
DESTINC =
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I$(WORK_HOME)/include \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I../local \
                   -I../bill \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(WORK_HOME)/include/pmbilling \
                   -I../base  \
                   -I../policy   \
                   -I../policy/mgr \
                   -I../JSParser   \
                   -I.. 


THE_LIBPATH      = 


THE_LIB          = 


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DAIX -DJSFILE -DHAVE_SYS_INTTYPES_H -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DAIX -DJSFILE -DHAVE_SYS_INTTYPES_H -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
