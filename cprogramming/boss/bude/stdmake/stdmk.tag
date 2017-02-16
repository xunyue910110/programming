
################################################################
#错误时的缺省输出
.DEFAULT:
	@echo 下列指令有效::
	@echo  "  make            - 等效于make debug                  "
	@echo  "  make  all       - 一次完成清除、编译                "
	@echo  "  make  debug     - 调试编译,不提交,不清除已提交文件  "
	@echo  "  make  normal    - 普通编译,不提交,不清除已提交文件  "
	@echo  "  make  release   - 优化编译,不提交,不清除已提交文件  "
	@echo  "  make  submit    - 提交头文件,源代码及目标文件       "
	@echo  "  make  unsubmit  - 卸载提交的头文件,源代码及目标文件 "
	@echo  "  make  clean     - 清除中间文件及目标文件            "
	@echo  "  make  doc       - 生成并提交文档                    "
	@echo  "  make  help      - 打印帮助信息                      "

#make缺省动作
defaultMiss: debug
	@echo
################################################################
#因为有的机器上 rule的名字不能为空
NotNullPROGRAM        = $(PROGRAM) NotNullTempStr1
NotNullSTATIC_LIBRARY = $(STATIC_LIBRARY) NotNullTempStr2
NotNullSHARE_LIBRARY  = $(SHARE_LIBRARY) NotNullTempStr3
_tmp_object_files_=${SRCS:.cpp=.o}
_object_files_=${_tmp_object_files_:.c=.o}

################################################################
#所有的make假目标隐藏定义
HIDDEN_TARGET = all release debug normal clean clear submit doc  \
                _all_  _release_  _debug_  _normal_  _clean_  _clear_  _submit_  _doc_ \
                _sub_all_  _sub_release_  _sub_debug_  _sub_normal_  _sub_clean_  _sub_clear_  _sub_submit_  _sub_doc_ \
                _release_obj  _debug_obj  _normal_obj  _release_clean  _debug_clean  _normal_clean
.PHONY: $(HIDDEN_TARGET)

################################################################
#所有的make 目标，这里提供对子目录的支持和完成时信息的提示功能
all : _sub_all_  _all_
	@echo All Done
_sub_all_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "all _submit_" $(SUBDIRS)); \
	fi

release : _sub_release_  _release_
	@echo Release done
_sub_release_ :
	@if [ "x$(BEFORE_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "release" $(BEFORE_MAKE)); \
	fi
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "release _submit_" $(SUBDIRS)); \
	fi

debug : _sub_debug_  _debug_
	@echo Debug done
_sub_debug_ :
	@if [ "x$(BEFORE_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "debug" $(BEFORE_MAKE)); \
	fi
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "debug _submit_" $(SUBDIRS)); \
	fi

normal : _sub_normal_  _normal_
	@echo Normal done
_sub_normal_ :
	@if [ "x$(BEFORE_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "normal" $(BEFORE_MAKE)); \
	fi
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "normal _submit_" $(SUBDIRS)); \
	fi

clean : _sub_clean_  _clean_
_sub_clean_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "clean" $(SUBDIRS)); \
	fi

clear : .DEFAULT
#clear : _sub_clear_  _clear_
_sub_clear_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "clear" $(SUBDIRS)); \
	fi

unsubmit : _sub_unsubmit_  _unsubmit_
_sub_unsubmit_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "unsubmit" $(SUBDIRS)); \
	fi

submit : _sub_submit_  _submit_
	@echo Submit Done
_sub_submit_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "submit" $(SUBDIRS)); \
	fi

doc : _sub_doc_  _doc_
	@echo Doc Done
_sub_doc_ :
	@if [ "x$(SUBDIRS)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdsub.sh $(MAKE) "doc" $(SUBDIRS)); \
	fi
################################################################
#对各个目标进行定义
_all_ : _clean_ _release_

_release_ : _release_obj $(NotNullSTATIC_LIBRARY) $(NotNullSHARE_LIBRARY) $(NotNullPROGRAM)
	@if [ "x$(AFTER_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "release" $(AFTER_MAKE)); \
	fi

_debug_ : _debug_obj $(NotNullSTATIC_LIBRARY) $(NotNullSHARE_LIBRARY) $(NotNullPROGRAM)
	@if [ "x$(AFTER_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "debug" $(AFTER_MAKE)); \
	fi

_normal_ : _normal_obj $(NotNullSTATIC_LIBRARY) $(NotNullSHARE_LIBRARY) $(NotNullPROGRAM)
	@if [ "x$(AFTER_MAKE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdshell.sh "normal" $(AFTER_MAKE)); \
	fi

_clean_ : _debug_clean _release_clean _normal_clean
	@$(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}
	@if [ "x$(CXXCACHE)" != "x$(CCCACHE)" ]; then \
	echo rm -rf $(PROGRAM) core $(CXXCACHE) $(CCCACHE) $(STATIC_LIBRARY) $(SHARE_LIBRARY); \
	rm -rf $(PROGRAM) core $(CXXCACHE) $(CCCACHE) $(STATIC_LIBRARY) $(SHARE_LIBRARY); \
	else \
	echo rm -rf $(PROGRAM) core $(CXXCACHE) $(STATIC_LIBRARY) $(SHARE_LIBRARY); \
	rm -rf $(PROGRAM) core $(CXXCACHE) $(STATIC_LIBRARY) $(SHARE_LIBRARY); \
	fi

_unsubmit_ :
	@if [ "x$(DEST)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdrm.sh $(DEST) $(PROGRAM));\
	fi
	@if [ "x$(DESTINC)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdrm.sh $(DESTINC) $(SUBMIT_HDRS));\
	fi
	@if [ "x$(DESTSRC)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdrm.sh $(DESTSRC) $(SRCS));\
	fi
	@if [ "x$(DESTLIB)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdrm.sh $(DESTLIB) $(STATIC_LIBRARY) $(SHARE_LIBRARY));\
	fi

_clear_ : _clean_ _unsubmit_


_submit_ :
	@if [ "x$(BEFORE_SUBMIT)" != "x" ]; then \
	(echo execute command "${BEFORE_SUBMIT}"); \
	(eval ${BEFORE_SUBMIT}); \
	fi

	@if [ "x$(STATIC_LIBRARY)" != "x" ]; then \
	if [ "x$(DESTLIB)" != "x." ]; then \
	if [ "x$(DESTLIB)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTLIB) $(STATIC_LIBRARY)); \
	fi \
	fi \
	fi

	@if [ "x$(SHARE_LIBRARY)" != "x" ]; then \
	if [ "x$(DESTLIB)" != "x." ]; then \
	if [ "x$(DESTLIB)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTLIB) $(SHARE_LIBRARY)); \
	fi \
	fi \
	fi

	@if [ "x$(PROGRAM)" != "x" ]; then \
	if [ "x$(DEST)" != "x." ]; then \
	if [ "x$(DEST)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DEST) $(PROGRAM)); \
	fi \
	fi \
	fi

#提交所有的头文件和src文件
	@if [ "x$(DESTSRC)" != "x." ]; then \
	if [ "x$(DESTSRC)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTSRC) $(SRCS)); \
	fi \
	fi
	@if [ "x$(DESTSRC)" != "x." ]; then \
	if [ "x$(DESTSRC)" != "x" ]; then \
	if [ -f "makefile" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTSRC) "makefile"); \
	fi \
	fi \
	fi
	@if [ "x$(DESTSRC)" != "x." ]; then \
	if [ "x$(DESTSRC)" != "x" ]; then \
	if [ -f "Makefile" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTSRC) "Makefile"); \
	fi \
	fi \
	fi
	@if [ "x$(DESTSRC)" != "x." ]; then \
	if [ "x$(DESTSRC)" != "x" ]; then \
	if [ "x$(SUBMIT_MAKEFILE)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTSRC) $(SUBMIT_MAKEFILE)); \
	fi \
	fi \
	fi
	@if [ "x$(DESTINC)" != "x." ]; then \
	if [ "x$(DESTINC)" != "x" ]; then \
	($(STDMAKE_ROOT)/stdcp.sh $(DESTINC) $(SUBMIT_HDRS)); \
	fi \
	fi

	@if [ "x$(AFTER_SUBMIT)" != "x" ]; then \
	(echo execute command "${AFTER_SUBMIT}"); \
	(eval ${AFTER_SUBMIT}); \
	fi

#生成并提交文档
_doc_ :
	@if [ "x$(DESTDOC)" != "x." ]; then \
	if [ "x$(DESTDOC)" != "x" -a "x$(STDMAKE_DOC)" != "x" ]; then \
	($(STDMAKE_ROOT)/stddoc.sh $(STDMAKE_DOC) $(VERSION) $(DESTDOC) $(STDMAKE_DOC_HEAD) $(STDMAKE_DOC_FOOT)); \
	fi \
	fi
################################################################
#RELEASE的分解部分一
_release_obj :
	@if [ -d "_debug" ] ; then \
	(rmdir _debug); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ -d "_normal" ] ; then \
	(rmdir _normal); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ ! -d "_release" ] ; then \
	(mkdir _release ); \
	fi

#DEBUG的分解部分一
_debug_obj :
	@if [ -d "_release" ] ; then \
	(rmdir _release); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ -d "_normal" ] ; then \
	(rmdir _normal); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ ! -d "_debug" ] ; then \
	(mkdir _debug ); \
	fi

#NORMAL的分解部分一
_normal_obj :
	@if [ -d "_release" ] ; then \
	(rmdir _release); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ -d "_debug" ] ; then \
	(rmdir _debug); \
	($(STDMAKE_ROOT)/stdrm.sh \. ${_object_files_}); \
	fi
	@if [ ! -d "_normal" ] ; then \
	(mkdir _normal ); \
	fi

#CLEAN的分解
_release_clean :
	@if [ -d "_release" ] ; then \
	(rmdir _release); \
	fi

_debug_clean :
	@if [ -d "_debug" ] ; then \
	(rmdir _debug); \
	fi

_normal_clean :
	@if [ -d "_normal" ] ; then \
	(rmdir _normal); \
	fi

################################################################
#RELEASE和DEBUG的公共分解部分
$(NotNullSHARE_LIBRARY): ${_object_files_}
	@if [ "x$@" != "xNotNullTempStr3" ]; then \
	(echo $(LD) -o $@ ${_object_files_} $(SHARE_LINK_FLAGS) $(LDFLAGS) $(STDMAKE_LIBPATH) $(STDMAKE_LIB) $(LDLIB)); \
	(echo "int main()  {return 0; }" > ._tso.cpp); \
	($(LD) -o ._tso ${_object_files_} ._tso.cpp $(LDFLAGS) $(STDMAKE_LIBPATH) $(STDMAKE_LIB) $(LDLIB)); \
	($(LD) -o $@ ${_object_files_} $(SHARE_LINK_FLAGS) $(LDFLAGS) $(STDMAKE_LIBPATH) $(STDMAKE_LIB) $(LDLIB)); \
	(rm -rf ._tso ._tso.cpp ); \
	(echo ); \
	fi

$(NotNullSTATIC_LIBRARY):${_object_files_}
	@if [ "x$@" != "xNotNullTempStr2" ]; then \
	(echo $(AR) $@ ${_object_files_}); \
	($(AR) $@ ${_object_files_}); \
	(echo ); \
	fi

$(NotNullPROGRAM): ${_object_files_}
	@if [ "x$@" != "xNotNullTempStr1" ]; then \
	(echo $(LD) -o $@ ${_object_files_} $(LDFLAGS) $(STDMAKE_LIBPATH) $(STDMAKE_LIB) $(LDLIB)); \
	($(LD) -o $@ ${_object_files_} $(LDFLAGS) $(STDMAKE_LIBPATH) $(STDMAKE_LIB) $(LDLIB) $(SHARE_LD_FLAGS)); \
	(echo ); \
	fi

################################################################
#对cpp文件的自动编译支持
.SUFFIXES: .cpp .c
.cpp.o:
	@if [ -d "_release" ] ; then \
	(echo ); \
	(echo pwd:`pwd` ); \
	(echo $(CXX) -o $@ $(STDMAKE_RELEASE) -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE)); \
	$(CXX) -o $@ $(STDMAKE_RELEASE) -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
	@if [ -d "_debug" ] ; then \
	(echo ); \
	(echo pwd:\n`pwd` ); \
	(echo $(CXX) -o $@ $(STDMAKE_DEBUG) -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE)); \
	$(CXX) -o $@ $(STDMAKE_DEBUG) -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
	@if [ -d "_normal" ] ; then \
	(echo ); \
	(echo pwd:\n`pwd` ); \
	(echo $(CXX) -o $@ -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE)); \
	$(CXX) -o $@ -c $< $(SHARE_CXX_FLAGS) $(CXXFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
.c.o:
	@if [ -d "_release" ] ; then \
	(echo ); \
	(echo pwd:`pwd` ); \
	(echo $(CC) -o $@ $(STDMAKE_RELEASE) -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE)); \
	$(CC) -o $@ $(STDMAKE_RELEASE) -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
	@if [ -d "_debug" ] ; then \
	(echo ); \
	(echo pwd:`pwd` ); \
	(echo $(CC) -o $@ $(STDMAKE_DEBUG) -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE)); \
	$(CC) -o $@ $(STDMAKE_DEBUG) -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
	@if [ -d "_normal" ] ; then \
	(echo ); \
	(echo pwd:`pwd` ); \
	(echo $(CC) -o $@ -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE)); \
	$(CC) -o $@ -c $< $(SHARE_CC_FLAGS) $(CCFLAGS) $(STDMAKE_INCLUDE); \
	(echo ); \
	fi
################################################################

