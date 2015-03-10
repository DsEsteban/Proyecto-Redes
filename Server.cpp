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

#define BUFF_SIZE 1024
#define MAX_CONN 200
#define QUEUE_CONN 20
#define MAX_MIEMBROS 10

using namespace std;

struct personas_chat {
	char* user_name;
	int id;
	bool es_superusuario;
	int sala_num;
};

class sala {
	private:
		char* nombre_sala;
		int num_sala;
		struct personas_chat* miembros[MAX_MIEMBROS];
		int num_personas;
		bool habilitada;
	public:
		sala(char* nombre_sala, int num);
		~sala();
		int agregar_usuario(struct personas_chat* usuario);
		int eliminar_usuario(struct personas_chat* usuario);
		int listar_usuarios();
		int habilitar();
		int deshabilitar();
};

// Crear sala
sala::sala(char* nombre, int num) {
	nombre_sala = nombre;
	num_personas = 0;
	num_sala = num;
	habilitada = true;
}

// Eliminar sala
sala::~sala() {
	for (int i = 0; i < num_personas; i++) {
		if (miembros[i] != NULL) {
			miembros[i]->sala_num = -1;
			miembros[i] = NULL;
		}
	}
}

// hab_sala
int sala::habilitar() {
	if (habilitada == true)
		return 0;
	else {
		habilitada = true;
		return 1;
	}	
}

// deshab_sala
int sala::deshabilitar() {
	if (habilitada == false)
		return 0;
	else {
		for (int i = 0; i < num_personas; i++) {
			if (miembros[i] != NULL) {
				miembros[i]->sala_num = -1;
				miembros[i] = NULL;
			}
		}
		habilitada = false;
		return 1;
	}	
}

// entrar
int sala::agregar_usuario(struct personas_chat* usuario) {
	if (num_personas == MAX_MIEMBROS)
		return 0;
	else	
		usuario->sala_num = num_sala;
		miembros[num_personas] = usuario;
		num_personas += 1;
		return 1;
}

// dejar
int sala::eliminar_usuario(struct personas_chat* usuario) {
	if (num_personas == 0)
		return 0;
	else {
		for (int i = 0; i < num_personas; i++) {
			if (miembros[i]->id == usuario->id) {
				if (i == (num_personas-1)) {
					usuario->sala_num = -1;
					miembros[i] = NULL;
					num_personas -= 1;
					return 1;
				} else {
					usuario->sala_num = -1;
					miembros[i] = miembros[num_personas-1];
					miembros[num_personas-1] = NULL;
					num_personas -= 1;
					return 1;
				}
			}
		}
	}	
}

// ver_usu_salas
int sala::listar_usuarios() {
	cout<<"Usuarios de la sala "<<nombre_sala<<endl;
	for (int i = 0; i < num_personas; i++) {
		cout<<miembros[i]<<endl;
	}
}

struct host {
	int fd;
	struct sockaddr_in addr;
};

class tcp_server {
	private:
		struct host *clients[MAX_CONN];
		int n_clients;
		int data_size;
		int sock_fd;
		int port;
	public:
		tcp_server(int size);
		~tcp_server();
		void do_close(int n);
		int get_clients();
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
	if (n < 0 || n >= n_clients) return;
	if (clients[n] == NULL) return;
	close(clients[n]->fd);
	free(clients[n]);
	clients[n] = NULL;
	//n_clients--;
	//if (n != n_clients) clients[n] = clients[n_clients];
	//clients[n_clients] = NULL;
}

int tcp_server::get_clients() {
	return n_clients;
	}

int tcp_server::do_listen(int port) {
	sockaddr_in sock_addr;
	
	/* Se verifica si ya el socket fue creado */
	if (sock_fd == -1) {
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		/* Capturamos el error al crear el socket */
		if (sock_fd == -1) {
			cout<<"Falla al crear el Socket."<<endl;
			return -1;
		}
		cout<<"Socket creado con exito."<<endl;
	} else {
		cout<<"Error ya existe una conexion."<<endl;
		return -1;
	}

	/* Configuramos la conexion con el servidor */
	memset(&sock_addr, 0, sizeof(sockaddr_in));
	sock_addr.sin_family      = AF_INET;                // Familia de direcciones para IPv4
	sock_addr.sin_port        = htons(port);            // Usa orden de bytes de la red
	sock_addr.sin_addr.s_addr = INADDR_ANY;             // Escuchamos en todas las ip
	
	/* Se le  asocia el puerto al filedescriptor */
	if (bind(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		cout<<"Error al asociar un puerto al socket."<<endl;
		return -1;
	}
	
	/* Se coloca en modo de escucha */
	if (listen(sock_fd, QUEUE_CONN) == -1) {
		cout<<"Error al colocar el puerto en modo de escucha."<<endl;
		return -1;
	}
	return 0;
}

int tcp_server::do_accept() {

	if (n_clients == MAX_CONN) {
		cout<<"Se alcanzo el numero maximo de conexiones."<<endl;
		return 0;
	}
	
	host *client = (host*) malloc(sizeof(host));
	int size_client = sizeof(client->addr);
	int sockfd_client = accept(sock_fd, (sockaddr*) &(client->addr),
		(socklen_t*) &size_client);
	
	/* Captura del error al crear el socket */
	if (sockfd_client == -1) {
		cout<<"Error al aceptar la conexion con el cliente."<<endl;
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
	int m = send(clients[n]->fd, data, size, 0);
	if (m == -1)
		cout<<"Envio fallido."<<endl;

	return m;
}

int tcp_server::do_recv(char* data, int size, int n) {
	if (size < data_size) {
		cout<<"La cadena de caracteres es menor que el minimo tamaño requerido."<<endl;
		return -1;
	}
	
	int m = recv(clients[n]->fd, data, size, 0);
	if (m == -1) {
		cout<<"Recepcion fallida."<<endl;
		return m;
	}
	if (m == 0) {
		cout<<"Conexion caida."<<endl;
		do_close(n);
		return m;
	}
	data[m] = '\0';
	return m;
}

/* Variable global */
tcp_server conexion;
pthread_mutex_t mutex;

void *recv_fun(void *client) {
	int *n_client = (int*) client;
	int len;
	char buffer[BUFF_SIZE];
	while(1) {
		len = conexion.do_recv(buffer, BUFF_SIZE, *n_client);
		if (len > 0) {
			pthread_mutex_lock(&mutex);
			cout<<"Mensaje recibido: "<<buffer<<endl;
			pthread_mutex_unlock(&mutex);
		} else break;
	}
	free(n_client);
	pthread_exit(NULL);
}

int main(int argc, char** argv){
#ifdef __WIN32__
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2), &WSAData);
#endif
	char* file;
	char opt;
	int port;
	int flagb = 0;
	int flagl = 0;
	
	if (argc != 5) {
		cout<<"Uso: scs_svr -l <pto_lcl> -b <file_log>.\n"<<endl;
		return(1);
	}
	
	opterr = 0;
	while ((opt = getopt (argc, argv, "b:l:")) != -1) {
		switch (opt) {
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
	
	if (conexion.do_listen(port) == -1) {
		cout<<"No se establececio el socket en modo escucha."<<endl;
		return 1;
	} else
		cout<<"Se establecio el socket en modo escucha."<<endl;

	pthread_mutex_init(&mutex, NULL); // Inicializamos el mutex
	pthread_t recv_thread;

	int *client =  (int*) malloc(sizeof(int));
	/* Se aceptan conexiones mientras se permita, esto es mientras no se
	alcance el maximo de conexiones */
	while (1) {
		*client = conexion.do_accept();
		if (*client == -1) break; // Error
		if (*client ==  0) {	//Numero maximo de conexiones alcanzado
			sleep(10);
			continue;
		}
		pthread_create(&recv_thread, NULL, &recv_fun, client);
		client =  (int*) malloc(sizeof(int));
	}
	
	cout<<"Programa finalizado.\n\n";
	free(client);
	pthread_mutex_destroy(&mutex); // Des-inicializa el mutex
	return 0;
}
