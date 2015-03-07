#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

int main()
{
	/* code */
	int clientes, servidor;
	int bufsize = 1024;
	int puerto = 1500;
	bool salir=false;
	char* buffer =new char (bufsize);

	struct sockaddr_in direc;
	socklen_t tamano;
	pid_t pid;

	if ((clientes=socket(AF_INET, SOCK_STREAM,0))<0){
		cout<<"Error al crear el socket"<<endl;
		exit(1);

	}

	cout<<"Socket servidor ha sido creado"<<endl;

	direc.sin_family = AF_INET;
	direc.sin_addr.s_addr = htons(INADDR_ANY);
	direc.sin_port = htons(puerto);

	if((bind(clientes,(struct sockaddr*)&direc,sizeof(direc)))<0){
		cout<<"Error en la conexión Bind"<<endl;
		return(-1);
	}

	tamano=sizeof(direc);
	cout<<"Conecte al cliente ... \n";
	listen(clientes,1);

	while((servidor = accept(clientes,(struct sockaddr *)&direc, &tamano))>0){
		strcpy(buffer,"servidor conectado\n");
		send(servidor,buffer,bufsize,0);
		cout<<"conexion con el cliente EXITOSA\n";
		cout<<"Recuerde poner asterisco al final para mandar un mensaje *\nEscriba # para terminar la conexion"<<endl;

		cout<<"Mensaje recibido ";
		do{
			recv(servidor,buffer,bufsize,0); 
			cout<<buffer<<" ";
				if (*buffer == '#')
				{
					*buffer='*';
					salir= true;
				}
		}while(*buffer != '*');

		do{
			cout<<"\nEscriba un mensaje: ";
			do{
				cin>>buffer;
				send(servidor,buffer,bufsize,0);
					if (*buffer == '#salir')
					{
						send(servidor,buffer,bufsize,0);
						*buffer = '*';
						salir = true;
					}
			}while(*buffer != '*');


		}while(!salir);
		cout<<"\nEl servidor termino la conexion con "<<inet_ntoa(direc.sin_addr);
		close(servidor);
		cout<<"\nConecte nuevo cliente."<<endl;
		salir = false;

}
close(clientes);
return 0;
}