#!/bin/bash

eval_times=$1
i=0
if [ "$1" == "-1" ]
then
  while true ; do
    echo "" >> picoQL-run-tests-summary.dat
    echo "**Test iteration: $i." >> picoQL-run-tests-summary.dat
    ./picoQL-auto-test.sh picoQL-tests.sql >> picoQL-run-tests-summary.dat
    if [ "$?" = "0" ]
    then
      ((i++))
    else
      echo "Auto tests failed."
      echo "Exiting now."
      exit 1
    fi
  done
else
  while [ "$i" -lt "$eval_times" ] ; do
    echo "" >> picoQL-run-tests-summary.dat
    echo "\\n**Test iteration: $i." >> picoQL-run-tests-summary.dat
    ./picoQL-auto-test.sh picoQL-tests.sql >> picoQL-run-tests-summary.dat
    if [ "$?" = "0" ]
    then
      ((i++))
    else
      echo "Auto tests failed."
      echo "Exiting now."
      exit 1
    fi
  done
fi
