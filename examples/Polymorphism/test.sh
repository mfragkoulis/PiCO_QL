#!/bin/sh
make prep
make clean
ruby pico_ql_generator.rb pico_ql_dsl.sql
make PICO_QL_DEBUG=1 PICO_QL_JOIN_THREADS=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1
gdb ./poly
