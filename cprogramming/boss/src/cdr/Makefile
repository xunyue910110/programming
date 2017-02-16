################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     =

################################################################################
SRCS             = CdrBase.cpp CdrFormat.cpp  CdrFmtConfig.cpp  CDRJScript.cpp


SUBMIT_HDRS      =AttrIdConstDef.h  CdrBase.h CdrFormat.h CdrFmtConfig.h CDRJScript.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libcdrbase.a
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC =$(WORK_HOME)/include/cdr
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include   

THE_LIBPATH      = -L./  


THE_LIB          = -lnsl -loradbi -ljs -ljshlp -lbase \
                    -lm -llog4cpp  -lclntsh  -lpthread


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DJSFILE  -DHAVE_SYS_INTTYPES_H 
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DJSFILE  -DHAVE_SYS_INTTYPES_H
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
