################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = Utility.cpp          FTPClientWrapper.cpp \
                   FSystemAgent.cpp      FTPImpl.cpp          FILEImpl.cpp         \
                   DTAImpl.cpp           FTAMImpl.cpp         \
                   ControlInfo.cpp       FTController.cpp NAMEJScript.cpp

SUBMIT_HDRS      = FTController.h  FTPClientWrapper.h FSystemAgent.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libftrans.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/ftrans
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include


THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib 


THE_LIB          = -ldta  -lnsl -lbase  -ljs -ljshlp

################################################################################
CXXFLAGS = ${THE_INCLUDE}  -DXP_UNIX -DJSFILE
CCFLAGS  = ${THE_INCLUDE} 
LDFLAGS  = ${THE_LIBPATH} 
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
