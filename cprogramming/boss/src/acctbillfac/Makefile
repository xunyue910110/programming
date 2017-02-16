################################################################################
## 包含BUDE基础开发环境MAKEFILE全局变量
################################################################################
include $(BUDE_HOME)/stdmake/stdenv


################################################################################
## 子目录make的编译，请输入子目录名
################################################################################
SUBDIRS          =
AFTER_SUMMIT     =


################################################################################
## 编译相关的文件定义
################################################################################
# 所有的源文件,用空格分隔,用\换行
SRCS = AcctBillFacInputFile.cpp AcctBillFacOutputFile.cpp AcctBillFacApp.cpp

# 定义要提交的头文件
SUBMIT_HDRS      =

# 生成目标文件
PROGRAM          = acctbillfac

# 生成的目标静态库
STATIC_LIBRARY   =

# 生成的目标动态库
SHARE_LIBRARY    =


################################################################################
## 提交目录的定义，可以为空
################################################################################
# 提交时, 可执行文件所要拷贝的目录
DEST    = $(WORK_HOME)/bin

# 提交时, 库文件所要拷贝的目录
DESTLIB = $(WORK_HOME)/lib

# 提交时, 头文件所要拷贝的目录
DESTINC =

# 提交时, 源代码所要拷贝的目录
DESTSRC =

# 提交时, 文档所要拷贝的目录
DESTDOC =


################################################################################
## 编译相关的参数定义
################################################################################
# 编译时需要的BUDE基础开发环境组件库名
BUDE_LIB      = -lapp -lbase -llog4cpp -loradbi

# 定义应用程序包含的头文件目录
THE_INCLUDE      =

# 定义应用程序包含的库文件目录
THE_LIBPATH      =

# 指定应用程序包含的库文件
THE_LIB          =

# 内部已经包含平台的头目录,这里指定编译需要额外的参数
# 编译时,可以选择的参数:
# 动态库文件     $(SHARE_CXX_FLAGS)
# ORACLE数据库   $(ORA_INCLUDE)
# SYBASE数据库   $(SYB_INCLUDE)
# BUDE基础组件库 $(SYSCOMP_INCLUDE)
# 举例:
# CXXFLAGS = $(ORA_INCLUDE) $(SYSCOMP_INCLUDE) $(THE_INCLUDE) -D_DEBUG_ \
#            -DFP_DEBUG -DXP_WIN -DJSFILE -DDEBUG -DXP_OS2
CXXFLAGS = $(ORA_INCLUDE) $(SYSCOMP_INCLUDE) $(THE_INCLUDE) -DDEBUG

# 内部已经包含平台的库目录,这里指定连接时需要额外的参数
# 编译时,可以选择的参数:
# ORACLE数据库   $(ORA_LIBPATH)
# SYBASE数据库   $(SYB_LIBPATH)
# BUDE基础组件库 $(SYSCOMP_INCLUDE)
# 举例:
# LDFLAGS  = $(ORA_LIBPATH) $(SYSCOMP_LIBPATH) $(THE_LIBPATH)
LDFLAGS  = $(ORA_LIBPATH) $(SYSCOMP_LIBPATH) $(THE_LIBPATH) 

# 指定需要的库名
# 编译时,可以选择的参数:
# ORACLE数据库   $(ORA_LIB)
# SYBASE数据库   $(SYB_LIB)
# BUDE基础组件库 $(BUDE_LIB)
# 举例:
# LDLIB  = $(THE_LIB) $(BUDE_LIB) $(ORA_LIB) \
#          -lnsl -lsocket -lclntsh -lclntst9 -lm
LDLIB    = $(THE_LIB) $(BUDE_LIB) $(ORA_LIB)   


################################################################################
## 编译的makefile定义，不要修改
################################################################################
# 包含预定义的 makefile
include $(BUDE_HOME)/stdmake/stdmk
