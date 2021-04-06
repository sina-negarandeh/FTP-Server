all : Client

Client : Main.o Client.o json_parser.o
		g++ Main.o Client.o json_parser.o -o Client

Main.o : Main.cpp Client.hpp
	g++ -c Main.cpp

Client.o : Client.cpp Client.hpp
	g++ -c Client.cpp

json_parser.o : json_parser.cpp json_parser.hpp
	g++ -c json_parser.cpp

clean:
	rm *.o Client
