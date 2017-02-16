
################################################################################
#环境相关的宏定义
#  该Makefile配置必须依赖一组关联的配置文件，这些配置文件默认存放在 
#  ${BUDE_HOME}/stdmake 下。
################################################################################

include $(BUDE_HOME)/stdmake/stdenv
WORK_HOME = ${BUDE_HOME}/syscomp


################################################################################
#编译源和生成物定义：子目录、源文件
################################################################################

SUBDIRS =
SRCS    = budeipc.cpp IpcLock.cpp IpcShm.cpp IpcMsg.cpp IpcSqlBufImp.cpp

#提交内容：头文件、Makefile/makefile
SUBMIT_HDRS  = budeipc.h IpcSqlBuf.h

#生成结果
PROGRAM        = 
STATIC_LIBRARY = libuipc.a
SHARE_LIBRARY  = 

#结果提交至目录
DEST    =
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/uipc
DESTSRC =
DESTDOC = 


################################################################################
##编译相关的参数定义
################################################################################
THE_INCLUDE      = 
THE_LIBPATH      = 
THE_LIB          = 

CXXFLAGS= $(ORA_INCLUDE)
LDFLAGS = $(ORA_LIBPATH)
LDLIB   = $(ORA_LIB) 

###############################################################################
##编译的makefile定义，不要修改
################################################################################
include $(BUDE_HOME)/stdmake/stdmk