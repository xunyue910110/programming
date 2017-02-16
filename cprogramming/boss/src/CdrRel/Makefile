################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS              =  CdrRelApp.cpp CdrRelCommon.cpp

SUBMIT_HDRS      =  

#生成目标
PROGRAM          =  CdrRel
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I$(WORK_HOME)/include

THE_LIBPATH      = -L./ \
                   -L$(WORK_HOME)/lib

THE_LIB          = -lbase -lapp -llog4cpp -lclntsh -locci -loradbi

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB} ${ORA_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
