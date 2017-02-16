################################################################################
#  Makefile for log4cpp
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  Demo.cpp       
                         
SUBMIT_HDRS      =     


#生成目标
PROGRAM          = demo
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/log4cpp
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib                       
                   
THE_LIB          = -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} 
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

