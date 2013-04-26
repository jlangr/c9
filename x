#!/bin/bash

export COUNT=5000
if [[ -n "$1" ]]; then
   export COUNT=$1
fi

for ((i = 1; i <= $COUNT; i++))
do
#   echo -n $i-
   build/utest >/dev/null 2>out.txt
   if [ "$?" -ne "0" ]; then
      echo $i failed:
      cat out.txt
      break
#   else
#      rm out.txt
   fi
done
echo 
