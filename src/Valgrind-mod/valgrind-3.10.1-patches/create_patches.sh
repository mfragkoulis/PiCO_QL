#!/bin/bash

VG_TAPPED_DIR="../../../.."
VG_DIR=$1

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
  diff $VG_TAPPED_DIR/$f $VG_DIR/$f > $(dirname $f)_$(basename $f).patch
  echo "Created patch for file $f"
done
