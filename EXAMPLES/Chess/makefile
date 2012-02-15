CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS

OBJ=main.o ChessPiece.o Position.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

RCXX=$(CXX) $(CXXFLAGS) -c $<

RCC=$(CC) $(CXXFLAGS) -c $<

chess: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp ChessPiece.h Position.h stl_search.h
	$(RCXX)

ChessPiece.o: ChessPiece.cpp ChessPiece.h Position.h
	$(RCXX)

Position.o: Position.cpp Position.h
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
