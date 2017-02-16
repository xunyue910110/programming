#!/bin/sh
# $1 = 文档生成工具
# $2 = 文档提交的目录
#For make doc
#set -x
BIN_DOC=$1
VERSION=$2
DEST_DOC=$3
HEAD=$4
FOOT=$5
INPUT=`pwd`
CONFIG=`pwd`/.Doxyfile

#1.检测是否工具doxygen,perl,dot
PERL=`which perl`
DOT=`which dot`
DOTPATH=`dirname ${DOT}`
DOTFILE_DIRECTED=`dirname ${DOTPATH}`/share/graphviz/graphs/directed 
DOTFILE_UNDIRECTED=`dirname ${DOTPATH}`/share/graphviz/graphs/undirected

if [ ! -x "`which ${BIN_DOC}`" ]; then
    echo "${BIN_DOC} can't execute!!!"
    exit -1;
fi
if [ ! -x "`which ${PERL}`" ]; then
    echo "perl not found!!!"
    exit -1;
fi
if [ ! -x "`which ${DOT}`" ]; then
    echo "dot not found!!!"
    exit -1;
fi
if [ ! -d "${DOTFILE_DIRECTED}" ]; then
    echo "`dirname ${DOT}`/../share/graphviz/graphs/directed not found!!!"
    exit -1;
fi
if [ ! -d "${DOTFILE_UNDIRECTED}" ]; then
    echo "`dirname ${DOT}`/../share/graphviz/graphs/undirected not found!!!"
    exit -1;
fi

#2.使用的工具说明
echo "#######################################"
echo "Using Environment:"
echo "BIN_DOC   = ${BIN_DOC}"
echo "HEAD      = ${HEAD}"
echo "FOOT      = ${FOOT}"
echo "INPUT     = ${INPUT}"
echo "DEST_DOC  = ${DEST_DOC}"
echo "PERL      = ${PERL}"
echo "DOT       = ${DOT}"
echo "DOTPATH   = ${DOTPATH}"
echo "DOTFILE_DIRECTED  = ${DOTFILE_DIRECTED}"
echo "DOTFILE_UNDIRECTED= ${DOTFILE_UNDIRECTED}"
echo "#######################################"

#3.生成配置文件Doxyfile
echo "generate config..."
cat >${CONFIG} << _EOF_
PROJECT_NAME           = "〈`basename ${DEST_DOC}`〉"
PROJECT_NUMBER         = "版本号：${VERSION}"
OUTPUT_DIRECTORY       = ${DEST_DOC}
OUTPUT_LANGUAGE        = Chinese
USE_WINDOWS_ENCODING   = NO
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        = 
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = YES
MULTILINE_CPP_IS_BRIEF = NO
DETAILS_AT_TOP         = NO
INHERIT_DOCS           = YES
DISTRIBUTE_GROUP_DOC   = NO
TAB_SIZE               = 8
ALIASES                = 
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
SUBGROUPING            = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = NO
EXTRACT_LOCAL_CLASSES  = YES
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       = 
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_FORMAT            = "\$file:\$line: \$text"
WARN_LOGFILE           = 
INPUT                  = ${INPUT}
FILE_PATTERNS          = 
RECURSIVE              = YES
EXCLUDE                = 
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = 
EXAMPLE_PATH           = "."
EXAMPLE_PATTERNS       = 
EXAMPLE_RECURSIVE      = YES
IMAGE_PATH             = 
INPUT_FILTER           = 
FILTER_SOURCE_FILES    = NO
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = YES
REFERENCES_RELATION    = YES
VERBATIM_HEADERS       = YES
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          = 
GENERATE_HTML          = YES
HTML_OUTPUT            = ${DEST_DOC}/html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            = ${HEAD}
HTML_FOOTER            = ${FOOT}
HTML_STYLESHEET        = 
HTML_ALIGN_MEMBERS     = YES
GENERATE_HTMLHELP      = YES
CHM_FILE               = YES
HHC_LOCATION           = 
GENERATE_CHI           = YES
BINARY_TOC             = NO
TOC_EXPAND             = YES
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 4
GENERATE_TREEVIEW      = YES
TREEVIEW_WIDTH         = 200
GENERATE_LATEX         = YES
LATEX_OUTPUT           = ${DEST_DOC}/latex
LATEX_CMD_NAME         = latex
MAKEINDEX_CMD_NAME     = makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4wide
EXTRA_PACKAGES         = 
LATEX_HEADER           = 
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = NO
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
GENERATE_RTF           = YES
RTF_OUTPUT             = ${DEST_DOC}/rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = YES
RTF_STYLESHEET_FILE    = 
RTF_EXTENSIONS_FILE    = 
GENERATE_MAN           = NO
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_LINKS              = NO
GENERATE_XML           = NO
XML_OUTPUT             = xml
XML_SCHEMA             = 
XML_DTD                = 
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = YES
PERLMOD_LATEX          = YES
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX = 
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = NO
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           = 
INCLUDE_FILE_PATTERNS  = 
PREDEFINED             = 
EXPAND_AS_DEFINED      = 
SKIP_FUNCTION_MACROS   = YES
TAGFILES               = 
GENERATE_TAGFILE       = 
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = ${PERL}
CLASS_DIAGRAMS         = YES
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = YES
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
UML_LOOK               = YES
TEMPLATE_RELATIONS     = YES
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = YES
GRAPHICAL_HIERARCHY    = YES
DOT_IMAGE_FORMAT       = png
DOT_PATH               = ${DOTPATH}
DOTFILE_DIRS           = "${DOTFILE_DIRECTED}" "${DOTFILE_UNDIRECTED}"
MAX_DOT_GRAPH_WIDTH    = 1024
MAX_DOT_GRAPH_HEIGHT   = 1024
MAX_DOT_GRAPH_DEPTH    = 0
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
SEARCHENGINE           = NO
_EOF_

#4.清除以前的文档
echo "rm -rf ${DEST_DOC}/html"
rm -rf ${DEST_DOC}/html
echo "rm -rf ${DEST_DOC}/latex"
rm -rf ${DEST_DOC}/latex
echo "rm -rf ${DEST_DOC}/rtf"
rm -rf ${DEST_DOC}/rtf

#5.运行${BIN_DOC} Doxyfile
echo ${BIN_DOC} ${CONFIG}
${BIN_DOC} ${CONFIG}
