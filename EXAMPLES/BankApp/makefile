CXXFLAGS=-g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS

test: main.o Account.o SuperAccount.o SpecialAccount.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o
	g++ -lswill -lsqlite3 -W -g $(CXXFLAGS) main.o Account.o SuperAccount.o SpecialAccount.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o -o test

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h stl_search.h
	g++ -Wno-write-strings -g -c $(CXXFLAGS) main.cpp

Account.o: Account.cpp Account.h
	g++ -W -g -c $(CXXFLAGS) Account.cpp

SuperAccount.o: SuperAccount.cpp SuperAccount.h
	g++ -W -g -c $(CXXFLAGS) SuperAccount.cpp

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h
	g++ -W -g -c $(CXXFLAGS) SpecialAccount.cpp

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	g++ -W -g -c $(CXXFLAGS) stl_search.cpp

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
	gcc -g -c $(CXXFLAGS) stl_to_sql.c

user_functions.o: user_functions.c user_functions.h stl_test.h
	gcc -W -g -c $(CXXFLAGS) user_functions.c

workers.o: workers.cpp workers.h stl_search.h
	g++ -W -g -c $(CXXFLAGS) workers.cpp

stl_test.o: stl_test.c stl_test.h
	gcc -W -g -c $(CXXFLAGS) stl_test.c
