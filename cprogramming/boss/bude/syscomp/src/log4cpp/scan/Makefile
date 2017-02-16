################################################################################
#  Makefile for log4cpp
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  scanmain.cpp ExporterFileScanner.cpp      
                         
SUBMIT_HDRS      =  ExporterFileScanner.h	ExportConstants.h   


#生成目标
PROGRAM          = scan
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
                   
THE_LIB          = -lcppsocket -lhessian -liconv -lbase

################################################################################
CXXFLAGS = ${THE_INCLUDE} -DFP_DEBUG
CCFLAGS  = ${THE_INCLUDE} 
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

