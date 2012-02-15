CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS

OBJ=main.o Fleet.o Truck.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o mtrand.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

RCXX=$(CXX) $(CXXFLAGS) -c $<

RCC=$(CC) $(CXXFLAGS) -c $<


test: $(OBJ)
	$(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@ -lemon

main.o: main.cpp Fleet.h Truck.h Customer.h Position.h mtrand.h stl_search.h
	$(CXX) -c $<

Fleet.o: Fleet.cpp Fleet.h Truck.h mtrand.h
	$(CXX) -c $<

Truck.o: Truck.cpp Truck.h Customer.h mtrand.h
	$(CXX) -c $<

Customer.o: Customer.cpp Customer.h Position.h
	$(CXX) -c $<

LinehaulCustomer.o: LinehaulCustomer.cpp LinehaulCustomer.h Customer.h mtrand.h
	$(CXX) -c $<

BackhaulCustomer.o: BackhaulCustomer.cpp BackhaulCustomer.h Customer.h mtrand.h
	$(CXX) -c $<

Position.o: Position.cpp Position.h
	$(CXX) -c $<

mtrand.o: mtrand.cpp mtrand.h
	$(CXX) -c $<

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	$(CXX) -c $<

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
	$(RCC)

user_functions.o: user_functions.c user_functions.h stl_test.h
	$(RCC)

workers.o: workers.cpp workers.h stl_search.h
	$(RCXX)

stl_test.o: stl_test.c stl_test.h
	$(RCC)