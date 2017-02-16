################################
##编译相关的文件定义
################################
#所有的源文件,用空格分隔,用\换行
SRCS	      =    PrimitiveStub.cpp \
                        LockStub.cpp \
                SharedMemoryStub.cpp \
                   SemaphoreStub.cpp \
                       QueueStub.cpp \
                       MutexStub.cpp \
                     SessionStub.cpp \
                   ClientFactory.cpp \



#子目录make的编译，请输入子目录名
SUBDIRS       =

#要提交到$(DESTINC)的头文件,用空格分隔,用\换行
#由于在cpp文件中已经include相关的头文件了,故这里只需要
#定义要提交的头文件
SUBMIT_HDRS	  =

#指定要提交到$(DESTSRC)的makefile文件名称
#makefile,Makefile默认提交，其他文件名需要在下面指定
#SUBMIT_MAKEFILE =

#生成的目标程序
PROGRAM       =

#生成的目标静态库
STATIC_LIBRARY= libnipc.client.a

#生成的目标动态库
SHARE_LIBRARY =


################################
##提交目录的定义，可以为空
################################
#提交时, 可执行文件所要拷贝的目录
DEST    =

#提交时, 库文件所要拷贝的目录
DESTLIB = $(BUDP_HOME)/lib

#提交时, 头文件所要拷贝的目录
DESTINC	= $(BUDP_HOME)/include

#提交时, 源代码所要拷贝的目录
DESTSRC	=

#提交时, 文档所要拷贝的目录
DESTDOC = $(BUDP_HOME)/doc

################################
##编译相关的参数定义
################################
#内部已经包含平台的头目录,这里指定编译需要额外的参数
#编译时,可以选择的参数:
#动态库文件     $(SHARE_CXX_FLAGS)
#ORACLE数据库   $(ORA8I_INCLUDE)
#SYBASE数据库   $(SYB_INCLUDE)
#举例:
#CXXFLAGS= $(SHARE_CXX_FLAGS) $(ORA8I_INCLUDE)
CXXFLAGS= -I../inc -I../inc/internal

#内部已经包含平台的库目录,这里指定连接时需要额外的参数
#编译时,可以选择的参数:
#ORACLE数据库   $(ORA8I_LIBPATH)
#SYBASE数据库   $(SYB_LIBPATH)
#举例:
#LDFLAGS= $(ORA8I_LIBPATH) $(SYB_LIBPATH)
LDFLAGS	=

#指定需要的库名
#编译时,可以选择的参数:
#ORACLE数据库   $(ORA8I_LIB)
#SYBASE数据库   $(SYB_LIB)
#举例:
#LDLIB  = $(ORA8I_LIB) $(SYB_LIB) -lm
LDLIB   =


################################
##编译的makefile定义，不要修改
################################
#包含预定义的 makefile
include $(BUDE_HOME)/src/stdmake/stdmk

