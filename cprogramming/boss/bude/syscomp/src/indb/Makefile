################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp
################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = indb.cpp



SUBMIT_HDRS      =

#生成目标
PROGRAM          = indb indb_dr
STATIC_LIBRARY   =
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB =
DESTINC =
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = ${NET_LIB} -loradbi -lapp -llog4cpp -lbase -lclntsh

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} ${ORA_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

