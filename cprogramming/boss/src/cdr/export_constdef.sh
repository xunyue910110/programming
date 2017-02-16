#!/bin/sh

TMP_PATH=/tmp/
BUDP_HOME=/billing
grep "^const int ATTRI." AttrIdConstDef.h |cut -d" " -f,3-100|cut -d";" -f,1 |grep -v "+"|awk '{print " export " $1$2$3$4 " "}' > ${TMP_PATH}/attrib.base.def
. ${TMP_PATH}/attrib.base.def
grep "^const int ATTRI." AttrIdConstDef.h |cut -d" " -f,3-100|cut -d";" -f,1 |grep "+"|awk '{print "" $1 "\t= "}' > ${TMP_PATH}/attrib.tmp.1
grep "^const int ATTRI." AttrIdConstDef.h |cut -d" " -f,3-100|cut -d";" -f,1 |grep "+"|awk '{print " expr" " ${"$3"} " $4" " $5" "}' > ${TMP_PATH}/attrib.tmp.2
sh ${TMP_PATH}/attrib.tmp.2 > ${TMP_PATH}/attrib.tmp.3
paste  ${TMP_PATH}/attrib.tmp.1 ${TMP_PATH}/attrib.tmp.3 > ${TMP_PATH}/attrib.tmp.4
cat ${TMP_PATH}/attrib.base.def|cut -d" " -f,3 > ${BUDP_HOME}/etc/rate.CDR_ATTRIBUTES_MACRO.cfg
cat ${TMP_PATH}/attrib.tmp.4 >> ${BUDP_HOME}/etc/rate.CDR_ATTRIBUTES_MACRO.cfg
