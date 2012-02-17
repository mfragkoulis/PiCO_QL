ifdef RELEASE
CXXFLAGS=-D_NDEBUG -O2
CFLAGS=-D_NDEBUG -O2
else
CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -DTEST
CFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -DTEST
ifdef TYPESAFE
CXXFLAGS+=-DTYPESAFE
CFLAGS+=-DTYPESAFE
endif
ifdef DEBUG
CXXFLAGS+=-DDEBUG
CFLAGS+=-DDEBUG
endif
endif

OBJ=main.o Account.o SuperAccount.o SpecialAccount.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

bank_app: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h stl_search.h

Account.o: Account.cpp Account.h

SuperAccount.o: SuperAccount.cpp SuperAccount.h

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h

user_functions.o: user_functions.c user_functions.h stl_test.h

workers.o: workers.cpp workers.h stl_search.h

stl_test.o: stl_test.c stl_test.h


seed:
	cp test_current.txt test_success.txt


.PHONY= clean
clean: 
	rm *.o