################################################################################
include $(BUDE_HOME)/stdmake/stdenv

WORK_HOME = ${BUDE_HOME}/syscomp

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = SAX.c entities.c encoding.c error.c parserInternals.c  \
                   parser.c tree.c hash.c list.c xmlIO.c xmlmemory.c uri.c  \
                   valid.c xlink.c HTMLparser.c HTMLtree.c debugXML.c xpath.c  \
                   xpointer.c xinclude.c nanohttp.c nanoftp.c DOCBparser.c \
                   catalog.c globals.c threads.c c14n.c xmlstring.c \
                   xmlregexp.c xmlschemas.c xmlschemastypes.c xmlunicode.c \
                   xmlreader.c relaxng.c dict.c SAX2.c \
                   xmlwriter.c legacy.c chvalid.c pattern.c xmlsave.c \
                   XmlApi.cpp \
                   XmlParam.cpp

SUBMIT_HDRS      = XmlApi.h XmlParam.h

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libxml.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/xml
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include \
                   -I./include

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
