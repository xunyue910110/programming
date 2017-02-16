################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = /billing/user/gaojf/mdb

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  Exception.cpp 

SUBMIT_HDRS      =  Exception.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libexception.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = ../lib
DESTINC = $(WORK_HOME)/include/mdb
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = -lapp -llog4cpp -lbase

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE} -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
