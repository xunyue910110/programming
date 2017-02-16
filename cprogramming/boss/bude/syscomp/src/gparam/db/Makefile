################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = Exp.cpp ExpAnd.cpp ExpOr.cpp ExpEq.cpp ExpGe.cpp ExpGt.cpp \
                   ExpLe.cpp ExpLt.cpp ExpNe.cpp  ExpStrnEq.cpp \
            		QueryUseExpression.cpp QueryManager.cpp ParameterBuffer.cpp \
            		DataSource.cpp DbDataSource.cpp  \
            		ParameterList.cpp DataSourceList.cpp FileDataSource.cpp  \
            		ParameterCdr.cpp  ParameterLog.cpp \
            		ParameterInterface.cpp  Parameter.cpp IndexMessage.cpp  \
            		Index.cpp ParameterIndex.cpp 

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/gparam
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
CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE}#_DFOR_TEST
CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
