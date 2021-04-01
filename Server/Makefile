all : Server

Server : Main.o Server.o
		g++ Main.o Server.o -o Server

Main.o : Main.cpp Server.hpp
	g++ -c Main.cpp

Server.o : Server.cpp Server.hpp
	g++ -c Server.cpp

clean:
	rm *.o Server
