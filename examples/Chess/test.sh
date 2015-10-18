#!/bin/sh
make prep
make clean
ruby pico_ql_generator.rb pico_ql_dsl.sql
# Available test configurations (2):
#make PICO_QL_DEBUG=1 PICO_QL_JOIN_THREADS=1 PICO_QL_HANDLE_TEXT_ARRAY=0 PICO_QL_POLYMORPHISM=1 PICO_QL_SINGLE_THREADED=1
make PICO_QL_JOIN_THREADS=1 PICO_QL_HANDLE_TEXT_ARRAY=1 && \
./chess
diff pico_ql_test_current.txt pico_ql_test_success.txt
# Available release (SWILL interface) configurations (2):
#make RELEASE=1 PICO_QL_SINGLE_THREADED=1
#make RELEASE=1 PICO_QL_JOIN_THREADS=1
