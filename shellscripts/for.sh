data="name,sexno,rollno,location"
oldIFS=$IFS
IFS=","
for item in $data
do
    echo $item
done
IFS=$oldIFS

line="root:x:0:0:root:/root:/bin/bash"
IFS=":"
count=0
for item in $line
do
    [ $count -eq 0 ] && user=$item
    [ $count -eq 6 ] && shell=$item
    echo $item
    let count++
done

echo $count
echo $user
echo $shell

for i in 1 2 3 4 5
do
    echo $i
done

for i in {10..20}
do
    echo $i
done

for i in $(seq 10 20)
do
    echo $i
done

for i in {a..h}
do
    echo $i
done

dir="/root"
for f in $(ls ~)
do
    echo $f
done

echo "entering $HOME"
dir=$HOME
cd $dir
for f in $(ls $dir)
do
    [ -d $f ] &&  du -s $f
done


#for((;;))
#do
#    echo $i
#done

while true
do
    echo 2
done
