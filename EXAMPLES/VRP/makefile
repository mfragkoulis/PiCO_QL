CXXFLAGS=-g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS

test: main.o Fleet.o Truck.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o mtrand.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o
	g++ -lswill -lsqlite3  -W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS main.o Fleet.o Truck.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o mtrand.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o -o test -lemon

main.o: main.cpp Fleet.h Truck.h Customer.h Position.h mtrand.h stl_search.h
	g++ -W -g -c main.cpp

Fleet.o: Fleet.cpp Fleet.h Truck.h mtrand.h
	g++ -Wall -g -c Fleet.cpp

Truck.o: Truck.cpp Truck.h Customer.h mtrand.h
	g++ -W -g -c Truck.cpp 

Customer.o: Customer.cpp Customer.h Position.h
	g++ -W -g -c Customer.cpp 

LinehaulCustomer.o: LinehaulCustomer.cpp LinehaulCustomer.h Customer.h mtrand.h
	g++ -Wall -g -c LinehaulCustomer.cpp

BackhaulCustomer.o: BackhaulCustomer.cpp BackhaulCustomer.h Customer.h mtrand.h
	g++ -Wall -g -c BackhaulCustomer.cpp

Position.o: Position.cpp Position.h
	g++ -Wall -g -c Position.cpp

mtrand.o: mtrand.cpp mtrand.h
	g++ -Wall -g -c mtrand.cpp

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	g++ -W -g -c stl_search.cpp

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
	gcc -g -c $(CXXFLAGS) stl_to_sql.c

user_functions.o: user_functions.c user_functions.h stl_test.h
	gcc -W -g -c $(CXXFLAGS) user_functions.c

workers.o: workers.cpp workers.h stl_search.h
	g++ -W -g -c $(CXXFLAGS) workers.cpp

stl_test.o: stl_test.c stl_test.h
	gcc -W -g -c $(CXXFLAGS) stl_test.c