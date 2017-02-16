################################################################################
#环境相关的宏定义
#  该Makefile配置必须依赖一组关联的配置文件，这些配置文件默认存放在 
#  ${BUDE_HOME}/stdmake 下。
################################################################################

include $(BUDE_HOME)/stdmake/stdenv
WORK_HOME = ${BUDE_HOME}/syscomp


################################################################################
#编译源和生成物定义：子目录、源文件
################################################################################

SUBDIRS =
SRCS    = StaticDAO.cpp SingleDAO.cpp DynamicDAO.cpp

#提交内容：头文件、Makefile/makefile
SUBMIT_HDRS  = StaticDAO.h SingleDAO.h DynamicDAO.h

#生成结果
PROGRAM        = 
STATIC_LIBRARY = 
SHARE_LIBRARY  = libpubDAO.${SHARE_LIB_SUFFIX} 

#结果提交至目录
DEST    =
DESTLIB = $(WORK_HOME)/lib
DESTINC = $(WORK_HOME)/include/pubDao
DESTSRC =
DESTDOC = 


################################################################################
##编译相关的参数定义
################################################################################
THE_INCLUDE      = -I$(WORK_HOME)/include/mwci -I$(TUXDIR)/include
THE_LIBPATH      = 
THE_LIB          = -ludbiEx

CXXFLAGS= $(SYSCOMP_INCLUDE) $(ORA_INCLUDE) $(TUX_INCLUDE) $(THE_INCLUDE)
LDFLAGS = $(SYSCOMP_LIBPATH) $(ORA_LIBPATH) $(TUX_LIBPATH)
LDLIB   = $(SYSCOMP_MWCI_LIB) $(ORA_LIB) $(TUX_LIB)

###############################################################################
##编译的makefile定义，不要修改
################################################################################
include $(BUDE_HOME)/stdmake/stdmk



