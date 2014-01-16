#!/bin/bash

# Fill in tty appropriately.
ps -t <tty> >> ps_vlg.out
printf "\n\n>>\n\n" >> ps_vlg.out
