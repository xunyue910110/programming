################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = PM_Rate_Local.cpp   PO_CellDef.cpp \
                   LocalProc_$(ASP_CODE)_$(PROV_CODE).cpp \
                   RateAppProc_$(ASP_CODE)_$(PROV_CODE).cpp

SUBMIT_HDRS      =

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libratelocal.a
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
                   -I../bill \
                   -I$(WORK_HOME)/include \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I../param \
                   -I../policy   \
                   -I../policy/mgr \
                   -I../app \
                   -I../proc \
                   -I../JSParser \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(WORK_HOME)/include/pmbilling


THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib 


THE_LIB          = 


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
