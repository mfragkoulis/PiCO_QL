#!/bin/sh
make prep
make clean
ruby pico_ql_generator.rb pico_ql_dsl.sql no_mem_mgt
make PICO_QL_JOIN_THREADS=1
#make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1
gdb ./schedule
