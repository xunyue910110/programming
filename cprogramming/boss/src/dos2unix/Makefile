################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  dos2unix.cpp

SUBMIT_HDRS      =  

#生成目标
PROGRAM          = dos2unix
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = 
DESTINC =
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include/ocbs \
                   -I$(WORK_HOME)/include

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib/ocbs \
                   -L$(WORK_HOME)/lib

THE_LIB          = -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) -D_SYS_CONF_
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE)
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH)
#LDLIB    = ${THE_LIB} $(LANG_BASE_LIB) 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
