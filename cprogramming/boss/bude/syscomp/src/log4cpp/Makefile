################################################################################
#  Makefile for log4cpp
################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
#注意, Logger.cpp 必须是最后一个, 保证所有的插件加载后, 初始化log4cpp
SRCS             =  DailyFileAppender.cpp       \
                    ConsoleAppender.cpp  \
                    StdFormatter.cpp    \
                    HessianFormatter.cpp        \
                    ExporterAppender.cpp        \
                    UdpAppender.cpp     \
                    Logger.cpp

SUBMIT_HDRS      =  Logger.h


#生成目标
PROGRAM          =
STATIC_LIBRARY   =
SHARE_LIBRARY    =	liblog4cpp.$(SHARE_LIB_SUFFIX)

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/log4cpp
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include

THE_LIBPATH      = -L$(WORK_HOME)/lib

THE_LIB          = -lcppsocket -lnsl -lbase -lhessian ${ICONV_LIB} ${SHARE_LD_FLAGS}
#-liconv

################################################################################
CXXFLAGS = ${THE_INCLUDE}
CCFLAGS  = ${THE_INCLUDE}
LDFLAGS  = ${THE_LIBPATH}
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
