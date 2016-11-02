ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src examples
dist_doc_DATA = doc/README.mediawiki

test: test-clean
	cd $(bindir) && bash run_tests.sh

test-clean:
	-cd $(bindir) && rm *_test_current.txt
