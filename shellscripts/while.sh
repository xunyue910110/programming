i=1
s=0
for ((;i<=100; i++))
do
    [ $i -eq 10 ] && break;
    let s+=$i
done
echo $s

i=1
s=0
while ((i<=100))
do
    let s+=$i 
    let i++
done
echo $s

i=1
s=0
until((i>100))
do
    let s+=$i
    let i++
done
echo $s

while read line
do
    echo $line
done < while.sh

IFS=":"
while read f1 f2 f3 f4 f5 f6 f7
do
    echo  -e "account: $f1\tlogin shell: $f7"
done < /etc/passwd

#while :
#do
#    echo 1
#done

#until false
#do
#    echo 2
#done


