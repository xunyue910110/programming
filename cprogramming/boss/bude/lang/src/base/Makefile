################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/lang

################################################################################
SUBDIRS          =
AFTER_SUBMIT     = cp -f $(DESTINC)/config-all.h $(DESTINC)/../config-all.h

################################################################################
SRCS             = StringUtil.cpp Directory.cpp Properties.cpp SysParam.cpp Types.cpp \
                   PatternMatcher.cpp  SimpleConfigure.cpp CryptUtil.cpp UUID.cpp \
                   PString.cpp ConfigFileMgr.cpp

SUBMIT_HDRS      = Directory.h Properties.h StringUtil.h SysParam.h Types.h \
                   SmartPtr.h PatternMatcher.h TimeUtil.h BCDUtil.h CryptUtil.h \
                   SimpleConfigure.h Exception.h Exception.hpp config-all.h \
                   exceptions.h supdebug.h OcciUtil.h UUID.h ClassRegister.h  \
                   PString.h ConfigFileMgr.h
#生成目标
PROGRAM          =
STATIC_LIBRARY   = libbase.a
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/base
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
CXXFLAGS = ${THE_INCLUDE}
CCFLAGS  = ${THE_INCLUDE}
LDFLAGS  = ${THE_LIBPATH}
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
