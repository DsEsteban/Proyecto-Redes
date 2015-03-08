#ifdef __WIN32__
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

class tcp_client {
	private:
		struct sockaddr_in sock_addr;
		char server_addr[32];
		int data_size;
		int sock_fd;
		int port;
	public:
		tcp_client(int size);
		~tcp_client();
		int do_connect(char* address, int port);
		int do_send(char* data,  int size);
		int do_recv(char** data, int size);
};

tcp_client::tcp_client(int size = 1024) {
	data_size = size;
	sock_fd = -1;
}

tcp_client::~tcp_client() {
	close(sock_fd);
}

int tcp_client::do_connect(char* address, int port) {
	
	/* Se crea un socket si antes no ha sido creado */
	if (sock_fd == -1) {
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		/* Capturamos el error al crear el socket */
		if (sock_fd == -1) {
			perror("No se pudo crear el Socket.");
			return -1;
		}
		cout<<"Socket creado con exito"<<endl;
	} else {
		/* Si ya existe una conexion se libre y se crea otra*/
		close(sock_fd);
		sock_fd = -1;
		return do_connect(address, port);
	}

	/* Configuramos la conexion con el servidor */
	sock_addr.sin_family      = AF_INET;                // Familia de direcciones para IPv4
	sock_addr.sin_port        = htons(port);            // Usa orden de bytes de la red
	sock_addr.sin_addr.s_addr = inet_addr(address);     // String a Decimal
	
	/* Capturamos si el formato de la dirección ip no es correcto */
	if (sock_addr.sin_addr.s_addr == -1) {
		perror("Formato ip invalido.");
		return -1;
	}
	
	if (connect(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		perror("Conexion rechazada por el servidor.");
		return -1;
	}
	return 0;
}

int tcp_client::do_send(char* data, int size) {
	if (size > data_size) {
		perror("La cadena de caracteres supera el maximo tamaño posible.");
		return -1;
	}
	if (send(sock_fd, data, strlen(data), 0) == -1) {
		perror("Envio fallido.");
		return -1;
	}
	return 0;
}

int tcp_client::do_recv(char** data, int size) {
	if (size < data_size) {
		perror("La cadena de caracteres es menor que el tamaño requerido.");
		return -1;
	}
	if (recv(sock_fd, *data, data_size, 0) == -1) {
		perror("Recepcion fallida.");
		return -1;
	}
	return 0;
}

int main(int argc, char** argv){
#ifdef __WIN32__
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2), &WSAData);
#endif
	
	tcp_client conexion;
	char* server_addr;
	int server_port;
	int local_port;
	int n;
	char buffer[1024];
	char c;
	
	if (argc < 5) {
		cout<<"Uso: scs_cli -d <dir> -p <pto_svr> [-l <pto_lcl>].\n"<<endl;
		return(1);
	}
	
	opterr = 0;
	while ((c = getopt (argc, argv, "d:p:l:")) != -1) {
		switch (c) {
			case 'd':
				server_addr = optarg;
				break;
			case 'p':
				server_port = strtol(optarg, NULL, 10);
				break;
			case 'l':
				local_port  = strtol(optarg, NULL, 10);
				break;
			case '?':
				cout<<"Error parametro invalido."<<endl;
				return(1);
      }
	}
	
	
	conexion.do_connect(server_addr, server_port);
	
	/* Se lee de entrada estandar y se envia al servidor */
	while (1){
		cin>>buffer;
		n = strlen(buffer);
		if (n == 1) break;
		if (conexion.do_send(buffer, n) == 0) {
			cout<<"Mensaje enviado"<<endl;
		}
	}
	
	cout<<"Conexion terminada. Programa finalizado\n\n";
	return 0;
}
