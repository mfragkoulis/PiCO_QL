CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS

OBJ=main.o Account.o SuperAccount.o SpecialAccount.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

RCXX=$(CXX) $(CXXFLAGS) -c $<

RCC=$(CC) $(CXXFLAGS) -c $<

test: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o test

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h stl_search.h
	$(RCXX)

Account.o: Account.cpp Account.h
	$(RCXX)

SuperAccount.o: SuperAccount.cpp SuperAccount.h
	$(RCXX)

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h
	$(RCXX)

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	$(RCXX)

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
	$(RCC)

user_functions.o: user_functions.c user_functions.h stl_test.h
	$(RCC)

workers.o: workers.cpp workers.h stl_search.h
	$(RCXX)

stl_test.o: stl_test.c stl_test.h
	$(RCC)
