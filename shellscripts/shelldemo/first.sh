#!/bin/bash
for file in *
do 
    if grep -q strict $file
    then
        echo $file
    fi
done

exit 0
