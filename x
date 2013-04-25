#!/bin/bash

for ((i = 1; i <= 5000; i++))
do
#   build/utest >/dev/null
   echo -n $i-
   build/utest # >out.txt
   if [ "$?" -ne "0" ]; then
#      cat out.txt
      echo failed at $i
      break
#   else
#      rm out.txt
   fi
done

