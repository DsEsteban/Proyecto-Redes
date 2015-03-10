#include <cstdlib>
#include <iostream>
#include <string>

#include "hash.h"

using namespace std;

hash::hash(){
	for(int i = 0; i <tableSize; i++){
		HashTable[i] = new item;
		HashTable[i]->user = "empty";
		HashTable[i]->user_id = "empty";
		HashTable[i]->pass = "empty";
		HashTable[i]->next = NULL;

	}
}

int hash::usuario_valido(string user){
	int response = 2;
	int index = clave_de_Hash(user);
	item* Ptr = HashTable[index];
	while(Ptr->next != NULL){
		if(Ptr->user == user){
			response = 1;
			Ptr = Ptr->next;
		}else{
			Ptr = Ptr->next;
		}
	}
	if(response > 1){
		if(Ptr->user == user){
			response = 1;
		}else{
			response = 0;
		}

	}

	return response;
}

int hash::verificar_clave(string user, string password){
	int index = clave_de_Hash(user);
	item* Ptr = HashTable[index];
	while(Ptr->next != NULL){
		if(Ptr->user == user){
			if(Ptr->pass == password){
				return 1;
			}else{
				return 0;
			}
		}
	}	

}

void hash::crear_usuario(string user, string password){
	int index = clave_de_Hash(user);
	if(usuario_valido(user)){
		cout << "nombre de usuario ya existente: " << user <<endl;
	}else{


		if(HashTable[index]->user == "empty"){
			HashTable[index]->user = user;
			HashTable[index]->pass = password;
		}
		else{
			item* Ptr = HashTable[index];
			item* n = new item;
			n->user = user;
			n->pass = password;
			n->next = NULL;
			while (Ptr->next != NULL)
			{
				Ptr = Ptr->next;
			}
			Ptr->next = n;
		}
	}
}

void hash::eliminar_usuario(string user){
	if(usuario_valido(user)){
		int index = clave_de_Hash(user);
		item* Ptr = HashTable[index];
		if(Ptr->user == user){
			HashTable[index] = Ptr->next;
			delete(Ptr);
		}else{
			while(Ptr->next->user != user){
				Ptr = Ptr->next;
			}
			item* PtrAux = Ptr->next->next;
			Ptr->next->next = NULL;
			delete(Ptr->next);
			Ptr->next = PtrAux;
		}


	}else{
		cout << "el usuario no puede ser eliminado porque no existe" << endl;
	}
}

void hash::ver_usuarios(){
	int number;
	cout << "----------------------------\n";
	cout << "Lista de usuarios : \n";
	for (int i = 0; i < tableSize; i++)
	{

		item* Ptr = HashTable[i];
		while( Ptr->next != NULL){
			cout << Ptr->user << endl;
			Ptr = Ptr->next;
		}
		if(Ptr->user != "empty"){
			cout << Ptr->user << endl;
		}
	}
};

int hash::clave_de_Hash(string key)
{
	int hash = 0;
	int index;

	for (int i = 0; i < key.length(); i++)
	{
		hash = hash + (int)key[i];
	}
	index = hash % tableSize;

	return index;
}
