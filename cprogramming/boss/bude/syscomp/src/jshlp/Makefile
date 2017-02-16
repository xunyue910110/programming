include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

SUBDIRS      =
SRCS         = JavaScript.cpp
SUBMIT_HDRS  = JavaScript.h 

PROGRAM        = 
STATIC_LIBRARY = libjshlp.a
SHARE_LIBRARY  = 

DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/jshlp
DESTSRC =
DESTDOC = 

THE_INCLUDE      =  -I$(WORK_HOME)/include/js16
THE_LIBPATH      =  
THE_LIB          = 

CXXFLAGS=${THE_INCLUDE} ${SYSCOMP_INCLUDE} -DXP_UNIX -DJSFILE -DDEBUG

LDFLAGS = 
LDLIB   = 
#包含预定义的 makefile
include $(BUDE_HOME)/stdmake/stdmk



