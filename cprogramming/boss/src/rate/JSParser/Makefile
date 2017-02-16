################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = CException.cpp DataBuffer.cpp JScriptParser.cpp SpiderMonkeyHelper.cpp

SUBMIT_HDRS      =

#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libratejsparser.a
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
#DESTLIB = $(WORK_HOME)/lib
DESTLIB = ../lib
DESTINC =
DESTSRC =
DESTDOC =

################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I../param \
                   -I../bill \
                   -I../policy \
                   -I../policy/mgr \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/pmbilling \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I$(WORK_HOME)/include  \
                   -I$(WORK_HOME)/include/mdb 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(ALTIBASE_HOME)/lib/ \
                   -L../param \
                   -L../param_local \
                   -L../bill


THE_LIB          = -lnsl -loradbi -ljs -ljshlp -lbase -lapp -lbill \
                   -lfile -ldbparam -lm -llog4cpp  -lclntsh  -lpthread


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} -lcdrbase -lrateparam -lratelocalparam -lrateBill -lmdb -lodbccli -lrateproc

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
