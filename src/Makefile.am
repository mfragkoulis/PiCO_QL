lib_LTLIBRARIES = libpicoql.la

if ENABLE_TESTS
AM_CXXFLAGS = -O3 -Wall -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DPICO_QL_TEST
else
AM_CXXFLAGS = -D_NDEBUG -O2
endif

if ENABLE_DEBUG
AM_CXXFLAGS += -DPICO_QL_DEBUG
endif

if ENABLE_SWILL
AM_CXXFLAGS += -DPICO_QL_SWILL
endif

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
AM_CXXFLAGS += -DPICO_QL_SWILL
endif
if ENABLE_SWILL
libpicoql_la_SOURCES = sqlite3.c pico_ql_error_page.cpp pico_ql_db.cpp pico_ql_step.cpp pico_ql_logo.cpp pico_ql_search_helper.cpp pico_ql_vt.cpp pico_ql_swill.cpp
else
libpicoql_la_SOURCES = sqlite3.c pico_ql_db.cpp pico_ql_step.cpp pico_ql_search_helper.cpp pico_ql_vt.cpp
endif

if ENABLE_SWILL
libpicoql_la_LIBADD = -lswill -ldl
else
libpicoql_la_LIBADD = -ldl
endif
