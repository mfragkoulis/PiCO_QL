bin_PROGRAMS = bankapp

AM_CXXFLAGS = -I../../src

bankapp_SOURCES = pico_ql_search.cpp pico_ql_internal.cpp main.cpp Account.cpp SuperAccount.cpp SpecialAccount.cpp

pico_ql_search.cpp:
	cp ../../src/pico_ql_generator.rb .
	cp -R ../../src/pico_ql_erb_templates .
	ruby pico_ql_generator.rb pico_ql_dsl.sql

pico_ql_internal.cpp: pico_ql_search.cpp

bankapp_LDADD = ../../lib/libpicoql.la -lpthread
