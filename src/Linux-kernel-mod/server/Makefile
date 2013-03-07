 #
 #   Decsribe source files dependencies.
 #
 #   Copyright 2012 Marios Fragkoulis
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
 #   distributed on an "AS IS" BASIS.
 #   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 #   express or implied.
 #   See the License for the specific language governing
 #   permissions and limitations under the License.
 #

EXTRA_CFLAGS += -O3 -Wall -Werror -g -nostdinc -I /usr/src/linux/include -DPICO_QL_SINGLE_THREADED -DSQLITE_OMIT_FLOATING_POINT -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_OMIT_BUILTIN_TEST -DNDEBUG -DSQLITE_OMIT_WAL

obj-m += picoQL.o
picoQL-objs := sqlite3.o pico_ql_procfs.o pico_ql_search.o pico_ql_exec.o pico_ql_internal.o pico_ql_vt.o pico_ql_search_helper.o stdio.o stdlib.o time.o string.o unistd.o

all:
	cp ../../pico_ql_generator.rb ../../pico_ql_internal.h ../../pico_ql_search_helper.c ../../pico_ql_search_helper.h ../../pico_ql_vt.c ../../pico_ql_vt.h .
	cp -r ../../pico_ql_erb_templates .
	ruby pico_ql_generator.rb pico_ql_dsl.sql kernel no_mem_mgt
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

prep:
	if [ -L '/usr/src/linux' ] ; then \
	rm /usr/src/linux ; \
	fi
	ln -s /usr/src/linux-headers-`uname -r` /usr/src/linux
	if [ -L '/usr/src/linux/include/sqlite3.h' ] ; then \
	rm /usr/src/linux/include/sqlite3.h ; \
	fi
	ln -s `pwd`/sqlite3.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/assert.h' ] ; then \
	rm /usr/src/linux/include/assert.h ; \
	fi
	ln -s `pwd`/assert.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/errno.h' ] ; then \
	rm /usr/src/linux/include/errno.h ; \
	fi
	ln -s `pwd`/errno.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/fcntl.h' ] ; then \
	rm /usr/src/linux/include/fcntl.h ; \
	fi
	ln -s `pwd`/fcntl.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/stdio.h' ] ; then \
	rm /usr/src/linux/include/stdio.h ; \
	fi
	ln -s `pwd`/stdio.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/stdlib.h' ] ; then \
	rm /usr/src/linux/include/stdlib.h ; \
	fi
	ln -s `pwd`/stdlib.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/string.h' ] ; then \
	rm /usr/src/linux/include/string.h ; \
	fi
	ln -s `pwd`/string.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/sys' ] ; then \
	rm /usr/src/linux/include/sys ; \
	fi
	ln -s `pwd`/sys /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/time.h' ] ; then \
	rm /usr/src/linux/include/time.h ; \
	fi
	ln -s `pwd`/time.h /usr/src/linux/include/ ; \
	if [ -L '/usr/src/linux/include/sys/time.h' ] ; then \
	rm /usr/src/linux/include/sys/time.h ; \
	fi
	ln -s `pwd`/time.h /usr/src/linux/include/sys/ ; \
	if [ -L '/usr/src/linux/include/unistd.h' ] ; then \
	rm /usr/src/linux/include/unistd.h ; \
	fi
	ln -s `pwd`/unistd.h /usr/src/linux/include/ ; \

