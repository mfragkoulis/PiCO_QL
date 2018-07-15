bin_PROGRAMS = cppapp capp

AM_CFLAGS = -I$(abs_top_srcdir)/src
AM_CXXFLAGS = -I$(abs_top_srcdir)/src

if ENABLE_TESTS
AM_CFLAGS += -O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CFLAGS += -D_NDEBUG -O2
endif

if ENABLE_DEBUG
AM_CFLAGS += -DPICO_QL_DEBUG
endif

if ENABLE_TESTS
AM_CXXFLAGS += -O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CXXFLAGS += -D_NDEBUG -O2
endif

if ENABLE_DEBUG
AM_CXXFLAGS += -DPICO_QL_DEBUG
endif

# Counter common .o file error
capp_CFLAGS = $(AM_CFLAGS)
cppapp_CFLAGS = $(AM_CXXFLAGS)

capp_SOURCES = pico_ql_search.c pico_ql_internal.c pico_ql_test.c main.c
cppapp_SOURCES = pico_ql_search_cpp.cpp pico_ql_internal_cpp.cpp pico_ql_test_cpp.cpp main_cpp.cpp

pico_ql_search.c:
	cp $(abs_top_srcdir)/src/pico_ql_generator.rb .
	cp -R $(abs_top_srcdir)/src/pico_ql_erb_templates .
	ruby pico_ql_generator.rb $(srcdir)/pico_ql_dsl.sql C

pico_ql_internal.c: pico_ql_search.c

pico_ql_search_cpp.cpp:
	cp $(abs_top_srcdir)/src/pico_ql_generator.rb .
	cp -R $(abs_top_srcdir)/src/pico_ql_erb_templates .
	ruby pico_ql_generator.rb $(srcdir)/pico_ql_dsl.sql
	mv pico_ql_search.cpp pico_ql_search_cpp.cpp
	mv pico_ql_internal.cpp pico_ql_internal_cpp.cpp

pico_ql_internal_cpp.cpp: pico_ql_search_cpp.cpp

capp_LDADD = $(abs_top_builddir)/src/libpicoql.la -lpthread
cppapp_LDADD = $(abs_top_builddir)/src/libpicoql.la -lpthread

clean-local:
	-rm pico_ql_search_cpp.cpp pico_ql_search.c
