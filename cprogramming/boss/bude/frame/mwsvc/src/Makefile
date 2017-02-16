
################################################################################
#环境相关的宏定义
#  该Makefile配置必须依赖一组关联的配置文件，这些配置文件默认存放在
#  ${BUDE_HOME}/stdmake 下。
################################################################################

# 设置系统要依赖的环境变量
include $(BUDE_HOME)/stdmake/stdenv

# 设置当前组建或应用应属的工作目录
WORK_HOME = ${BUDE_HOME}/frame/mwsvc


################################################################################

#编译源和生成物定义
################################################################################

# 需要关联编译当前目录下子目录组建或应用。子目录名,用空格分隔多个子目录
SUBDIRS      =

# 所有需要要编译的源文件,用空格分隔,用\换行
SRCS         = inParam_Mem.cpp InterfaceLog.cpp mwsvc.cpp ibsvc.cpp itfsvc.cpp MwsvcConf.cpp

#要提交到$(DESTINC)的头文件,用空格分隔,用\换行
#定义要提交的头文件。一般只有对其他组建或应用开放的头文件进行提交
SUBMIT_HDRS  =

#指定要提交到$(DESTSRC)的makefile文件名称
#makefile,Makefile默认提交，其他文件名需要在下面指定
#SUBMIT_MAKEFILE =

#生成的目标程序
PROGRAM        =
#生成的目标静态库
STATIC_LIBRARY =
#生成的目标动态库
SHARE_LIBRARY  =


#提交时, 可执行文件所要拷贝的目录
DEST    =
#提交时, 库文件所要拷贝的目录
DESTLIB =
#提交时, 头文件所要拷贝的目录
DESTINC =
#提交时, 源代码所要拷贝的目录
DESTSRC = $(WORK_HOME)/txdsvc
#提交时, 文档所要拷贝的目录
DESTDOC =


################################################################################
#该组建或应用所要依赖的头和库文件的目录
################################################################################
THE_INCLUDE      = -I./  \
                   -I../ \
                   -I$(WORK_HOME)/include
THE_LIBPATH      = -L./  \
                   -L$(WORK_HOME)/lib \
THE_LIB          =


################################################################################
CXXFLAGS = ${THE_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${TUX_LIBPATH} ${SYSCOMP_LIBPATH} -bmaxdata:0x20000000
LDLIB    = ${THE_LIB} ${TUX_LIB}

################################################################################

include $(BUDE_HOME)/stdmake/stdmk

BSVC = buildserver

all:$(OBJS)  itfserver bossserver

# BOSS外部接口
itfserver : $(OBJS)
	$(BSVC) -r Oracle_XA -s ITF_BKHSVC:MWSVC -o ../bin/itfbkhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_CCHSVC:MWSVC -o ../bin/itfcchserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -f MwsvcConf.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu -lxml -ldynlib -llog4cpp'
	$(BSVC) -r Oracle_XA -s ITF_CCOSVC:MWSVC -o ../bin/itfccoserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_DMOSVC:MWSVC -o ../bin/itfdmoserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_DMHSVC:MWSVC -o ../bin/itfdmhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_SPHSVC:MWSVC -o ../bin/itfsphserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_DKHSVC:MWSVC -o ../bin/itfdkhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_PBHSVC:MWSVC -o ../bin/itfpbhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_PTHSVC:MWSVC -o ../bin/itfpthserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_SMHSVC:MWSVC -o ../bin/itfsmhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_UAHSVC:MWSVC -o ../bin/itfuahserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_IBHSVC:MWSVC -o ../bin/itfibhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_IBOSVC:MWSVC -o ../bin/itfiboserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_MMHSVC:MWSVC -o ../bin/itfmmhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_VAHSVC:MWSVC -o ../bin/itfvahserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_SBHSVC:MWSVC -o ../bin/itfsbhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
#  Gaoq ADD by SHXI start  #
	$(BSVC) -r Oracle_XA -s ITF_WCHSVC:MWSVC -o ../bin/itfwchserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_WBHSVC:MWSVC -o ../bin/itfwbhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_DMHSVC:MWSVC -o ../bin/itfdmhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_MEHSVC:MWSVC -o ../bin/itfmehserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_XFHSVC:MWSVC -o ../bin/itfxfhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_PMHSVC:MWSVC -o ../bin/itfpmhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_WPHSVC:MWSVC -o ../bin/itfwphserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_ICHSVC:MWSVC -o ../bin/itfichserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_STHSVC:MWSVC -o ../bin/itfsthserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
#  Gaoq ADD by SHXI end    #
	$(BSVC) -r Oracle_XA -s ITF_VPHSVC:MWSVC -o ../bin/itfvphserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_TUHSVC:MWSVC -o ../bin/itftuhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_SBHSVC:MWSVC -o ../bin/itfsbhserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s ITF_JSHSVC:MWSVC -o ../bin/itfjshserver -f inParam_Mem.o -f itfsvc.o -f InterfaceLog.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

# BOSS内部调用
bossserver:
	$(BSVC) -r Oracle_XA -s QAM_CRM1_L1SVC:MWSVC -o ../bin/qamcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CRM1_L2SVC:MWSVC -o ../bin/qamcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CRM1_L3SVC:MWSVC -o ../bin/qamcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CRM1_L4SVC:MWSVC -o ../bin/qamcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CRM1_L1SVC:MWSVC -o ../bin/tamcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CRM1_L2SVC:MWSVC -o ../bin/tamcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CRM1_L3SVC:MWSVC -o ../bin/tamcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CRM1_L4SVC:MWSVC -o ../bin/tamcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CRM1_L1SVC:MWSVC -o ../bin/xamcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CRM1_L2SVC:MWSVC -o ../bin/xamcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CRM1_L3SVC:MWSVC -o ../bin/xamcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CRM1_L4SVC:MWSVC -o ../bin/xamcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCC_CRM1_L1SVC:MWSVC -o ../bin/qcccrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCC_CRM1_L2SVC:MWSVC -o ../bin/qcccrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCC_CRM1_L3SVC:MWSVC -o ../bin/qcccrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCC_CRM1_L4SVC:MWSVC -o ../bin/qcccrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCC_CRM1_L1SVC:MWSVC -o ../bin/tcccrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCC_CRM1_L2SVC:MWSVC -o ../bin/tcccrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCC_CRM1_L3SVC:MWSVC -o ../bin/tcccrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCC_CRM1_L4SVC:MWSVC -o ../bin/tcccrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCC_CRM1_L1SVC:MWSVC -o ../bin/xcccrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCC_CRM1_L2SVC:MWSVC -o ../bin/xcccrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCC_CRM1_L3SVC:MWSVC -o ../bin/xcccrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCC_CRM1_L4SVC:MWSVC -o ../bin/xcccrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCM_CRM1_L1SVC:MWSVC -o ../bin/qcmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCM_CRM1_L2SVC:MWSVC -o ../bin/qcmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCM_CRM1_L3SVC:MWSVC -o ../bin/qcmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCM_CRM1_L4SVC:MWSVC -o ../bin/qcmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCM_CRM1_L1SVC:MWSVC -o ../bin/tcmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCM_CRM1_L2SVC:MWSVC -o ../bin/tcmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCM_CRM1_L3SVC:MWSVC -o ../bin/tcmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCM_CRM1_L4SVC:MWSVC -o ../bin/tcmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCM_CRM1_L1SVC:MWSVC -o ../bin/xcmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCM_CRM1_L2SVC:MWSVC -o ../bin/xcmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCM_CRM1_L3SVC:MWSVC -o ../bin/xcmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCM_CRM1_L4SVC:MWSVC -o ../bin/xcmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCS_CRM1_L1SVC:MWSVC -o ../bin/qcscrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCS_CRM1_L2SVC:MWSVC -o ../bin/qcscrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCS_CRM1_L3SVC:MWSVC -o ../bin/qcscrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QCS_CRM1_L4SVC:MWSVC -o ../bin/qcscrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCS_CRM1_L1SVC:MWSVC -o ../bin/tcscrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCS_CRM1_L2SVC:MWSVC -o ../bin/tcscrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCS_CRM1_L3SVC:MWSVC -o ../bin/tcscrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TCS_CRM1_L4SVC:MWSVC -o ../bin/tcscrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCS_CRM1_L1SVC:MWSVC -o ../bin/xcscrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCS_CRM1_L2SVC:MWSVC -o ../bin/xcscrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCS_CRM1_L3SVC:MWSVC -o ../bin/xcscrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XCS_CRM1_L4SVC:MWSVC -o ../bin/xcscrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_CRM1_L1SVC:MWSVC -o ../bin/qitfcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_CRM1_L2SVC:MWSVC -o ../bin/qitfcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_CRM1_L3SVC:MWSVC -o ../bin/qitfcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_CRM1_L4SVC:MWSVC -o ../bin/qitfcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_CRM1_L1SVC:MWSVC -o ../bin/titfcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_CRM1_L2SVC:MWSVC -o ../bin/titfcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_CRM1_L3SVC:MWSVC -o ../bin/titfcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_CRM1_L4SVC:MWSVC -o ../bin/titfcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_CRM1_L1SVC:MWSVC -o ../bin/xitfcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_CRM1_L2SVC:MWSVC -o ../bin/xitfcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_CRM1_L3SVC:MWSVC -o ../bin/xitfcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_CRM1_L4SVC:MWSVC -o ../bin/xitfcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPL_CRM1_L1SVC:MWSVC -o ../bin/qplcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPL_CRM1_L2SVC:MWSVC -o ../bin/qplcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPL_CRM1_L3SVC:MWSVC -o ../bin/qplcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPL_CRM1_L4SVC:MWSVC -o ../bin/qplcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPL_CRM1_L1SVC:MWSVC -o ../bin/tplcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPL_CRM1_L2SVC:MWSVC -o ../bin/tplcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPL_CRM1_L3SVC:MWSVC -o ../bin/tplcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPL_CRM1_L4SVC:MWSVC -o ../bin/tplcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPL_CRM1_L1SVC:MWSVC -o ../bin/xplcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPL_CRM1_L2SVC:MWSVC -o ../bin/xplcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPL_CRM1_L3SVC:MWSVC -o ../bin/xplcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPL_CRM1_L4SVC:MWSVC -o ../bin/xplcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPU_CRM1_L1SVC:MWSVC -o ../bin/qpucrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPU_CRM1_L2SVC:MWSVC -o ../bin/qpucrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPU_CRM1_L3SVC:MWSVC -o ../bin/qpucrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QPU_CRM1_L4SVC:MWSVC -o ../bin/qpucrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPU_CRM1_L1SVC:MWSVC -o ../bin/tpucrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPU_CRM1_L2SVC:MWSVC -o ../bin/tpucrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPU_CRM1_L3SVC:MWSVC -o ../bin/tpucrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TPU_CRM1_L4SVC:MWSVC -o ../bin/tpucrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPU_CRM1_L1SVC:MWSVC -o ../bin/xpucrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPU_CRM1_L2SVC:MWSVC -o ../bin/xpucrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPU_CRM1_L3SVC:MWSVC -o ../bin/xpucrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XPU_CRM1_L4SVC:MWSVC -o ../bin/xpucrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRA_CRM1_L1SVC:MWSVC -o ../bin/qracrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRA_CRM1_L2SVC:MWSVC -o ../bin/qracrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRA_CRM1_L3SVC:MWSVC -o ../bin/qracrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRA_CRM1_L4SVC:MWSVC -o ../bin/qracrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRA_CRM1_L1SVC:MWSVC -o ../bin/tracrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRA_CRM1_L2SVC:MWSVC -o ../bin/tracrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRA_CRM1_L3SVC:MWSVC -o ../bin/tracrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRA_CRM1_L4SVC:MWSVC -o ../bin/tracrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRA_CRM1_L1SVC:MWSVC -o ../bin/xracrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRA_CRM1_L2SVC:MWSVC -o ../bin/xracrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRA_CRM1_L3SVC:MWSVC -o ../bin/xracrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRA_CRM1_L4SVC:MWSVC -o ../bin/xracrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_CRM1_L1SVC:MWSVC -o ../bin/qrmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_CRM1_L2SVC:MWSVC -o ../bin/qrmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_CRM1_L3SVC:MWSVC -o ../bin/qrmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_CRM1_L4SVC:MWSVC -o ../bin/qrmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRM_CRM1_L1SVC:MWSVC -o ../bin/trmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRM_CRM1_L2SVC:MWSVC -o ../bin/trmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRM_CRM1_L3SVC:MWSVC -o ../bin/trmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRM_CRM1_L4SVC:MWSVC -o ../bin/trmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRM_CRM1_L1SVC:MWSVC -o ../bin/xrmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRM_CRM1_L2SVC:MWSVC -o ../bin/xrmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRM_CRM1_L3SVC:MWSVC -o ../bin/xrmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XRM_CRM1_L4SVC:MWSVC -o ../bin/xrmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSM_CRM1_L1SVC:MWSVC -o ../bin/qsmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSM_CRM1_L2SVC:MWSVC -o ../bin/qsmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSM_CRM1_L3SVC:MWSVC -o ../bin/qsmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSM_CRM1_L4SVC:MWSVC -o ../bin/qsmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSM_CRM1_L1SVC:MWSVC -o ../bin/tsmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSM_CRM1_L2SVC:MWSVC -o ../bin/tsmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSM_CRM1_L3SVC:MWSVC -o ../bin/tsmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSM_CRM1_L4SVC:MWSVC -o ../bin/tsmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSM_CRM1_L1SVC:MWSVC -o ../bin/xsmcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSM_CRM1_L2SVC:MWSVC -o ../bin/xsmcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSM_CRM1_L3SVC:MWSVC -o ../bin/xsmcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSM_CRM1_L4SVC:MWSVC -o ../bin/xsmcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QAM_CBS1_L1SVC:MWSVC -o ../bin/qamcbs1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CBS1_L2SVC:MWSVC -o ../bin/qamcbs1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CBS1_L3SVC:MWSVC -o ../bin/qamcbs1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QAM_CBS1_L4SVC:MWSVC -o ../bin/qamcbs1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CBS1_L1SVC:MWSVC -o ../bin/tamcbs1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CBS1_L2SVC:MWSVC -o ../bin/tamcbs1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CBS1_L3SVC:MWSVC -o ../bin/tamcbs1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TAM_CBS1_L4SVC:MWSVC -o ../bin/tamcbs1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CBS1_L1SVC:MWSVC -o ../bin/xamcbs1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CBS1_L2SVC:MWSVC -o ../bin/xamcbs1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CBS1_L3SVC:MWSVC -o ../bin/xamcbs1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XAM_CBS1_L4SVC:MWSVC -o ../bin/xamcbs1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QITF_UIF_L1SVC:MWSVC -o ../bin/qitfuifl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_UIF_L2SVC:MWSVC -o ../bin/qitfuifl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_UIF_L3SVC:MWSVC -o ../bin/qitfuifl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QITF_UIF_L4SVC:MWSVC -o ../bin/qitfuifl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_UIF_L1SVC:MWSVC -o ../bin/titfuifl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_UIF_L2SVC:MWSVC -o ../bin/titfuifl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_UIF_L3SVC:MWSVC -o ../bin/titfuifl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_UIF_L4SVC:MWSVC -o ../bin/titfuifl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_UIF_L1SVC:MWSVC -o ../bin/xitfuifl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_UIF_L2SVC:MWSVC -o ../bin/xitfuifl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_UIF_L3SVC:MWSVC -o ../bin/xitfuifl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XITF_UIF_L4SVC:MWSVC -o ../bin/xitfuifl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TITF_XFK_L1SVC:MWSVC -o ../bin/titfxfkl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QSA_CEN_L1SVC:MWSVC -o ../bin/qsacenl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSA_CEN_L2SVC:MWSVC -o ../bin/qsacenl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSA_CEN_L3SVC:MWSVC -o ../bin/qsacenl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QSA_CEN_L4SVC:MWSVC -o ../bin/qsacenl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSA_CEN_L1SVC:MWSVC -o ../bin/tsacenl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSA_CEN_L2SVC:MWSVC -o ../bin/tsacenl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSA_CEN_L3SVC:MWSVC -o ../bin/tsacenl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TSA_CEN_L4SVC:MWSVC -o ../bin/tsacenl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSA_CEN_L1SVC:MWSVC -o ../bin/xsacenl1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSA_CEN_L2SVC:MWSVC -o ../bin/xsacenl2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSA_CEN_L3SVC:MWSVC -o ../bin/xsacenl3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s XSA_CEN_L4SVC:MWSVC -o ../bin/xsacenl4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QBLL_CRM1_L1SVC:MWSVC -o ../bin/qbllcrm1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CRM1_L2SVC:MWSVC -o ../bin/qbllcrm1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CRM1_L3SVC:MWSVC -o ../bin/qbllcrm1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CRM1_L4SVC:MWSVC -o ../bin/qbllcrm1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QBLL_CDR1_L1SVC:MWSVC -o ../bin/qbllcdr1l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR1_L2SVC:MWSVC -o ../bin/qbllcdr1l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR1_L3SVC:MWSVC -o ../bin/qbllcdr1l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR1_L4SVC:MWSVC -o ../bin/qbllcdr1l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QBLL_CDR2_L1SVC:MWSVC -o ../bin/qbllcdr2l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR2_L2SVC:MWSVC -o ../bin/qbllcdr2l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR2_L3SVC:MWSVC -o ../bin/qbllcdr2l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR2_L4SVC:MWSVC -o ../bin/qbllcdr2l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'

	$(BSVC) -r Oracle_XA -s QBLL_CDR3_L1SVC:MWSVC -o ../bin/qbllcdr3l1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR3_L2SVC:MWSVC -o ../bin/qbllcdr3l2server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR3_L3SVC:MWSVC -o ../bin/qbllcdr3l3server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QBLL_CDR3_L4SVC:MWSVC -o ../bin/qbllcdr3l4server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s TRM_KI_L1SVC:MWSVC -o ../bin/trmkil1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_KI_L1SVC:MWSVC -o ../bin/qrmkil1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
        $(BSVC) -r Oracle_XA -s TRM_HFK_L1SVC:MWSVC -cle_XA -s TRM_KI_L1SVC:MWSVC -o ../bin/trmkil1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
	$(BSVC) -r Oracle_XA -s QRM_KI_L1SVC:MWSVC -o ../bin/qrmkil1server -f inParam_Mem.o -f mwsvc.o -l'$(SYSCOMP_LIBPATH) -lmwci -ludbi -lmwlcu'
        $(