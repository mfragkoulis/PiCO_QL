chess: main.o ChessPiece.o Position.o
	g++ -W -G main.o ChessPiece.o Position.o -o chess

main.o: main.cpp ChessPiece.h Position.h
	g++ -W -G -c main.cpp

ChessPiece.o: ChessPiece.cpp ChessPiece.h Position.h
	g++ -W -G -c ChessPiece.cpp

Position.o: Position.cpp Position.h
	g++ -W -G -c Position.cpp