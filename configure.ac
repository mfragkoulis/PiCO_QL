AC_INIT([picoql], [1.0], [bug-automake@gnu.org])

AM_INIT_AUTOMAKE([-Wall foreign])

AM_PROG_AR

LT_INIT

AC_PROG_CC
AC_PROG_CXX

AC_ARG_ENABLE([run-tests],
	      AS_HELP_STRING([--enable-run-tests], [Enable test configuration]))

AM_CONDITIONAL([ENABLE_TESTS], [test "$enable_run_tests" = "yes"])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

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
