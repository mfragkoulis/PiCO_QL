 #   Copyright [2012] [Marios Fragkoulis]
 #
 #   Licensed under the Apache License, Version 2.0
 #   (the "License");you may not use this file except in
 #   compliance with the License.
 #   You may obtain a copy of the License at
 #
 #       http://www.apache.org/licenses/LICENSE-2.0
 #
 #   Unless required by applicable law or agreed to in
 #   writing, software distributed under the License is
 #   distributed on an "AS IS" BASIS.
 #   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 #   express or implied.
 #   See the License for the specific language governing
 #  permissions and limitations under the License.
 #

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
endif

ifdef DEBUG
CXXFLAGS+=-DDEBUG
CFLAGS+=-DDEBUG
endif

OBJ=main.o ChessPiece.o Position.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

chess: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp ChessPiece.h Position.h stl_search.h

ChessPiece.o: ChessPiece.cpp ChessPiece.h Position.h

Position.o: Position.cpp Position.h

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
