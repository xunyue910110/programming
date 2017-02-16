################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = TpProc.cpp  BillProc.cpp PriceBaseProc.cpp PriceProc.cpp \
                    AcctDiscnt.cpp  AcctDiscntProc.cpp  AdjustBeforeCalc.cpp  \
                    DiscntBase.cpp  PayRelationProc.cpp PayRelationSolution.cpp \
                    UserDiscnt.cpp  UserDiscntProc.cpp  UserGroupDiscnt.cpp \
                    TpRealRentMgr.cpp RentProc.cpp    GprsPriceProc_Unicom.cpp  \
                    SpPriceProc_Unicom.cpp  SumBillProc.cpp RemindProc.cpp \
		    AcctAttribProc.cpp AcctBillProc.cpp  AcctTpProc.cpp \
		    UserGrpDiscntProc.cpp AccountAttribProc.cpp	\
		    Monfeeproc.cpp CDiscntMonfee.cpp CRecvMonfee.cpp  UserAfterAcctDiscntProc.cpp  						

SUBMIT_HDRS      =

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = librateproc.a
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
                   -I../base \
                   -I../param \
                   -I../bill \
                   -I../policy \
                   -I../policy/mgr \
                   -I../JSParser \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/pmbilling \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I$(WORK_HOME)/include  \
                   -I$(WORK_HOME)/include/mdb 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib 


THE_LIB          = -lnsl -loradbi -ljs -ljshlp -lbase -lapp -lbill \
                   -lfile -ldbparam -lm -llog4cpp  -lclntsh 


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D_GROUPTEST_ -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
