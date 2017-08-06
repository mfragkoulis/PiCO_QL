AC_INIT([picoql], [1.0], [mfg@aueb.gr])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/pico_ql_exec.cpp])
AC_CONFIG_HEADER([pico_ql_config.h])

AM_INIT_AUTOMAKE([-Wall foreign parallel-tests color-tests])

AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX

LT_INIT

AC_ARG_ENABLE([has-sqlite],
	      AS_HELP_STRING([--omit-sqlite], [Omit compilation and linking with SQLite]))
AC_ARG_ENABLE([run-tests],
	      AS_HELP_STRING([--enable-run-tests], [Enable test configuration]))
AC_ARG_ENABLE([debug],
	      AS_HELP_STRING([--enable-debug], [Enable debug configuration]))
AC_ARG_ENABLE([swill],
	      AS_HELP_STRING([--enable-swill], [Enable swill web server]))

AM_CONDITIONAL([OMIT_SQLITE], [test "$omit_sqlite" = "yes"])
AM_CONDITIONAL([ENABLE_TESTS], [test "$enable_run_tests" = "yes"])
AM_CONDITIONAL([ENABLE_DEBUG], [test "$enable_debug" = "yes"])
AM_CONDITIONAL([ENABLE_SWILL], [test "$enable_swill" = "yes"])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 examples/Makefile
 examples/BankApp/Makefile
 examples/Chess/Makefile
 examples/VRP/Makefile
 examples/CApp/Makefile
 examples/Polymorphism/Makefile
 examples/Parentchild/Makefile
])

AC_OUTPUT
