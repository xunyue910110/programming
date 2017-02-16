count=0;
start=$(date +%s)
echo -n count:
tput sc 

while true
do
    if [ $count -lt 15 ]
    then
        let count++
        tput rc
       # tput ed
        echo -n $count
        sleep 1
    else 
        break;
    fi

done
end=$(date +%s)
diff=$(( end - start))
echo "Time taken to execute is $diff";
