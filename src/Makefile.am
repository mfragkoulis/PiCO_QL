AM_CFLAGS = -I/usr/local/include

lib_LTLIBRARIES = libpicoql.la

libpicoql_la_SOURCES = sqlite3.c pico_ql_error_page.c pico_ql_interface.c pico_ql_logo.c pico_ql_search_helper.c pico_ql_test.c pico_ql_vt.c

# Cover both local install and system-wide install of libswill
libpicoql_la_LIBADD = /usr/local/lib/libswill.a
