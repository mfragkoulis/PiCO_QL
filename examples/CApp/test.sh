#!/bin/sh
make prep
make clean
ruby pico_ql_generator.rb pico_ql_dsl.sql
make PICO_QL_JOIN_THREADS=1
gdb ./capp