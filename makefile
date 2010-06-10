test: Account.o Type.o search.o stl_to_sql.o
	g++ -bundle -fPIC -lsqlite3 -W -g Account.o Type.o search.o stl_to_sql.o -o test

stl_to_sql.o: stl_to_sql.c stl_to_sql.h bridge.h
	gcc -W -g -c stl_to_sql.c

search.o: search.cpp search.h bridge.h Account.h Type.h
	g++ -W -g -c search.cpp

Account.o: Account.cpp Account.h
	g++ -W -g -c Account.cpp

Type.o: Type.cpp Type.h
	g++ -W -g -c Type.cpp