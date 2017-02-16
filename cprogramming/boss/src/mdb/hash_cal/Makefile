################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = ti.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = ti
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = 
DESTINC = 
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  -I$(WORK_HOME)/include \
					-I$(WORK_HOME)/include/mdb \
					-I.. -I../memMgr -I../common -I../semaphore

THE_LIBPATH      = -L./  -L$(WORK_HOME)/lib -L$(WORK_HOME)/lib/mdb -L$(ALTIBASE_HOME)/lib/

THE_LIB          = -lbase -lpthread -lmdb -lodbccli

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG -w
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
#ifdef ALTIBASE_HOME
#    SRCS+= ODBC/OdbcDB.cpp  ODBC/OdbcSession.cpp
#    CXXFLAGS+= -DALTIBASE
#    THE_INCLUDE+= -I$(ALTIBASE_HOME)/include
#    THE_LIBPATH+= -L$(ALTIBASE_HOME)/lib/
#    THE_LIB+= -lodbccli
#else 
#    ifdef TimesTen
#        SRCS+= ODBC/OdbcDB.cpp  ODBC/OdbcSession.cpp
#        CXXFLAGS+= -DTIMESTEN
#        THE_INCLUDE+= -I$(TimesTen)/include
#        THE_LIBPATH+= -L$(TimesTen)/lib/
#        THE_LIB+= -lodbc 
#    endif
#endif

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
