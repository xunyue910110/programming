################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
HDRS  = 

SRCS  = RrApplication.cpp  

                 
SUBMIT_HDRS      =

#生成目标
PROGRAM          = rr 
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
                   -I$(WORK_HOME)/include 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib

THE_LIB          = -lapp -lbase -loradbi -lnsl -llog4cpp -lclntsh


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DRUN_IN_BACK -DFP_DEBUG -D__HPACC_NOEH -D_REENTRANT
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} ${ORA_LIB} -lrr

################################################################################
include $(BUDE_HOME)/stdmake/stdmk

#指定编译时的参数
#CXXFLAGS= $(FRM_INCLUDE) $(ORA_INCLUDE)
#指定连接时的参数
#LDFLAGS	= $(FRM_LIBPATH) $(FRM_LIB) $(ORA_LIBPATH) $(ORA_LIB) -ldbparam -lbill
