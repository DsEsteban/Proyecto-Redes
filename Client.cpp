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
#include "regex"
//#include "hash.h"


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

char* contenido_comando(char* buffer) {
	char num_comando[3];
	
    // Comandos no privilegiados
	if (strcmp("conectarse", buffer)==0)
		memcpy(num_comando, "00",3);
	else if (strcmp("salir", buffer)==0)
	    memcpy(num_comando, "01",3);
	else if (strcmp(buffer,"entrar")==0)
	    memcpy(num_comando, "02",3);
	else if (strcmp(buffer,"dejar")==0)
	    memcpy(num_comando, "03",3);
	else if (strcmp(buffer,"ver_salas")==0)
	    memcpy(num_comando, "04",3);
	else if (strcmp(buffer,"ver_usuarios")==0)
	    memcpy(num_comando, "05",3);
	else if (strcmp(buffer,"ver_usu_salas")==0)
	    memcpy(num_comando, "06",3);
	else if (strcmp(buffer,"help")==0)
	    memcpy(num_comando,"07",3);
	//Comandos privilegiados
	else if (strcmp(buffer,"crear_usu")==0)
	    memcpy(num_comando, "10",3);
	else if (strcmp(buffer,"elim_usu")==0)
	    memcpy(num_comando, "11",3);
	else if (strcmp(buffer,"crear_sala")==0)
	    memcpy(num_comando, "12",3);
	else if (strcmp(buffer,"elim_sala")==0)
	    memcpy(num_comando, "13",3);
	else if (strcmp(buffer,"ver_salas")==0)
	    memcpy(num_comando, "14",3);
	else if (strcmp(buffer,"ver_usuarios")==0)
	    memcpy(num_comando, "15",3);
	else if (strcmp(buffer,"hab_sala")==0)
	    memcpy(num_comando, "16",3);
	else if (strcmp(buffer,"deshab_sala")==0)
	    memcpy(num_comando, "17",3);
	else if (strcmp(buffer,"ver_log")==0)
	    memcpy(num_comando, "18",3);
	else
	    memcpy(num_comando, "-1",3);
	    
	return num_comando;	
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
	
	cout<<"\n"<<endl;
	cout<<"Si deseas ingresar al SCS introduce el comando 'conectarse', si no, introduce el comando 'salir'."<<endl;
	cout<<"\n"<<endl;
	cin>>buffer;
	
	while (strcmp(buffer,"conectarse")!=0 && strcmp(buffer,"salir")!=0 ) {
		cout<<"No. Si deseas ingresar al SCS introduce el comando 'conectarse', si no, introduce el comando 'salir'."<<endl;
		cout<<"\n"<<endl;
		cin>>buffer;
	}	
	
	if (strcmp(buffer,"salir")==0) 
		return 0;
	
	cout<<"Bienvenido. Luego de ingresar al SCS si desea ver una lista de comandos disponiblen introduzca 'help'."<<endl;
	cout<<"Por favor ingrese su nombre de usuario: ";
	cin>>buffer;
	
	/*cambios inicio*/
	//hash* comandos = new hash();
		
	/*cambios inicio*/
	
	
	len = strlen(buffer);
	
	while (!regex_match(buffer, regex("(\\w)+")) || len > 15 || len < 2) {
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
	char* numcom;
	
	while (1) {
		cin.getline(buffer, BUFF_SIZE);
		len = strlen(buffer);
		if (len == 0) continue;
		numcom = contenido_comando(buffer);
		if (strcmp(numcom,"-1")!=0) {
			if (strcmp(numcom,"07")==0) {
				cout<<"\n"<<endl;
				cout<<"Comandos disponibles: "<<endl;
				cout<<"    - 'salir': ejecuta un logout del SCS."<<endl;
				cout<<"    - 'entrar': entra a una sala activa en el SCS."<<endl;
				cout<<"    - 'dejar': sale de la sala del SCS en la cual se encontraba."<<endl;
				cout<<"    - 'ver_salas': muestra la lista de las salas activas en el SCS."<<endl;
				cout<<"    - 'ver_usuarios': muestra la lista de usuarios declarados en el SCS."<<endl;
				cout<<"    - 'ver_usu_salas': muestra la lista actual de usuarios suscritos a una sala del SCS."<<endl;
				cout<<"\n"<<endl;
			} else {
				num = conexion.do_send(numcom, 2);
				if (num < 1) {
					cout<<"Error: mensaje no enviado."<<endl;
					break;
				}
			}	
		} else {
			num = conexion.do_send(buffer, len);
			if (num < 1) {
				cout<<"Error: mensaje no enviado."<<endl;
				break;
			}
		}	
	}

	cout<<"Conexion terminada. Programa finalizado.\n\n";
	return 0;
}
