################################################################################
include $(BUDE_HOME)/stdmake/stdenv

################################################################################
SUBDIRS          =
AFTER_SUMMIT     =

################################################################################
SRCS             = RateApp.cpp  


SUBMIT_HDRS      =

#生成目标
PROGRAM          = rate
STATIC_LIBRARY   = 
SHARE_LIBRARY    =

#提交相关变量
DEST    = $(WORK_HOME)/bin
#DEST     = $(WORK_HOME)/bin/debug
DESTLIB = $(WORK_HOME)/lib
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
                   -I../local \
                   -I../base  \
                   -I../proc  \
                   -I../JSParser \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/pmbilling \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I$(WORK_HOME)/include  \
                   -I$(WORK_HOME)/include/mdb 

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(ALTIBASE_HOME)/lib/ \
                   -L../lib


THE_LIB          = -lnsl -loradbi -ljs16 -ljshlp -lbase -lapp  \
                   -lfile -ldbparam -lm -llog4cpp  -lclntsh  -lpthread


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} ${SYSCOMP_INCLUDE} ${ORA_INCLUDE} -DXP_UNIX  -DJSFILE -DHAVE_SYS_INTTYPES_H -DFP_DEBUG -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} ${SYSCOMP_LIBPATH} ${ORA_LIBPATH}
LDLIB    = ${THE_LIB} -lcdrbase -lbillingparam -lrateparam -lratebase -lratebill -lratelocal -lratepolicy -lratejsparser -lmdb -lodbccli -lrateproc -lmdbparambase -bbigtoc


################################################################################
include $(BUDE_HOME)/stdmake/stdmk
