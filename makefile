# Makefile
# Se incluye la referencia a las librerias necesarias.

all: Server Client
	g++ -pthread -o scs_cli Client.o
	g++ -pthread -o scs_svr Server.o

Server:
	g++ -c Server.cpp

Client:
	g++ -c Client.cpp -std=gnu++11

clean:
	rm *.o scs_cli scs_svr
