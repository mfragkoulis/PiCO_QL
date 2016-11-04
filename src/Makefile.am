lib_LTLIBRARIES = libpicoql.la

if ENABLE_TESTS
AM_CFLAGS = -O3 -Wall -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CFLAGS = -D_NDEBUG -O2
endif

if ENABLE_DEBUG
AM_CFLAGS += -DPICO_QL_DEBUG
endif

if ENABLE_SWILL
AM_CFLAGS += -DPICO_QL_SWILL
endif

if ENABLE_SWILL
libpicoql_la_SOURCES = sqlite3.c pico_ql_error_page.c pico_ql_db.c pico_ql_step.c pico_ql_logo.c pico_ql_search_helper.c pico_ql_vt.c pico_ql_swill.c
else
libpicoql_la_SOURCES = sqlite3.c pico_ql_db.c pico_ql_step.c pico_ql_search_helper.c pico_ql_vt.c
endif

if ENABLE_SWILL
libpicoql_la_LIBADD = -lswill -ldl
else
libpicoql_la_LIBADD = -ldl
endif
