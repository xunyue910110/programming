################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = ParamManager.cpp

SUBMIT_HDRS      = ParamContainer.h ParamContainerNext.h ParamContainerChild.h ParamContainer.cpp ParamContainerNext.cpp ParamContainerChild.cpp ParamContainerBase.h ParamManager.h PO_Common.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libdbparam.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/dbparam
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = 

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
