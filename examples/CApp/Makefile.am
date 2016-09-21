bin_PROGRAMS = cppapp capp

bindata_DATA = capp_test_success.txt cppapp_test_success.txt
bindatadir = $(bindir)

AM_CFLAGS = -I../../src
AM_CXXFLAGS = -I../../src

if ENABLE_TESTS
AM_CFLAGS += -O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CFLAGS += -D_NDEBUG -O2
endif

if ENABLE_TESTS
AM_CXXFLAGS += -O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CXXFLAGS += -D_NDEBUG -O2
endif

# Counter common .o file error
capp_CFLAGS = $(AM_CFLAGS)
cppapp_CFLAGS = $(AM_CXXFLAGS)

capp_SOURCES = pico_ql_search.c pico_ql_internal.c pico_ql_test.c main.c
cppapp_SOURCES = pico_ql_search_cpp.cpp pico_ql_internal_cpp.cpp pico_ql_test_cpp.c main_cpp.c

pico_ql_search.c:
	cp ../../src/pico_ql_generator.rb .
	cp -R ../../src/pico_ql_erb_templates .
	ruby pico_ql_generator.rb pico_ql_dsl.sql C

pico_ql_internal.c: pico_ql_search.c

pico_ql_search_cpp.cpp:
	cp ../../src/pico_ql_generator.rb .
	cp -R ../../src/pico_ql_erb_templates .
	ruby pico_ql_generator.rb pico_ql_dsl.sql
	mv pico_ql_search.cpp pico_ql_search_cpp.cpp
	mv pico_ql_internal.cpp pico_ql_internal_cpp.cpp

pico_ql_internal_cpp.cpp: pico_ql_search_cpp.cpp

capp_LDADD = ../../lib/libpicoql.la -lpthread
cppapp_LDADD = ../../lib/libpicoql.la -lpthread

clean-local:
	-rm pico_ql_search_cpp.cpp pico_ql_search.c
