lib_LTLIBRARIES = libpicoql.la

if ENABLE_TESTS
AM_CFLAGS = -O3 -Wall -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CFLAGS = -D_NDEBUG -O2
endif

libpicoql_la_SOURCES = sqlite3.c pico_ql_error_page.c pico_ql_interface.c pico_ql_logo.c pico_ql_search_helper.c pico_ql_vt.c

libpicoql_la_LIBADD = -lswill -ldl
