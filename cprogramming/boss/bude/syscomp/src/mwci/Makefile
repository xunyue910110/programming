
################################################################################
#环境相关的宏定义
#  该Makefile配置必须依赖一组关联的配置文件，这些配置文件默认存放在
#  ${BUDE_HOME}/stdmake 下。
################################################################################

# 设置系统要依赖的环境变量
include $(BUDE_HOME)/stdmake/stdenv

# 设置当前组建或应用应属的工作目录
WORK_HOME = ${BUDE_HOME}/syscomp/


################################################################################
#编译源和生成物定义
################################################################################

# 需要关联编译当前目录下子目录组建或应用。子目录名,用空格分隔多个子目录
SUBDIRS      =

# 所有需要要编译的源文件,用空格分隔,用\换行
SRCS         = CBuffer.cpp CCallerFactory.cpp CDBNameServiceImpl.cpp CFmlBuf.cpp CLNSNaming.cpp CLocalNameServiceImpl.cpp CMutex.cpp CNameService.cpp CNaming.cpp CRemoteNameServiceImpl.cpp CTuxCaller.cpp  IMWCaller.cpp LocalParseN.cpp ${TPINIT_O}

#要提交到$(DESTINC)的头文件,用空格分隔,用\换行
#定义要提交的头文件。一般只有对其他组建或应用开放的头文件进行提交
SUBMIT_HDRS  = *.h

#指定要提交到$(DESTSRC)的makefile文件名称
#makefile,Makefile默认提交，其他文件名需要在下面指定
#SUBMIT_MAKEFILE =

#生成的目标程序
PROGRAM        =
#生成的目标静态库
STATIC_LIBRARY =
#生成的目标动态库
SHARE_LIBRARY  = libmwci.$(SHARE_LIB_SUFFIX)


#提交时, 可执行文件所要拷贝的目录
DEST    = $(WORK_HOME)/bin
#提交时, 库文件所要拷贝的目录
DESTLIB = $(WORK_HOME)/lib
#提交时, 头文件所要拷贝的目录
DESTINC = $(WORK_HOME)/include/mwci
#提交时, 源代码所要拷贝的目录
DESTSRC =
#提交时, 文档所要拷贝的目录
DESTDOC =


################################################################################
#该组建或应用所要依赖的头和库文件的目录
################################################################################
THE_INCLUDE      = -I./  -I../
THE_LIBPATH      = -L./ -L../lib/
#THE_LIB          = -ludbi -llog4cpp -lwsc -ltux -lbuft  -lfml -lfml32 -lengine \
                  -lpthread -lnsl

THE_LIB          = -ludbi -llog4cpp -ltux -lfml -lfml32 -lengine -lpthread

################################################################################
##编译相关的参数定义
################################################################################
CXXFLAGS = ${THE_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
CCFLAGS  = ${THE_INCLUDE} ${TUX_INCLUDE} ${SYSCOMP_INCLUDE}
LDFLAGS  = ${THE_LIBPATH} ${TUX_LIBPATH} ${SYSCOMP_LIBPATH}

#指定该组件或应用所依赖的库名
LDLIB    = ${THE_LIB}

###############################################################################
##编译的makefile定义，不要修改
################################################################################
#包含预定义的 makefile
include $(BUDE_HOME)/stdmake/stdmk
