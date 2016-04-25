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
in PiCO\_QL/src/Valgrind\_mod/ps\_vlg.sh (command 'tty' on Valgrind process's terminal session)
  * chmod +x ps\_vlg.sh

* Here is what I did one time I came back to use Valgrind:
  * Identify tty terminal session where the Valgrind process will run (e.g. /dev/pts/1)
  * Input that to ../../../bin/ps_vlg.sh
  * Remove picoQL_* pipes from /home/mfg/test/
  * Run Valgrind process, e.g.: ../../../bin/valgrind --tool=memcheck zip -r mozilla.zip /home/mfg/sysSW/mozilla-central
  * Run PiCO QL's GUI e.g.: ../../../bin/picoQL-gui 8081
