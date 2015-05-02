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

# Run the script from within the directory it is located.

PATCHDIR="PiCO_QL/src/Valgrind-mod/valgrind-3.9.0-patches"
OPTS="-N"

cd ../../../../
patch $OPTS cachegrind/Makefile.in < $PATCHDIR/cachegrind_Makefile.patch
patch $OPTS configure < $PATCHDIR/valgrind_configure.patch
patch $OPTS cachegrind/cg_main.c < $PATCHDIR/cachegrind_cg_main.patch
patch $OPTS memcheck/Makefile.in < $PATCHDIR/memcheck_Makefile.patch
patch $OPTS memcheck/mc_main.c < $PATCHDIR/memcheck_mc_main.patch
patch $OPTS memcheck/mc_errors.c < $PATCHDIR/memcheck_mc_main.patch
patch $OPTS callgrind/Makefile.in < $PATCHDIR/callgrind_Makefile.patch
patch $OPTS callgrind/main.c < $PATCHDIR/callgrind_main.patch
patch $OPTS callgrind/bbcc.c < $PATCHDIR/callgrind_bbcc.patch
printf "\nNow you can configure, compile, and install Valgrind\n \
in the usual manner by executing \n\n \
./configure && make && make install\n\n \
at Valgrind's root directory.\n\n"
