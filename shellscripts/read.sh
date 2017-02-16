read -p "input your name: " name
echo "hi $name, welcome to "

if read -t 5 -n  1 -p "Please enter your name: " name
then
    echo $name
else
    echo "timeout!"
fi

count=0
while read line
do
    let count++
    echo "Line $count: $line"
done < copy.c
