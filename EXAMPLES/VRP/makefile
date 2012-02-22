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

OBJ=main.o Fleet.o Truck.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o mtrand.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

schedule: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

main.o: main.cpp Fleet.h Truck.h Customer.h Position.h mtrand.h stl_search.h
	$(CXX) -W -g -c $<

Fleet.o: Fleet.cpp Fleet.h Truck.h mtrand.h
	$(CXX) -W -g -c $<

Truck.o: Truck.cpp Truck.h Customer.h mtrand.h
	$(CXX) -W -g -c $<

Customer.o: Customer.cpp Customer.h Position.h
	$(CXX) -W -g -c $<

LinehaulCustomer.o: LinehaulCustomer.cpp LinehaulCustomer.h Customer.h mtrand.h
	$(CXX) -W -g -c $<

BackhaulCustomer.o: BackhaulCustomer.cpp BackhaulCustomer.h Customer.h mtrand.h
	$(CXX) -W -g -c $<

Position.o: Position.cpp Position.h
	$(CXX) -W -g -c $<

mtrand.o: mtrand.cpp mtrand.h
	$(CC) -W -g -c $<

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	$(CXX) -W -g -c $<

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h

user_functions.o: user_functions.c user_functions.h stl_test.h

workers.o: workers.cpp workers.h stl_search.h

stl_test.o: stl_test.c stl_test.h

seed:
	cp test_current.txt test_success.txt

.PHONY: clean

clean:
	rm *.o

