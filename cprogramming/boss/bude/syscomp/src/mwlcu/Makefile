################################################################################
#  Makefile for mwlcu
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             =  tradestateagent.cpp \
                    lcuplugins.cpp    \
                    fmlcontext.cpp  \
                    fmloperator.cpp \
                    udbidbloader.cpp \
                    udbidblog.cpp  \
                    mwlcu.cpp


SUBMIT_HDRS      =  fmloperator.h  fmlcontext.h  lcuplugins.h  mwlcu.h  udbidblog.h  udbidbloader.h tradestateagent.h

#生成目标
PROGRAM          =
STATIC_LIBRARY   =
SHARE_LIBRARY    = libmwlcu.$(SHARE_LIB_SUFFIX)

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/mwlcu
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../

THE_LIBPATH      = -L./  \
                   -L../lib/ \
                   -L$(WORK_HOME)/lib


THE_LIB          = -lbase -llcu -ldynlib -ludbi -lmwci

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${ORA_LIBPATH} ${TUX_LIBPATH} ${SYSCOMP_LIBPATH}
LDLIB    = ${THE_LIB} ${ORA_LIB} ${TUX_LIB} ${SYSCOMP_MWCI_LIB} ${SYSCOMP_UDBI_LIB} ${SYSCOMP_LOG4CPP_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk


