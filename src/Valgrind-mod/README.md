### Installation and deployment

* Clone PiCO QL repo in the root directory of Valgrind.
* Patch the target Valgrind release to use the PiCO QL Valgrind module
  * cd PiCO\_QL/src/Valgrind\_mod/Valgrind-<release>-patches
  * chmod +x valgrind\_tap.sh
  * ./valgrind\_tap.sh
* Compile and install Valgrind with PiCO QL
  * ./configure $(pwd) && make && make install (from the root directory of Valgrind)
sets up a local installation.
* Setup client-server communication.
  * Input the absolute tty file name of the terminal on which the Valgrind process is running
in PiCO\_QL/src/Valgrind\_mod/ps\_vlg.sh 
  * chmod +x ps\_vlg.sh
