#!/bin/sh
make prep
make clean
ruby pico_ql_generator.rb pico_ql_dsl.sql
# Available test configurations (2):
#make PICO_QL_DEBUG=1 PICO_QL_JOIN_THREADS=1 PICO_QL_HANDLE_TEXT_ARRAY=0
make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1
# Available release (SWILL interface) configurations (2):
#make RELEASE=1 PICO_QL_SINGLE_THREADED=1 PICO_QL_POLL=1
#make RELEASE=1 PICO_QL_JOIN_THREADS=1
./schedule solomon.txt 2
