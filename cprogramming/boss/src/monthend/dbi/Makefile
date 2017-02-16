################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = AltibaseIntf.cpp

SUBMIT_HDRS      = AltibaseIntf.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libabdbi.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = ../lib
DESTINC = ../include
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include -I$(HOME)/include -I$(ALTIBASE_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib -L$(HOME)/lib

THE_LIB          = 

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
CCFLAGS  = 
LDFLAGS  =  
LDLIB    = 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
