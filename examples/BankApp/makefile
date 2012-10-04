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

ifdef RELEASE
CXXFLAGS=-D_NDEBUG -O2
CFLAGS=-D_NDEBUG -O2
else
CXXFLAGS=-O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
CFLAGS=-O3 -Wall -Werror -g -DPICO_QL_TEST
endif
ifdef PICO_QL_DEBUG
CXXFLAGS+=-DPICO_QL_DEBUG
CFLAGS+=-DPICO_QL_DEBUG
endif
ifdef PICO_QL_SINGLE_THREADED
CXXFLAGS+=-DPICO_QL_SINGLE_THREADED
CFLAGS+=-DPICO_QL_SINGLE_THREADED
endif
ifdef PICO_QL_JOIN_THREADS
CXXFLAGS+=-DPICO_QL_JOIN_THREADS
CFLAGS+=-DPICO_QL_JOIN_THREADS
endif
ifdef PICO_QL_HANDLE_POLYMORPHISM
CXXFLAGS+=-DPICO_QL_HANDLE_POLYMORPHISM
CFLAGS+=-DPICO_QL_HANDLE_POLYMORPHISM
endif

all: prep bank_app

OBJ=main.o Account.o SuperAccount.o SpecialAccount.o pico_ql_search.o pico_ql_vt.o pico_ql_logo.o pico_ql_error_page.o pico_ql_interface.o pico_ql_search_helper.o pico_ql_test.o

bank_app: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h pico_ql_search.h

Account.o: Account.cpp Account.h

SuperAccount.o: SuperAccount.cpp SuperAccount.h

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h

pico_ql_search.o: pico_ql_search.cpp pico_ql_search.h pico_ql_internal.h pico_ql_interface.h pico_ql_search_helper.h

pico_ql_vt.o: pico_ql_vt.c pico_ql_vt.h pico_ql_internal.h

pico_ql_logo.o: pico_ql_logo.c pico_ql_swill_access_func.h

pico_ql_error_page.o: pico_ql_error_page.c pico_ql_swill_access_func.h

pico_ql_interface.o: pico_ql_interface.c pico_ql_interface.h pico_ql_vt.h pico_ql_test.h pico_ql_swill_access_func.h

pico_ql_search_helper.o: pico_ql_search_helper.cpp pico_ql_interface.h pico_ql_internal.h

pico_ql_test.o: pico_ql_test.c pico_ql_test.h pico_ql_internal.h

cmp:
	diff pico_ql_test_current.txt pico_ql_test_success.txt

seed:
	cp pico_ql_test_current.txt pico_ql_test_success.txt

prep:
	mv pico_ql_test.c test.c
	cp -r ../../src/ .
	mv test.c pico_ql_test.c


.PHONY= clean
clean: 
	rm *.o
	rm Bank