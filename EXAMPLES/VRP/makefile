schedule: main.o search.o stl_to_sql.o user_functions.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o Truck.o Fleet.o mtrand.o sqlite3.o
	g++ -lswill -Wall -g main.o search.o stl_to_sql.o user_functions.o Customer.o LinehaulCustomer.o BackhaulCustomer.o Position.o Truck.o Fleet.o mtrand.o sqlite3.o -o schedule -lemon

main.o: main.cpp Customer.h Position.h Truck.h mtrand.h bridge.h
	g++ -Wall -g -c main.cpp

Customer.o: Customer.cpp Customer.h Position.h
	g++ -Wall -g -c Customer.cpp

Postion.o: Position.cpp Position.h
	g++ -Wall -g -c Position.cpp

LinehaulCustomer.o: LinehaulCustomer.cpp LinehaulCustomer.h Customer.h mtrand.h
	g++ -Wall -g -c LinehaulCustomer.cpp

BackhaulCustomer.o: BackhaulCustomer.cpp BackhaulCustomer.h Customer.h mtrand.h
	g++ -Wall -g -c BackhaulCustomer.cpp

Truck.o: Truck.cpp Truck.h Customer.h mtrand.h
	g++ -Wall -g -c Truck.cpp

Fleet.o: Fleet.cpp Fleet.h Truck.h mtrand.h
	g++ -Wall -g -c Fleet.cpp

mtrand.o: mtrand.cpp mtrand.h
	g++ -Wall -g -c mtrand.cpp

user_functions.o: user_functions.c bridge.h
	gcc -W -g -c user_functions.c

stl_to_sql.o: stl_to_sql.c stl_to_sql.h bridge.h
	gcc -g -c stl_to_sql.c

search.o: search.cpp bridge.h
	g++ -W -g -c search.cpp

sqlite3.o: sqlite3.c sqlite3.h
	gcc -W -g -c sqlite3.c