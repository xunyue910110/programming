#!/bin/bash
# used to statistic file types in designated directory
declare -A stat
while read line
do
  filetype=`file -b $line|cut -d, -f1`
   let stat["$filetype"]++
done < <(
find $1 -type f -print);

for ftype in "${!stat[@]}"
do
    echo "$ftype": ${stat["$ftype"]}
done
