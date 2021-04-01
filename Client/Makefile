all : Client

Client : Main.o Client.o
		g++ Main.o Client.o -o Client

Main.o : Main.cpp Client.hpp
	g++ -c Main.cpp

Client.o : Client.cpp Client.hpp
	g++ -c Client.cpp

clean:
	rm *.o Client
