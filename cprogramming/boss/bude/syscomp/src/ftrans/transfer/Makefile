################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = TransferApp.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = ftrans ftrans_dr
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = 
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include


THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib 

THE_LIB          = -lftrans -ldta -lapp -lbase -llog4cpp -ljs16 -ljshlp

################################################################################
CXXFLAGS = ${THE_INCLUDE}  ${SYSCOMP_INCLUDE} -DXP_UNIX -DJSFILE -DAIX -DHAVE_SYS_INTTYPES_H
CCFLAGS  = ${THE_INCLUDE}  ${SYSCOMP_INCLUDE}
LDFLAGS  = ${THE_LIBPATH}  ${SYSCOMP_LIBPATH}
LDLIB    = ${THE_LIB} 

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
