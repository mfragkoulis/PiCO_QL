 #
 #   Declare the source files dependencies.
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
 #  permissions and limitations under the License.
 #

CC=gcc
# exported from Cachegrind's makefile
PICO_QL_CFLAGS = $(PICO_QL_CFLAGS_$(ARCH))
PICO_QL_CFLAGS+=-Werror -nodefaultlibs -DPICO_QL_SINGLE_THREADED -DPICO_QL_VALGRIND
#-DPICO_QL_DEBUG
# Removed -Wwrite-strings

#Complete list of flags: 
# -DHAVE_CONFIG_H -I../../cachegrind -I../.. -I. -I../../include -I../../VEX/pub -DVGA_amd64=1 -DVGO_darwin=1 -DVGP_amd64_darwin=1 -DVGPV_amd64_darwin_vanilla=1 -arch x86_64 -O2 -g -Wall -Wmissing-prototypes -Wshadow -Wpointer-arith -Wstrict-prototypes -Wmissing-declarations -Wno-format-zero-length -fno-strict-aliasing -fno-builtin -mmacosx-version-min=10.5 -fno-stack-protector -Wno-long-long -fno-stack-protector

ifdef AMD64
ARCH=amd64
else
ARCH=x86
endif

ifdef PICO_QL_DEBUG
CFLAGS+=-DPICO_QL_DEBUG
endif
ifdef PICO_QL_SINGLE_THREADED
CFLAGS+=-DPICO_QL_SINGLE_THREADED
endif
ifdef PICO_QL_JOIN_THREADS
CFLAGS+=-DPICO_QL_JOIN_THREADS
endif
ifdef PICO_QL_HANDLE_TEXT_ARRAY
CFLAGS+=-DPICO_QL_HANDLE_TEXT_ARRAY
endif
ifdef PICO_QL_HANDLE_POLYMORPHISM
CFLAGS+=-DPICO_QL_HANDLE_POLYMORPHISM
endif

# For dynamic loading of SQL functions extension library
ifeq ($(shell uname -s), Darwin)
#RPATH=-rpath -Xlinker -I$(PWD)   # 10.8
RPATH=-L$(PWD)  # [TODO-configure]
SUFFIX=dylib
else
RPATH=-Wl,-rpath=$(PWD)
SUFFIX=so
endif

INCLUDES= -I. -I.. -I../../include -I../../VEX/pub
PICO_QL_CFLAGS+=$(INCLUDES) 

CFLAGS=-Wall -Werror -g 
#-DPICO_QL_DEBUG
UI_CFLAGS=$(CFLAGS)

all: gen libpicoQL_$(ARCH)

OBJ=pico_ql_search_$(ARCH).o pico_ql_internal_$(ARCH).o pico_ql_vt_$(ARCH).o pico_ql_interface_$(ARCH).o pico_ql_search_helper_$(ARCH).o pico_ql_test_$(ARCH).o sqlite3_$(ARCH).o errno_$(ARCH).o stdio_$(ARCH).o string_$(ARCH).o unistd_$(ARCH).o time_$(ARCH).o

APP_OBJ=cachegrind_$(ARCH)_darwin-cg_main.o


libpicoQL_$(ARCH): $(OBJ) $(APP_OBJ) picoQL-gui
	ar rcs libpicoQL_$(ARCH).a $(OBJ) $(APP_OBJ)

pico_ql_search_$(ARCH).o: pico_ql_search.c pico_ql_search.h pico_ql_internal.h
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@ 

pico_ql_internal_$(ARCH).o: pico_ql_internal.c pico_ql_internal.h pico_ql_interface.h pico_ql_search_helper.h
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@ 

pico_ql_vt_$(ARCH).o: pico_ql_vt.c pico_ql_vt.h pico_ql_internal.h
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@


pico_ql_interface_$(ARCH).o: pico_ql_interface.c pico_ql_interface.h pico_ql_vt.h pico_ql_test.h 
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

pico_ql_search_helper_$(ARCH).o: pico_ql_search_helper.c pico_ql_search_helper.h pico_ql_internal.h
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

pico_ql_test_$(ARCH).o: pico_ql_test.c pico_ql_test.h pico_ql_internal.h
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

pico_ql_search.c: pico_ql_generator.rb valgrind_dsl.sql
	ruby pico_ql_generator.rb valgrind_dsl.sql valgrind

sqlite3_$(ARCH).o: sqlite3.c sqlite3.h
	$(CC) $(PICO_QL_CFLAGS) -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_OMIT_BUILTIN_TEST -DNDEBUG -DSQLITE_OMIT_WAL -DSQLITE_WITHOUT_ZONEMALLOC -c $< -o $@

errno_$(ARCH).o: errno.c
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

stdio_$(ARCH).o: stdio.c
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

string_$(ARCH).o: string.c
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

unistd_$(ARCH).o: unistd.c
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

time_$(ARCH).o: time.c
	$(CC) $(PICO_QL_CFLAGS) -c $< -o $@

picoQL-gui: pico_ql_swill_ui.o pico_ql_logo.o pico_ql_error_page.o
	$(CC) $(UI_CFLAGS) -lswill $^ -o $@

pico_ql_swill_ui.o: pico_ql_swill_ui.c pico_ql_swill_access_func.h

pico_ql_logo.o: pico_ql_logo.c pico_ql_swill_access_func.h

pico_ql_error_page.o: pico_ql_error_page.c pico_ql_swill_access_func.h

cmp:
	diff pico_ql_test_current.txt pico_ql_test_success.txt

seed:
	cp pico_ql_test_current.txt pico_ql_test_success.txt

gen: prep pico_ql_search.c

prep:
#	mv pico_ql_test.c test.c
	cp -r ../pico_ql_erb_templates .
	cd ../ && cp pico_ql_generator.rb pico_ql_interface.h pico_ql_search_helper.c \
          pico_ql_internal.h pico_ql_vt.c pico_ql_test.c pico_ql_error_page.c \
          pico_ql_logo.c pico_ql_search_helper.h pico_ql_vt.h pico_ql_test.h \
          pico_ql_swill_access_func.h sqlite3.c sqlite3.h Valgrind-mod/
#	mv test.c pico_ql_test.c
#	gcc -g -fPIC -shared math_func_sqlitext.c -o math_func_sqlitext.$(SUFFIX)


.PHONY= clean
clean: 
	rm *.o
