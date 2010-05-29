test: stl_to_sql.o sqlite3.o search.o
	gcc -lstdc++ -W -g stl_to_sql.o sqlite3.o search.o -o test

stl_to_sql.o: stl_to_sql.c stl_to_sql.h sqlite3.h bridge.h
	gcc -W -g -c stl_to_sql.c

sqlite3.o: sqlite3.c sqlite3.h
	gcc -W -g -c sqlite3.c

search.o: search.cpp search.h sqlite3.h bridge.h Account.h
	gcc -W -g -c search.cpp

