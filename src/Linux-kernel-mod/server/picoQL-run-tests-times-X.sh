#!/bin/bash

cd /home/mfg/Documents/workspace/PiCO_QL/src/Linux-kernel-mod/server
exec 9>/var/lock/picoQL-tests
if ! flock -n 9  ; then
    echo "another instance is running on `date`"
    exit 1
else
    echo "restarting script on `date`"
fi

if ! cat /proc/modules | grep -q pico ; then
    insmod picoQL.ko
fi

eval_times=$1
i=0
if [ "$1" == "-1" ]
then
  while true ; do
    echo ""
    echo "**Test iteration: $i."
    ./picoQL-auto-test.sh picoQL-tests.sql >> picoQL-runner.log
    if [ "$?" = "0" ]
    then
      ((i++))
    else
      echo "Auto tests failed."
      #echo "Exiting now."
      #exit 1
    fi
  done
else
  while [ "$i" -lt "$eval_times" ] ; do
    echo ""
    echo "\\n**Test iteration: $i."
    echo ""
    ./picoQL-auto-test.sh picoQL-tests.sql >> picoQL-runner.log
    if [ "$?" = "0" ]
    then
      ((i++))
    else
      echo "Auto tests failed."
      #echo "Exiting now."
      #exit 1
    fi
  done
fi
echo "Script finished on `date`."
