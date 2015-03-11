# Makefile
# Se incluye la referencia a las librerias necesarias.

all: Server.o Client.o
	g++ -pthread -o scs_cli Client.o
	g++ -pthread -o scs_svr Server.o

Server:
	g++ -c Server.cpp

Client:
	g++ -std=gnu++11 -c Client.cpp

clean:
	rm *.o scs_cli scs_svr
