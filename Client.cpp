#ifdef __WIN32__
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex>

#define BUFF_SIZE 1024

using namespace std;

class tcp_client {
	private:
		int data_size;
		int sock_fd;
	public:
		tcp_client();
		~tcp_client();
		int do_connect(char* address, int server_port, int local_port);
		int do_send(char* data,  int size);
		int do_recv(char* data,  int size);
};

tcp_client::tcp_client () {
	data_size = 1024;
	sock_fd   = -1;
}

tcp_client::~tcp_client() {
	close(sock_fd);
}

int tcp_client::do_connect(char* address, int server_port, int local_port) {
	sockaddr_in sock_addr;
	
	/* Se verifica si ya el socket fue creado */
	if (sock_fd == -1) {
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		/* Capturamos el error al crear el socket */
		if (sock_fd == -1) {
			cout<<"Falla al crear el Socket."<<endl;
			return -1;
		} else
			cout<<"Socket creado con exito."<<endl;
		
	} else {
		cout<<"Error ya existe una conexion."<<endl;
		return -1;
	}
	
	/* Si se especifico un puerto local se asigna */
	if (local_port != -1) {
		
		memset(&sock_addr, 0, sizeof(sockaddr_in));
		sock_addr.sin_family      = AF_INET;                // Familia de direcciones para IPv4
		sock_addr.sin_port        = htons(local_port);      // Usa orden de bytes de la red
		sock_addr.sin_addr.s_addr = INADDR_LOOPBACK;        // String a long decimal
	
		/* Se le  asocia el puerto local al filedescriptor */
		if (bind(sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
			cout<<"Error al asociar el puerto local al socket."<<endl;
			return -1;
		}
	}

	/* Configuramos la conexion con el servidor */
	memset(&sock_addr, 0, sizeof(sockaddr_in));
	sock_addr.sin_family      = AF_INET;                // Familia de direcciones para IPv4
	sock_addr.sin_port        = htons(server_port);            // Usa orden de bytes de la red
	sock_addr.sin_addr.s_addr = inet_addr(address);     // String a long decimal
	
	/* Capturamos el error si el formato de la dirección ip no es correcto */
	if (sock_addr.sin_addr.s_addr == -1) {
		cout<<"Formato direccion invalido."<<endl;
		return -1;
	}
	
	if (connect(sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		cout<<"Conexion rechazada por el servidor."<<endl;
		return -1;
	}
	
	return 0;
}

int tcp_client::do_send(char* data, int size) {
	if (size > data_size) {
		cout<<"La cadena de caracteres supera el maximo tamaño permitido."<<endl;
		return -1;
	}
	
	int m = send(sock_fd, data, size, 0);
	if (m == -1)
		cout<<"Envio fallido."<<endl;

	return m;
}

int tcp_client::do_recv(char* data, int size) {
	if (size < data_size) {
		cout<<"La cadena de caracteres es menor que el minimo tamaño requerido."<<endl;
		return -1;
	}
	
	int m = recv(sock_fd, data, size, 0);
	if (m == -1) {
		cout<<"Recepcion fallida."<<endl;
		return m;
	}
	if (m == 0) {
		cout<<"Conexion caida."<<endl;
		return m;
	}
	data[m] = '\0';
	return m;
}

/* Variable global */
tcp_client conexion;

void *recv_fun(void *) {
	int len;
	char buffer[BUFF_SIZE];
	while(1) {
		len = conexion.do_recv(buffer, BUFF_SIZE);
		if (len > 0) {
			cout<<"Mensaje recibido: "<<buffer<<endl;
		} else break;
	}
	exit(1);
}

int main(int argc, char** argv){
#ifdef __WIN32__
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2), &WSAData);
#endif
	pthread_t recv_thread;
	char* server_addr;		// Direccion del servidor
	int   server_port = -1;
	int   local_port  = -1;
	int   flagd = 0;
	int   flagp = 0;
	char opt;
	
	if (argc < 5 || argc > 7) {
		cout<<"Uso: scs_cli -d <dir> -p <pto_svr> [-l <pto_lcl>]."<<endl;
		return 1;
	}
	
	opterr = 0;
	while ((opt = getopt (argc, argv, "d:p:l:")) != -1) {
		switch (opt) {
			case 'd':
				server_addr = optarg;
				flagd = 1;
				break;
			case 'p':
				server_port = strtol(optarg, NULL, 10);
				flagp = 1;
				break;
			case 'l':
				local_port  = strtol(optarg, NULL, 10);
				break;
			case '?':
				cout<<"Error parametro invalido."<<endl;
				return(1);
			}
	}
	
	if (!flagd || !flagp) {
		cout<<"No se encontraron los paramatros requeridos."<<endl;
		return 1;
	}
	
	if (conexion.do_connect(server_addr, server_port, local_port) == -1) {
		cout<<"No se pudo realizar la conexion."<<endl;
		return 1;
	} else
		cout<<"Se ha realizado la conexion con exito."<<endl;

	int len, num;
	char buffer[BUFF_SIZE];
	pthread_create(&recv_thread, NULL, &recv_fun, NULL);
	
	cout<<"Bienvenido."<<endl;
	cout<<"Por favor ingrese su nombre de usuario: ";
	cin>>buffer;
	len = strlen(buffer);
	
	while (!regex_match (buffer, regex("[a-zA-Z0-9]+")) || len > 15 || len < 2) {
		cout<<"Por favor use solo letras y numeros."<<endl;
		cout<<"Nombre de u¡suario no mayor a 15 caracteres."<<endl;
		cout<<"Ingrese un nombre de usuario: ";
		cin>>buffer;
		len = strlen(buffer);
	}
	
	num = conexion.do_send(buffer, len);
	if (num < 1) {
		cout<<"Error: mensaje no enviado."<<endl;
		return 1;
	}

	while (1) {
		cin.getline(buffer, BUFF_SIZE);
		len = strlen(buffer);
		if (len == 0) continue;
		num = conexion.do_send(buffer, len);
		if (num < 1) {
			cout<<"Error: mensaje no enviado."<<endl;
			break;
		}
	}

	cout<<"Conexion terminada. Programa finalizado.\n\n";
	return 0;
}
