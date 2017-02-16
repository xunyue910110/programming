################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 

################################################################################
SRCS             = CEventCreatorApp.cpp   CEventInfoCreator.cpp  CEventInput.cpp CEventOutputFile.cpp	
SUBMIT_HDRS      = 

#生成目标
PROGRAM          = eventor
STATIC_LIBRARY   = 
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/mdb
DESTSRC =
DESTDOC = 

################################################################################
## 编译相关的参数定义
################################################################################
# 编译时需要的BUDE基础开发环境组件库名
BUDE_LIB      = -lapp  -ldbparam -lbase -loradbi -llog4cpp


################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include/mdb \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include \
                   -I$(WORK_HOME)/include/pmbilling	

                   
THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib/mdb \
                   -L$(WORK_HOME)/lib \
                   -L$(ALTIBASE_HOME)/lib
                   
                   

THE_LIB          = -lbase -lpthread -lmdb -llog4cpp -lnsl \
                   -ldbparam \
                   -lapp	-lbillingparam	\
                   -lmdbparambase\
                   -lodbccli \
                   -locci \
                   -lpthread
                 
#THE_INCLUDE+= -I$(ALTIBASE_HOME)/include
#THE_LIBPATH+= -L$(ALTIBASE_HOME)/lib/
#THE_LIB+= -lodbccli

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG -D_POSTPAIDSP_ -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -DFP_DEBUG -D__PROV${PROV_CODE}__
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = $(THE_LIB) $(BUDE_LIB) $(ORA_LIB)

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
