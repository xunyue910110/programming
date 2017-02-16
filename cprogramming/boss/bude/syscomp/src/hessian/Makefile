################################################################################
#  Makefile for log4cpp
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
#注意, Logger.cpp 必须是最后一个, 保证所有的插件加载后, 初始化log4cpp
SRCS             =  hessian_input.cpp hessian_output.cpp string_input_stream.cpp wrappers.cpp iconverter.cpp
                         
SUBMIT_HDRS      =  dump.h        hessian_input.h   input_stream.h         wrappers.h	 exceptions.h  hessian_output.h  string_input_stream.h


#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libhessian.a 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/hessian
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
CXXFLAGS = `sh ticonv.sh ${CXX}` ${THE_INCLUDE} 
CCFLAGS  = ${THE_INCLUDE} 
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

