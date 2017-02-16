################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUBMIT     = cp help.txt ${WORK_HOME}/bin; cp welcome.txt ${WORK_HOME}/bin
################################################################################
SRCS             = console.cpp MdbDmlSql.cpp

SUBMIT_HDRS      = common.h functions.h

#生成目标
PROGRAM          = console
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = #$(WORK_HOME)/lib/mdb
DESTINC = #$(WORK_HOME)/include/mdb
DESTSRC =
DESTDOC = 

################################################################################
# ifdef ALTIBASE_HOME
THE_INCLUDE      = -I./  \
				   -I$(WORK_HOME)/src/mdb/ \
                   -I$(WORK_HOME)/src/mdb/common/ \
                   -I$(WORK_HOME)/src/mdb/expression \
                   -I$(WORK_HOME)/src/mdb/semaphore \
                   -I$(WORK_HOME)/src/mdb/memMgr \
                   -I$(WORK_HOME)/include/   \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(BUDE_HOME)/lang/include/base \
                   -I$(ALTIBASE_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib  \
                   -L$(WORK_HOME)/lib/mdb   \
                   -L$(BUDE_HOME)/lang/lib \
                   -L$(ALTIBASE_HOME)/lib/ -lodbccli

#else
#
#INCLUDE      = -I./  \
#                                   -I$(WORK_HOME)/src/mdb/ \
#                   -I$(WORK_HOME)/src/mdb/common/ \
#                   -I$(WORK_HOME)/src/mdb/expression \
#                   -I$(WORK_HOME)/src/mdb/semaphore \
#                   -I$(WORK_HOME)/src/mdb/memMgr \
#                   -I$(WORK_HOME)/include/   \
#                   -I$(WORK_HOME)/include/mdb \
#                   -I$(ALTIBASE_HOME)/include
#
#THE_LIBPATH      = -L./  \
#                   -L$(WORK_HOME)/lib  \
#                   -L$(WORK_HOME)/lib/mdb   \
#                   -L$(ALTIBASE_HOME)/lib/ -lodbccli
#
#
#endif

THE_LIB          = -lbase -lpthread -lmdb



###############################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE)
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
