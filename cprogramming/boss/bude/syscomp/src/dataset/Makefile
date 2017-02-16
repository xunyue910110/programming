################################################################################
#  Makefile for log4cpp
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  DataSet.cpp    
                         
SUBMIT_HDRS      = DataSet.h Index.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = 
SHARE_LIBRARY    = libdataset.$(SHARE_LIB_SUFFIX)

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/dataset
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
CXXFLAGS = ${THE_INCLUDE} 
CCFLAGS  = ${THE_INCLUDE} 
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

