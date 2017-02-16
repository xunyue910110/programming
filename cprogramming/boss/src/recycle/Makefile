################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = RecycleApp.cpp OutPutFile.cpp BillUpdate.cpp \
                   OutPutBill.cpp 
                   #PO_DetailAccount.cpp PO_BillCycle.cpp \
                   #PMManager.cpp DetailAccountItem.cpp

SUBMIT_HDRS      =

#生成目标
PROGRAM          = redo
STATIC_LIBRARY   =
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC =
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(WORK_HOME)/include \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I$(WORK_HOME)/src/rate/base

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(ALTIBASE_HOME)/lib 

THE_LIB          = $(NET_LIB) -lnsl -loradbi -lapp -lbase -lfile \
                   -llog4cpp -lratebill -ldbparam -lmdb -lodbccli -lpthread

################################################################################
CXXFLAGS = -g ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} 
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} 
LDFLAGS  = -g ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} ${ORA_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

