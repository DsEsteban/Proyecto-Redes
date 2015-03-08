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

#define MAX_CONN 200
#define QUEUE_CONN 20

using namespace std;

struct host {
	int fd;
	struct sockaddr_in addr;
};

class tcp_server {
	private:
		struct host *clients[MAX_CONN];
		struct sockaddr_in sock_addr;
		int n_clients;
		int data_size;
		int sock_fd;
		int port;
	public:
		tcp_server(int size);
		~tcp_server();
		void do_close(int n);
		int do_listen(int port);
		int do_accept();
		int do_send(char* data, int size, int n);
		int do_recv(char* data, int size, int n);
};

tcp_server::tcp_server(int size = 1024) {
	data_size = size;
	n_clients = 0;
	sock_fd = -1;
}

tcp_server::~tcp_server() {
	for (int i = 0; i < n_clients; i++) {
		if (clients[i] != NULL) {
			close(clients[i]->fd);
			free(clients[i]);
		}
	}
	close(sock_fd);
}

void tcp_server::do_close(int n) {
	if (clients[n] == NULL) return;
	close(clients[n]->fd);
	free(clients[n]);
}

int tcp_server::do_listen(int port) {
	
	/* Se crea un socket si antes no ha sido creado */
	if (sock_fd == -1) {
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		/* Capturamos el error al crear el socket */
		if (sock_fd == -1) {
			cout<<"No se pudo crear el Socket"<<endl;
			return -1;
		}
		cout<<"Socket creado con exito"<<endl;
	} else {
		/* Si ya existe una conexion se libera y se crea otra*/
		close(sock_fd);
		sock_fd = -1;
		return do_listen(port);
	}

	/* Configuramos la conexion con el servidor */
	sock_addr.sin_family      = AF_INET;                // Familia de direcciones para IPv4
	sock_addr.sin_port        = htons(port);            // Usa orden de bytes de la red
	sock_addr.sin_addr.s_addr = INADDR_ANY;             // Escuchamos en todas las ip
	
	/* Se le  asocia el puerto al filedescriptor */
	if (bind(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		cout<<"Error al asociar un puerto al socket."<<endl;
		return -1;
	}
	
	/* Se le  asocia el puerto al filedescriptor */
	if (listen(sock_fd, QUEUE_CONN) == -1) {
		cout<<"Error al colocar el puerto en modo de escucha."<<endl;
		return -1;
	}
	return 0;
}

int tcp_server::do_accept() {
	struct host *client = (struct host*) malloc(sizeof(struct host));
	int size_client = sizeof(client->addr);
	int sockfd_client = accept(sock_fd, (struct sockaddr*) &(client->addr), &size_client);
	
	/* Captura del error al crear el socket */
	if (sockfd_client == -1) {
		cout<<"Error al crear el Socket para la comunicacion con el cliente."<<endl;
		free(client);
		return -1;
	}
	client->fd = sockfd_client;
	clients[n_clients] = client;
	return n_clients++;
}

int tcp_server::do_send(char* data, int size, int n) {
	if (size > data_size) {
		cout<<"La cadena de caracteres supera el maximo tamaño permitido."<<endl;
		return -1;
	}
	if (send(clients[n]->fd, data, size, 0) == -1) {
		perror("Envio fallido.");
		return -1;
	}
	return 0;
}

int tcp_server::do_recv(char* data, int size, int n) {
	int m;
	if (size < data_size) {
		cout<<"La cadena de caracteres es menor que el minimo tamaño requerido."<<endl;
		return -1;
	}
	
	m = recv(clients[n]->fd, data, size, 0);
	if (m == -1) {
		cout<<"Recepcion fallida."<<endl;
		return -1;
	}
	if (m == 0) {
		cout<<"Conexion caida."<<endl;
		return -1;
	}
	data[m] = '\0';
	return 0;
}

int main(int argc, char** argv){
#ifdef __WIN32__
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2), &WSAData);
#endif
	
	tcp_server conexion;
	char* file;
	int port;
	int n;
	char buffer[1024];
	char c;
	int flagb = 0;
	int flagl = 0;
	
	if (argc != 5) {
		cout<<"Uso: scs_svr -l <pto_lcl> -b <file_log>.\n"<<endl;
		return(1);
	}
	
	opterr = 0;
	while ((c = getopt (argc, argv, "b:l:")) != -1) {
		switch (c) {
			case 'l':
				port = strtol(optarg, NULL, 10);
				flagl++;
				break;
			case 'b':
				file = optarg;
				flagb++;
				break;
			case '?':
				cout<<"Error parametro invalido."<<endl;
				return 1;
      }
	}
	
	if (!flagb || !flagl) {
		cout<<"No se encontraron los paramtros requeridos."<<endl;
		return 1;
	}
	
	conexion.do_listen(port);
	int client = conexion.do_accept();
	
	/* Se lee de entrada estandar y se envia al servidor */
	while (1){
		if (conexion.do_recv(buffer, 1024, client) == 0) {
			cout<<"Mensaje recibido: "<<buffer<<endl;
		} else break;
	}
	cout<<"Conexion terminada. Programa finalizado\n\n";
	return 0;
}
