################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = MdbParamManager.cpp 


SUBMIT_HDRS      =  MdbParamManager.h Mdb_Common.h Mdb_Property.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libmdbparambase.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib/
DESTINC = $(WORK_HOME)/include/mdb_param
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
				           -I$(WORK_HOME)/include/mdb/ \
                   -I$(WORK_HOME)/include \
				           -I../common 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb

THE_LIB          = -lbase -lpthread -lmdb -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)  
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)  
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
