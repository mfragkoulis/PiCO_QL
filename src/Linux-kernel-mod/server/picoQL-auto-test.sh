#!/bin/bash

let count=0

function test {
   ((count++))
   local busy_counter=0
   local output=`cat /proc/picoQL`
   local error=""
   local code=$?
   #echo "/proc/picoQL read returns code $code."
   while [ "$code" -eq -11 ]; do
       ((busy_counter++))
       sleep 1
       echo "Busy loop waiting for query output; $busy_counter seconds."
       output=`cat /proc/picoQL`
       code=$?
       #if [ "$counter" -gt 30 ]; then
           #output="Waited 30 seconds for query output. Someone else might have consumed it or an error happened. Check system or picoQL log."
       #fi
   done
   if [ "$code" -ne 0 ]; then
       error="/proc/picoQL read returned with error code $code."
   fi
   echo "" > temp
   echo "" >> total
   echo "$output" >> temp
   echo "$error" >> temp
   echo "$output" >> total
   echo "$error" >> total
   if ! egrep -q -i ' rows in result set.' temp
   then
       echo "Test $count failed."
       echo "The reason is: "
       echo "$error"
   else
       echo "Test $count successful."
   fi
   echo " " > temp
}


exec 7<&0
exec < $1


# Activate metadata to facilitate testing.
# Script's source code is available. 
# Perhaps this is its first time of use, so compile to be certain.
gcc -g picoQL_config_ioctl.c -o picoQL_config_ioctl
echo "Calling picoQL ioctl to activate query output metadata."
./picoQL_config_ioctl 111

echo " "

kvm_is_running=$(cat /proc/modules | grep 'kvm')

while read LINE; do
   if [[ $LINE != *KVM* ]] || [ "$kvm_is_running" != "" ]
   then
     echo "$LINE" > /proc/picoQL
     sleep 1
     test
   else
     echo "Test not available: kvm not running in this system."
   fi

done

exec 7>&-
