################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
SUBDIRS          = 
AFTER_SUMMIT     = 



################################################################################
SRCS  = CondRule.cpp           \
				CondRuleIn.cpp         \
				CondRuleRela.cpp       \
				CondRuleScope.cpp      \
				CondRuleScopeL.cpp     \
				CondRuleScopeLR.cpp    \
				CondRuleScopeR.cpp     \
				NodeBase.cpp           \
				FeePolicyComp.cpp      \
				Price.cpp              \
				PriceComp.cpp			\
				PriceParamImpl.cpp		\
				PriceExtend.cpp			\
				EventFeePolicy.cpp		\
				AddupItem.cpp			\
				AddupCycleRule.cpp		\
				ObjectBase.cpp			\
				ObjectBill.cpp			\
				ObjectAddup.cpp			\
				ObjectCdr.cpp          \
				ObjectUser.cpp         \
				ObjectValue.cpp         \
				ObjectParam.cpp         \
				ObjectUserParam.cpp     \
				CondRuleExists.cpp	   \
				EffectBase.cpp		   \
				EffectDiscnt.cpp	   \
				EffectAddup.cpp		   \
				EffectFeeCount.cpp    \
				ElemEvent.cpp   		\
				ElemInfo.cpp	    	\
				PolicyInfo.cpp			\
				mgr/DetailBillMgr.cpp   \
				mgr/SumBillMgr.cpp     
    
#生成目标
PROGRAM          = 
STATIC_LIBRARY   = libratepolicy.a
SHARE_LIBRARY    = 

#提交相关变量
DEST    = $(WORK_HOME)/bin
#DESTLIB = $(WORK_HOME)/lib
DESTLIB = ../lib
DESTINC = $(WORK_HOME)/include
DESTSRC =
DESTDOC = 

################################################################################
THE_INCLUDE      = -I./  \
				           -I$(WORK_HOME)/include/mdb/ \
                   -I$(WORK_HOME)/include \
                   -I ../base \
                   -I ../param \
                   -I ../local \
                   -I ../bill  \
                   -I ./mgr    \
                   -I../JSParser \
                   -I$(WORK_HOME)/include/mdb_param \
                   -I$(WORK_HOME)/include/pmbilling \
                   -I$(WORK_HOME)/include/mdb_bill \
                   -I ../../common

THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
                   -L$(WORK_HOME)/lib/mdb

THE_LIB          = -lbase -lpthread -lmdbSunam -llog4cpp

################################################################################
CXXFLAGS = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -D__PROV${PROV_CODE}__
CCFLAGS  = ${THE_INCLUDE} $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) -D__PROV${PROV_CODE}__ 
LDFLAGS  = ${THE_LIBPATH} $(SYSCOMP_LIBPATH) $(ORA_LIBPATH)
LDLIB    = ${THE_LIB}

################################################################################
include $(BUDE_HOME)/stdmake/stdmk
