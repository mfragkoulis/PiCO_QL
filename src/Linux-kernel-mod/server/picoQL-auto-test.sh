#!/bin/bash


function test {
   output=`cat /proc/picoQL`
   while [ ! -n "$output" ]; do
       sleep 1
       output=`cat /proc/picoQL`
   done
   echo "$output" > temp
   echo "$output" >> total
   if ! egrep -q -i ' rows in result set.' temp
   then
       echo "Auto-tests failed."
       echo "The reason is:\n  $output"
       echo "Exiting now."
       #exit 1
   fi
   echo " " > temp
}


exec 7<&0
exec < $1

let count=0

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
     ((count++))
     echo "Test $count successful." 
     echo ""
     echo "PiCO QL processes running:"
     echo "`ps aux | grep pico`"
   else
     echo "Test not available: kvm not running in this system."
   fi

done

exec 7>&-
