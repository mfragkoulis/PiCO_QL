chess: main.o ChessPiece.o Position.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o
	g++ -lswill -lsqlite3 -W -g main.o ChessPiece.o Position.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o -o chess

main.o: main.cpp ChessPiece.h Position.h stl_search.h
	g++ -W -G -c main.cpp

ChessPiece.o: ChessPiece.cpp ChessPiece.h Position.h
	g++ -W -G -c ChessPiece.cpp

Position.o: Position.cpp Position.h
	g++ -W -G -c Position.cpp

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
	g++ -W -g -c stl_search.cpp

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
	gcc -g -c stl_to_sql.c

user_functions.o: user_functions.c user_functions.h stl_test.h
	gcc -W -g -c user_functions.c

workers.o: workers.cpp workers.h stl_search.h
	g++ -W -g -c workers.cpp

stl_test.o: stl_test.c stl_test.h
	gcc -W -g -c stl_test.c
