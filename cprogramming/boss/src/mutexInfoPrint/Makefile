################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = $(WORK_HOME)/src/mdb/semaphore/DoubleLock.cpp  $(WORK_HOME)/src/mdb/semaphore/MutexDoubleLock.cpp \
                   $(WORK_HOME)/src/mdb/common/Mdb_Exception.cpp $(WORK_HOME)/src/mdb/semaphore/MutexShmMgr.cpp $(WORK_HOME)/src/mdb/semaphore/MutexSrcMgr.cpp  \
                   MutexInfoPrint.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = MutexInfoPrint
STATIC_LIBRARY   = 
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
                   -I$(WORK_HOME)/src/mdb/common/ \
                   -I$(WORK_HOME)/src/mdb/semaphore/ \
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib 

THE_LIB          = -lpthread 

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -D_REENTRANT -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -D_REENTRANT -DFP_DEBUG 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB} #-lexception

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

