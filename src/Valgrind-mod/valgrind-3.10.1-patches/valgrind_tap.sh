#!/bin/sh
#
#   Add PiCO QL support to Valgrind 3.10.1
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


VG_DIR="../../../.."
PATCH_DIR="$(pwd)"

FILES="configure
cachegrind/cg_main.c
cachegrind/Makefile.in
callgrind/bbcc.c
callgrind/main.c
callgrind/Makefile.in
memcheck/mc_errors.c
memcheck/mc_main.c
memcheck/Makefile.in"

for f in $FILES;
do
  patch -d $VG_DIR $f $PATCH_DIR/$(dirname $f)_$(basename $f).patch
done

printf "\nNow you can configure, compile, and install Valgrind\n \
in the usual manner by executing \n\n \
./configure && make && make install\n\n \
at Valgrind's root directory.\n\n"
