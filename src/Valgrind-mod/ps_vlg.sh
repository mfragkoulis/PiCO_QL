#!/bin/bash

# Fill in tty appropriately.
ps -t /dev/pts/1 >> ps_vlg.out
printf "\n\n>>\n\n" >> ps_vlg.out
