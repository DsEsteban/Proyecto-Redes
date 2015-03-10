#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

#ifndef HASH_H
#define HASH_H

class hash
{
private:
	static const int tableSize = 10;

	struct item{
		string user;
		string user_id;
		string pass;
		item* next;

	};

	item* HashTable[tableSize];

public:
	hash();
	/* devuelve 1 si el usuario existe, 0 en caso contrario*/
	int usuario_valido(string user);
	/* devuelve 1 si la clave es correcta, 0 en caso contrario */
	int verificar_clave(string user, string password);
	/* devuele la clave de hash para un string*/
	int	clave_de_Hash(string key);
	/* agrega un usuario a la tabla de hash */
	void crear_usuario(string user_name, string password);
	/* devuelve una lista de todos los usuarios en la tabla de hash*/
	void ver_usuarios();
	/* eliminar usuario*/
	void eliminar_usuario(string user);
	
};


#endif