################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS 			 = ReleaseRCApp.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = RRC
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib    
DESTINC = $(WORK_HOME)/include
DESTSRC = 
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  -I$(WORK_HOME)/include \
				   -I$(WORK_HOME)/src/mdb/ \
                   -I$(WORK_HOME)/src/mdb/common/ \
                   -I$(WORK_HOME)/src/mdb/expression \
                   -I$(WORK_HOME)/src/mdb/semaphore \
                   -I$(WORK_HOME)/src/mdb/memMgr \
                   -I$(WORK_HOME)/src/mdb/client \
                   -I$(WORK_HOME)/include  \
                   -I$(ALTIBASE_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(ALTIBASE_HOME)/lib/

THE_LIB          =  -lapp -lbase -llog4cpp -lpthread -lmdb   -lodbccli

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
