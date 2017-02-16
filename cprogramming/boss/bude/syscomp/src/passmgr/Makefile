################################################################################
#  Makefile for Pass
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             =  DBPassOper.cpp HostPassOper.cpp

SUBMIT_HDRS      =  DBPassOper.h HostPassOper.h

#生成目标
PROGRAM          =
STATIC_LIBRARY   =
SHARE_LIBRARY    = libpass.$(SHARE_LIB_SUFFIX)

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/pass
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I${BUDE_HOME}/lang/include/base \
                   -I${BUDE_HOME}/syscomp/include  \
                   -I${BUDE_HOME}/syscomp/include\des \
                   -I${BUDE_HOME}/syscomp/include\archy

THE_LIBPATH      = -L./  \
                   -L${BUDE_HOME}/syscomp/lib
                   

THE_LIB          = -llog4cpp -lbase -ludbi -ldes -larchy -lpthread

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH}
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
