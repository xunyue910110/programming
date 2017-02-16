################################################################################
#include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = patch.cpp

SUBMIT_HDRS      =

#生成目标
PROGRAM          = patch
STATIC_LIBRARY   =
SHARE_LIBRARY    =

#提交相关变量
DEST    =
DESTLIB =
DESTINC =
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include \
                   -I$(BUDE_HOME)/lang/include \
                   -I$(BUDE_HOME)/lang/include/base \
                   -I$(WORK_HOME)/include \
                   -I$(WORK_HOME)/include/file


THE_LIBPATH      = -L./  \
                   -L/usr/lib \
                   -L/usr/local/lib \
                   -L$(BUDE_HOME)/lang/lib \
                   -L$(WORK_HOME)/lib

THE_LIB          =  -lbase  -lfile


################################################################################
#CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
#CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
#LDFLAGS  = ${THE_LIBPATH} ${ORA_LIBPATH} ${TUX_LIBPATH} ${SYSCOMP_LIBPATH}
#LDLIB    = ${THE_LIB} ${ORA_LIB} ${TUX_LIB} ${SYSCOMP_MWCI_LIB} ${SYSCOMP_UDBI_LIB} ${SYSCOMP_LOG4CPP_LIB}
CXXFLAGS =  ${THE_INCLUDE}
CCFLAGS  =  ${THE_INCLUDE}   
LDFLAGS  =  ${THE_LIBPATH}
LDLIB    =  ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
