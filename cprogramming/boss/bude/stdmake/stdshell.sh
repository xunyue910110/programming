#!/bin/sh

# $1 = make的目标:all release debug clean等等
# $2 = shell脚本路径名
# 删除文件：$1/$2

if [ "x$1" = "x" -o "x$2" = "x" ]; then
    exit -1;
fi

#暂时支持all,release,debug
if [ "x$1" = "xall" -o "x$1" = "xrelease" -o "x$1" = "xdebug" ]; then
    if [ -f $2 ]; then
        echo "正在执行shell脚本 ..."
        exec $2
    fi
else
    echo  "对不起,现在暂时不支持该目标的shell脚本的执行!"
fi
