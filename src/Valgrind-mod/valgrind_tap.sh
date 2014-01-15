#!/bin/sh
#
#   Compare the current test results with the successful 
#   prototype and print the outcome.
#
#
#   Copyright 2013 Marios Fragkoulis
#
#   Licensed under the Apache License, Version 2.0
#   (the "License");you may not use this file except in
#   compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in
#   writing, software distributed under the License is
#   distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#   express or implied.
#   See the License for the specific language governing
#   permissions and limitations under the License.

cp valgrind_configure ../../../configure
cp cachegrind_Makefile.in ../../../cachegrind/Makefile.in
cp cachegrind_cg_main.c ../../../cachegrind/cg_main.c
cp memcheck_Makefile.in ../../../memcheck/Makefile.in
cp memcheck_mc_main.c ../../../memcheck/mc_main.c
cp memcheck_mc_errors.c ../../../memcheck/mc_errors.c
cp callgrind_Makefile.in ../../../callgrind/Makefile.in
cp callgrind_main.c ../../../callgrind/main.c
cp callgrind_bbcc.c ../../../callgrind/bbcc.c
printf "\nNow you can configure, compile, and install Valgrind\n \
in the usual manner by executing \n\n \
./configure && make && make install\n\n \
at Valgrind's root directory.\n\n"
