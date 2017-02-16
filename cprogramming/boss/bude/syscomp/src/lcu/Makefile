################################################################################
#  Makefile for lcu
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          =
BEFORE_SUMMIT    =
AFTER_SUMMIT     =


################################################################################
SRCS             =  lcu.cpp \
                    CDateTime.cpp \
                    CSLEngine.cpp  \
                    decidetoolkit.cpp


SUBMIT_HDRS      = lcu.h  plugins.h lcuexceptions.h  decidetoolkit.h


#生成目标
PROGRAM          =
STATIC_LIBRARY   =
SHARE_LIBRARY    = liblcu.$(SHARE_LIB_SUFFIX)

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/lcu
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(BUDE_HOME)/lang/include/base \
                   -I$(WORK_HOME)/include

THE_LIBPATH      = -L./  \
                   -L../lib/ \
                   -L$(WORK_HOME)/lib

THE_LIB          = -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE}
CCFLAGS  = ${THE_INCLUDE}
LDFLAGS  = ${THE_LIBPATH}
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

