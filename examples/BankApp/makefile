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
CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -DTEST
CFLAGS=-W -g -DTEST
ifdef TYPESAFE
CXXFLAGS+=-DTYPESAFE
CFLAGS+=-DTYPESAFE
endif
endif
ifdef DEBUG
CXXFLAGS+=-DDEBUG
CFLAGS+=-DDEBUG
endif

OBJ=main.o Account.o SuperAccount.o SpecialAccount.o pico_ql_search.o pico_ql_vt.o pico_ql_interface.o pico_ql_search_helper.o pico_ql_test.o

bank_app: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h pico_ql_search.h

Account.o: Account.cpp Account.h

SuperAccount.o: SuperAccount.cpp SuperAccount.h

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h

pico_ql_search.o: pico_ql_search.cpp pico_ql_search.h pico_ql_interface.h pico_ql_search_helper.h

pico_ql_vt.o: pico_ql_vt.c pico_ql_vt.h pico_ql_search.h

pico_ql_interface.o: pico_ql_interface.c pico_ql_interface.h pico_ql_vt.h pico_ql_test.h

pico_ql_search_helper.o: pico_ql_search_helper.cpp pico_ql_interface.h pico_ql_search.h

pico_ql_test.o: pico_ql_test.c pico_ql_test.h

seed:
	cp pico_ql_test_current.txt pico_ql_test_success.txt


.PHONY= clean
clean: 
	rm *.o