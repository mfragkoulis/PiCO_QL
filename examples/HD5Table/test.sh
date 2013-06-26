#!/bin/sh
#make prep
#make clean
#rm pico_ql_internal.c pico_ql_search.c > /dev/null
# Remove "C" argument from ruby command for C++ build
#ruby pico_ql_generator.rb pico_ql_dsl.sql C
# Add "G_CXX=1" to compile options for C++ build
# Available test configurations (2):
make PICO_QL_JOIN_THREADS=1
#make PICO_QL_DEBUG=1 PICO_QL_SINGLE_THREADED=1
# Available release (SWILL interface) configurations (2):
#make RELEASE=1 PICO_QL_SINGLE_THREADED=1 G_CXX=1
#make RELEASE=1 PICO_QL_JOIN_THREADS=1
gdb ./h5t
