#!/bin/bash

RESULT_SET=`cat /proc/picoQL`
#echo "$RESULT_SET"
ERRNO=`echo $?`
#echo "$ERRNO"
while [ "$ERRNO" != '0' ]
do
sleep 1
RESULT_SET=`cat /proc/picoQL`
#echo "$RESULT_SET"
ERRNO=`echo $?`
#echo "$ERRNO"
done
echo "$RESULT_SET"
