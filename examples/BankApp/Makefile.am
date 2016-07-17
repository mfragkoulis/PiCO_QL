bin_PROGRAMS = bankapp

bankapp_SOURCES = main.cpp Account.cpp SuperAccount.cpp SpecialAccount.cpp pico_ql_search.cpp pico_ql_internal.cpp

pico_ql_search.cpp:
	ruby ../../src/pico_ql_generator.rb pico_ql_dsl.sql

pico_ql_internal.cpp: pico_ql_search.cpp

bankapp_LDADD = ../../lib/libpicoql.la -lpthread
