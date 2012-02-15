#!/bin/bash
if diff test_current.txt test_success.txt >/dev/null 2>&1
    then echo "TEST SUCCESSFUL"
    else echo "TEST PRODUCED UNEXPECTED RESULTS. See test_current.txt"
fi