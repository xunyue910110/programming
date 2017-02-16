################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS	      = PM_Filter.cpp PM_Filter_Mdb.cpp \
PO_TimeMethodBind.cpp \
PO_NetTypeCodeBind.cpp \
PO_SmServCode.cpp \
PO_SpServCode.cpp \
PO_HomeAreaCodeI.cpp \
PO_HomeAreaCodeN.cpp \
PO_HomeAreaCodeO.cpp \
PO_HomeAreaCodeP.cpp \
PO_HomeAreaCodeByPhoneN.cpp \
PO_HomeAreaCodeBySSP.cpp \
PO_SpOperFee.cpp \
PO_UssdServCode.cpp \
PO_MmsServCodeBySpOper.cpp \
PO_AspByMsisdnHeader.cpp \
PO_NrChannelByProvCode.cpp \
PO_DetailChannel.cpp \
PO_UserChannel.cpp \
PO_VirtualArea.cpp \
PO_HomeAreaCodeC.cpp \
PO_AreaCodeImsiI.cpp \
PO_HomeAreaCodeImsiN.cpp \
PO_HomeAreaCodeImsiP.cpp \
PO_MscByAlias.cpp \
PO_VisitAreaCodeLac.cpp \
PO_AVByMscCellid.cpp \
PO_AVByMscLac.cpp \
PO_AVByMscLacCellid.cpp \
PO_AVByMsc.cpp \
PO_AVByLac.cpp \
PO_AVByCellid.cpp \
PO_AVByLacCellid.cpp \
PO_HomeAreaCode.cpp \
PO_FixTelephone.cpp \
PO_HomeAreaCodeBySpecialMsisdn.cpp \
PO_TransNumber.cpp \
PO_SpecialPhone.cpp \
PO_SpecialPhoneGroup.cpp \
PO_B_Object.cpp \
PO_B_CompCond.cpp \
PO_B_SimpleCond.cpp \
PO_B_Eventelem.cpp \
PO_VoiceMagzine.cpp \
PO_ProvInnerCode.cpp \
PO_ProvFixTelephone.cpp \
PO_EdgeCellId.cpp \
PO_EdgeLacCellId.cpp \
PO_EdgeMscCellId.cpp \
PO_EdgeMscLacCellId.cpp \
PO_EdgeVisitCodeLacCellId.cpp \
PO_AreaCodeInterAsp.cpp \
PO_GprsSgsnAddr.cpp \
PO_GprsApn.cpp \
PO_GprsApnniType.cpp \
PO_SatelCar.cpp \
PO_Trunk.cpp \
PO_StateGroup.cpp \
PO_VistitAreaRoam.cpp \
PO_HomeAreaCodeFix.cpp \
PO_ExchangeToSectionFix.cpp \
PO_FixTrunk.cpp \
PO_MsisdnSectionFix.cpp \
PO_SectionRelFix.cpp \
PO_NbrGroup.cpp \
PO_NumberConvert.cpp \
PO_ExchangeDiscount.cpp \
PO_FixSpecialPhoneSection.cpp \
PO_JudgeFemtoByLacCellid.cpp \
PO_HomeAreaCodeByBureau.cpp \
Mdb_UserInfo_Crm.cpp \
Mdb_UserImInfobyPhone.cpp \
Mdb_UserImInfobyImsi.cpp \
Mdb_UserServState.cpp \
Mdb_UserOtherInfo.cpp \
Mdb_VpnInfobyPhone.cpp \
Mdb_User_Param.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          =
STATIC_LIBRARY   = libfilterparam.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = 
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I../app \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(WORK_HOME)/include \
                   -I$(ALTIBASE_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(WORK_HOME)/lib \
                   -L$(ALTIBASE_HOME)/lib

#THE_LIB          = -lbase -ldbparam -loradbi -lnsl -lclntsh -lmdb -lthread
THE_LIB          = -lbase -ldbparam  -lpthread -loradbi -lnsl -lclntsh -lodbccli -lmdb -ljs -ljshlp

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DFP_DEBUG -D_TYPE_UNICOM
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} ${ORA_LIB} -lcdrbase -lmdbparambase

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

