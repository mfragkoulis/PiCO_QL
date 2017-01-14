lib_LTLIBRARIES = libpicoql.la

if ENABLE_TESTS
AM_CFLAGS = -O3 -Wall -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
AM_CXXFLAGS = -O3 -Wall -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CFLAGS = -D_NDEBUG -O2
AM_CXXFLAGS = -D_NDEBUG -O2
endif

if OMIT_SQLITE
AM_CFLAGS += -DHAS_SQLITE
endif

if ENABLE_DEBUG
AM_CFLAGS += -DPICO_QL_DEBUG
AM_CXXFLAGS += -DPICO_QL_DEBUG
endif

if ENABLE_SWILL
AM_CFLAGS += -DPICO_QL_SWILL
AM_CXXFLAGS += -DPICO_QL_SWILL
endif

libpicoql_la_SOURCES = sqlite3.c pico_ql_db.cpp pico_ql_exec.cpp pico_ql_exec_c.c pico_ql_step.cpp pico_ql_step_c.c pico_ql_register_table.c pico_ql_search_helper.c pico_ql_vt.c

if ENABLE_SWILL
libpicoql_la_SOURCES += pico_ql_error_page.c pico_ql_logo.c pico_ql_swill.c
endif

if ENABLE_SWILL
libpicoql_la_LIBADD = -lswill -ldl
else
libpicoql_la_LIBADD = -ldl
endif
