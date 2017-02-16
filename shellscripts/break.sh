for ((i=1; i<=10; i++))
do
    for ((j=1; j<=10; j++))
    do
        r=$((i+j))
        [ $r -eq 3 ] && break 1;
        echo $r
    done
done
