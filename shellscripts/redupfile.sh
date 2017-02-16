#!/bin/bash

#used to remove duplicated files in the directory

ls -lS $1 | awk 'BEGIN{
    getline;
    getline;
    name1=$9;
    size=$5;
}
{
    name2=$9;
    if (size == $5)
    {
        com1="md5sum "name1;
        com2="md5sum "name2;
    }
    com1 |getline; csum1=$1;
    com2 |getline; csum2=$1;
    if (csum1 == csum2)
    {
        print name1;
        print name2;
    }
    size=$5; name1=name2;
}
'|sort -u > duplicated_files

cat duplicated_files|xargs -I {} md5sum {} |sort |uniq -w 32 |awk '{print $2}' |sort -u > duplicated_sample

echo Removing...
comm duplicate_files duplicated_sample -2 -3 |tee /dev/stderr |xargs rm
echo Removed duplicateds files successfully.
