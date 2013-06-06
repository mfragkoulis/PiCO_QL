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

# Distinguish Fedora, Ubuntu distros.
DISTRO = `echo $(uname -r)`
FEDORA = `echo $(DISTRO) | fgrep fc`
ifneq ($(FEDORA), 0)
KERNEL_HEADER_DIR = "/usr/src/kernels/`uname -r`/include"
else
KERNEL_HEADER_DIR = "/usr/src/linux-headers/`uname -r`/include"
endif

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
	if [ -L $(KERNEL_HEADER_DIR)/sqlite3.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/sqlite3.h ; \
	fi
	ln -s `pwd`/sqlite3.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/assert.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/assert.h ; \
	fi
	ln -s `pwd`/assert.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/errno.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/errno.h ; \
	fi
	ln -s `pwd`/errno.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/fcntl.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/fcntl.h ; \
	fi
	ln -s `pwd`/fcntl.h $(KERNEL_HEADER_DIR)/; \
	if [ -L $(KERNEL_HEADER_DIR)/stdio.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/stdio.h ; \
	fi
	ln -s `pwd`/stdio.h $(KERNEL_HEADER_DIR)/; \
	if [ -L $(KERNEL_HEADER_DIR)/stdlib.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/stdlib.h ; \
	fi
	ln -s `pwd`/stdlib.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/string.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/string.h ; \
	fi
	ln -s `pwd`/string.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/sys ] ; then \
	rm $(KERNEL_HEADER_DIR)/sys ; \
	fi
	ln -s `pwd`/sys $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/time.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/time.h ; \
	fi
	ln -s `pwd`/time.h $(KERNEL_HEADER_DIR)/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/sys/time.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/sys/time.h ; \
	fi
	ln -s `pwd`/time.h $(KERNEL_HEADER_DIR)/sys/ ; \
	if [ -L $(KERNEL_HEADER_DIR)/unistd.h ] ; then \
	rm $(KERNEL_HEADER_DIR)/unistd.h ; \
	fi
	ln -s `pwd`/unistd.h $(KERNEL_HEADER_DIR)/ ; \

