test: main.o Account.o Type.o search.o stl_to_sql.o user_functions.o
	g++ -lswill -lsqlite3 -W -g main.o Account.o Type.o search.o stl_to_sql.o user_functions.o -o test

main.o: main.cpp Account.h bridge.h
	g++ -W -g -c main.cpp

user_functions.o: user_functions.c bridge.h
	gcc -W -g -c user_functions.c

stl_to_sql.o: stl_to_sql.c stl_to_sql.h bridge.h
	gcc -g -c stl_to_sql.c

search.o: search.cpp bridge.h Account.h
	g++ -W -g -c search.cpp

Account.o: Account.cpp Account.h
	g++ -W -g -c Account.cpp
