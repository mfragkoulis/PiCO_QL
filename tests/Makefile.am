AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te

TESTSUITE = $(srcdir)/testsuite

EXTRA_DIST = \
	testsuite.at \
	$(TESTSUITE) \
	package.m4

DISTCLEANFILES = atconfig

TESTSCRIPTS =

# PLEASE adhere to alphabetical ordering of TESTSCRIPTS

TESTSCRIPTS += \
	$(srcdir)/BankApp/testsuite-bankapp.at \
	$(srcdir)/CApp/testsuite-capp.at \
	$(srcdir)/Chess/testsuite-chess.at \
	$(srcdir)/Parentchild/testsuite-parentchild.at \
	$(srcdir)/Polymorphism/testsuite-polymorphism.at \
	$(srcdir)/VRP/testsuite-vrp.at

# so far, example codes for these tests are not compiled
NONECOMPILED_TESTSSCRIPTS = \
	$(srcdir)/CScout/testsuite-cscout.at \
	$(srcdir)/HD5Table/testsuite-hd5table.at \
	$(srcdir)/QLandKarte/testsuite-qlandkarte.at \
	$(srcdir)/SparseMatrixC/testsuite-sparsematrixc.at \
	$(srcdir)/Stellarium/testsuite-stellarium.at

max_jobs = 8
 
check-local: atconfig $(TESTSUITE)
	nrjobs=; \
	for flag in $$MAKEFLAGS; do \
		case $$flag in \
		--* | =*=) ;; \
		*j*) nrjobs="-j$(max_jobs)" ;; \
		esac; \
	done; \
	$(SHELL) '$(TESTSUITE)' $$nrjobs AUTOTEST_PATH='$(abs_top_builddir)/src' $(TESTSUITEFLAGS)

installcheck-local: atconfig $(TESTSUITE)
	nrjobs=; \
	for flag in $$MAKEFLAGS; do \
		case $$flag in \
		--* | =*=) ;; \
		*j*) nrjobs="-j$(max_jobs)" ;; \
		esac; \
	done; \
	$(SHELL) '$(TESTSUITE)' $$nrjobs AUTOTEST_PATH='$(bindir)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

AUTOTEST = $(AUTOM4TE) --language=autotest
# testsuite must not depend on atlocal or atconfig as otherwise distcheck does
# not work (wants to regenerate testsuite from testsuite.at in srcdir, which is
# read-only)
$(TESTSUITE): $(srcdir)/testsuite.at package.m4 $(TESTSCRIPTS)
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

# The `:;' works around a Bash 3.2 bug when the output is not writeable.
package.m4: $(top_srcdir)/configure.ac
	:;{ \
		echo '# Signature of the current package.' && \
		echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])' && \
		echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])' && \
		echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])' && \
		echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])' && \
		echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	} >'package.m4'
