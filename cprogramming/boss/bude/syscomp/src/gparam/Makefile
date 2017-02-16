################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = Server.cpp API.cpp UnixSocket.cpp Session.cpp Listener.cpp \
Manager.cpp  Client.cpp DialogTool.cpp GParamManager.cpp  \
util/Util.cpp  \
util/DataFormat.cpp  \
util/DateTime.cpp \
util/PFileProcess.cpp \
util/Pdebug.cpp \
util/PConfigureFile.cpp \
util/cs_common.cpp \
util/MyFileProcess.cpp \
db/Exp.cpp db/ExpAnd.cpp db/ExpOr.cpp db/ExpEq.cpp \
db/ExpGe.cpp db/ExpGt.cpp db/ExpLe.cpp db/ExpLt.cpp \
db/ExpNe.cpp db/ExpStrnEq.cpp db/QueryUseExpression.cpp \
db/QueryManager.cpp db/ParameterBuffer.cpp db/DataSource.cpp \
db/DbDataSource.cpp db/ParameterList.cpp \
db/DataSourceList.cpp db/FileDataSource.cpp \
db/ParameterCdr.cpp db/ParameterLog.cpp \
db/ParameterInterface.cpp db/Parameter.cpp \
db/IndexMessage.cpp db/Index.cpp db/ParameterIndex.cpp 
                   
                   
SUBMIT_HDRS      = GParamManager.h DialogTool.h Thread.h Config.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libgparam.a
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
                   -I$(WORK_HOME)/include \
                   -I./db \
                   -I./util
                   
THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = 

################################################################################
CXXFLAGS = ${THE_INCLUDE} ${ORA_INCLUDE} -D_BUDE15_
CCFLAGS  = ${THE_INCLUDE} ${ORA_INCLUDE} -D_BUDE15_
LDFLAGS  = ${THE_LIBPATH} -D_BUDE15_
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
