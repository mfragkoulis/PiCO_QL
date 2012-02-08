test: main.o Account.o SuperAccount.o SpecialAccount.o
	g++ -W -g main.o Account.o SuperAccount.o SpecialAccount.o -o test

main.o: main.cpp Account.h SuperAccount.h SpecialAccount.h
	g++ -W -g -c main.cpp

Account.o: Account.cpp Account.h
	g++ -W -g -c Account.cpp

SuperAccount.o: SuperAccount.cpp SuperAccount.h
	g++ -W -g -c SuperAccount.cpp

SpecialAccount.o: SpecialAccount.cpp SpecialAccount.h
	g++ -W -g -c SpecialAccount.cpp