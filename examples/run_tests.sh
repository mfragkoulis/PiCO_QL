#!/bin/bash

./bankapp >/dev/null
bash pico_ql_diff_test.sh bankapp

./chess >/dev/null
bash pico_ql_diff_test.sh chess

./vrp solomon.txt 2 >/dev/null
bash pico_ql_diff_test.sh vrp

./capp >/dev/null
bash pico_ql_diff_test.sh capp

./cppapp >/dev/null
bash pico_ql_diff_test.sh cppapp

./polymorphism >/dev/null
bash pico_ql_diff_test.sh polymorphism

./parentchild >/dev/null
bash pico_ql_diff_test.sh parentchild

