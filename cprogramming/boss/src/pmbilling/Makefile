################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =    PM_Billing.cpp IdInfo.cpp Mdb_UserParam.cpp \
                      PO_CycleEparchy.cpp PO_BillingCycle.cpp PO_SubCycle.cpp \
                      PO_SysParam.cpp PO_CycleFeeRule.cpp PO_Discount.cpp \
                      PO_UserChannel.cpp  PO_ServStatePriority.cpp PO_EventType.cpp \
                      Mdb_UserInfo.cpp Mdb_UserServState.cpp Mdb_UserTp.cpp \
                      Mdb_UserImportInfo.cpp Mdb_ReUserInfo_All.cpp Mdb_UserServ.cpp \
                      Mdb_UserCondTp.cpp Mdb_AccntInfo.cpp Mdb_UserServParam.cpp


SUBMIT_HDRS      =    PM_Billing.h  BillingCommon.h IdInfo.h Mdb_UserParam.h \
                      PO_CycleEparchy.h PO_BillingCycle.h PO_SubCycle.h \
                      PO_SysParam.h PO_CycleFeeRule.h PO_Discount.h \
                      Mdb_UserInfo.h Mdb_UserServState.h Mdb_UserTp.h \
                      Mdb_UserImportInfo.h PO_UserChannel.h Mdb_ReUserInfo_All.h \
                      PO_EventType.h PO_ServStatePriority.h Mdb_UserServ.h \
                      Mdb_UserCondTp.h Mdb_AccntInfo.h Mdb_UserServParam.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libbillingparam.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/pmbilling
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I$(WORK_HOME)/include/mdb/ \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb

THE_LIB          = 
                   

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG -D__PROV${PROV_CODE}__ 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG -D__PROV${PROV_CODE}__ 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = $(ORA_LIB) ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
