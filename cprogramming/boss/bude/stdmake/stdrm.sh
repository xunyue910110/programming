#!/bin/sh

# $1 = dir
# $2 = files
# 删除文件：$1/$2

if [ "x$1" = "x" ]; then
    exit -1;
fi

szDires=$1
while [ $2 ]
do
    echo "	rm -rf $szDires/$2"
    rm -rf $szDires/$2
    shift
done
