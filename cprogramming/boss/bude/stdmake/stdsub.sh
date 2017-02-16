#!/bin/sh

# $1 = make 命令
# $2 = make 目标
# 使用$1来编译$2的目标

if [ "x$1" = "x" -o "x$3" = "x" ]; then
    exit -1;
fi

makecommand=$1
target=$2

parent_dir="`pwd`"

echo ""

while [ $3 ]
do
	dir=${parent_dir}/$3

    if [ ! -d "${dir}" ]; then 
        echo "** ${dir} 不是目录或不存在！**";
        shift;
        continue;
    fi
    if [ ! -f "${dir}/makefile" -a ! -f "${dir}/Makefile" ]; then 
        echo "** ${dir}/makefile或${dir}/Makefile不存在！**"
        shift;
        continue;
    fi

    echo ">>> ${dir}"; 
    echo "${makecommand} ${target}";
    cd ${dir};${makecommand} ${target};
    echo ""    
    shift
done

cd ${parent_dir}
echo "<<< ${parent_dir}"


