#!/bin/sh

# $1 = dir
# $2 = files
# 拷贝，从$2拷贝到$1

#相同目录下,不拷贝
if [ "x$PWD" = "x$1" -o "x$1" = "x"  ]; then
        exit -1;
fi

szDires=$1
mkdir -p $szDires
while [ $2 ]
do
    echo "	cp -f -r $2 $szDires"
    cp -f -r $2 $szDires
    shift
done
