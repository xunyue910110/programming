################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = 

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = DoubleLock.cpp     IpcDoubleLock.cpp  SysSemaphore.cpp 
                   

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = #smtest
STATIC_LIBRARY   = libsemlock.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = ../lib
DESTINC = $(WORK_HOME)/include
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I../common/ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L../lib

THE_LIB          = -lpthread -lbase -lnsl -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB} -lexception

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
