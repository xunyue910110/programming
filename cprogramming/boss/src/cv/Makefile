################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             =  CDRConvertApp.cpp CCdrBase.cpp CCdrConverstion.cpp \
										CConvertConfig.cpp CvLog.cpp

SUBMIT_HDRS      = 

#生成目标
PROGRAM          = cv
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
                   -I../ \
                   -I$(WORK_HOME)/include \
                   -I$(BUDE_HOME)/syscomp/include/dbi \

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(BUDE_HOME)/syscomp/lib \

THE_LIB          = -lapp -lbase -lnsl -llog4cpp -lclntsh -loradbi -ludbi -locci -loccidbi

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) $ -DFP_DEBUG #-D_DEBUG_ ####-P   ###-qmaxmem=-1 -g -w 
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) ###-qmaxmem=-1 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

#STDMAKE_DEBUG   = -g -D_DEBUG_ -D$(_TYPE_ASP) -D_DEBUG_ -DFP_DEBUG -DOUT_DEBUG_ -D_PROCESS_TRACE_DUMP_