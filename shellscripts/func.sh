
function getline()
{
    local i=0
    while read line 
    do
       let i++ 
        if (( $i > 10)) 
        then
            echo "已经超过10行"
            return 3
        fi
    done < $1
    echo "$file 共有$i行"
}

file=copy.c
getline $file

echo 结束状态 $?

if [ $? -eq 3 ] 
then
    echo "提前结束"
else
    echo "正常结束"
fi

appendfile()
{
    echo $2 >> $1
}
appendfile 1.txt "good morning"

echo "$@"
echo "$*"
