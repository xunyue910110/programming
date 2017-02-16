################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
#    SRCS             = CConsoleApp1.cpp sqlParser.cpp selectParser.cpp \
#                       ColumnValue.cpp  MdbUtil.cpp deleteParser.cpp updateParser.cpp \
#                       insertParser.cpp commonSqlParser.cpp createParser.cpp 
#     SRCS           = test.cpp MdbUtil.cpp
#   SRCS             = CConsoleApp.cpp
# 	SRCS = indexcfg.cpp
SRCS = mdbcfg.cpp
SUBMIT_HDRS      = 

#生成目标
PROGRAM          = createtool
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
                   -I$(WORK_HOME)/include  

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(ALTIBASE_HOME)/lib/ 
                   #\
                   #-L$(ALTIBASE_HOME)/lib/

THE_LIB          =  -lbase -llog4cpp -lpthread -lmdb 

#-lodbccli  

#endif
################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB} -lmdb -lodbccli  

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

