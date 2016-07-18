#!/bin/bash

./bankapp >/dev/null
bash pico_ql_diff_test.sh bankapp

./chess >/dev/null
bash pico_ql_diff_test.sh chess

./vrp >/dev/null
bash pico_ql_diff_test.sh vrp

./capp >/dev/null
bash pico_ql_diff_test.sh capp

./cppapp >/dev/null
mv capp_test_current.txt cppapp_test_current.txt
bash pico_ql_diff_test.sh cppapp

./polymorphism >/dev/null
bash pico_ql_diff_test.sh polymorphism

