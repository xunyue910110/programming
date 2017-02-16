function gcd()
{
    local a=$1;
    local b=$2;
    local left=$((a%b))
#    echo $a
#    echo $b
#    echo $left
    if [ $left -eq 0 ] 
    then
        echo $b
    else
        gcd $b $left
    fi
}

gcd $1 $2
#gcd 32  144 
#echo $?;
