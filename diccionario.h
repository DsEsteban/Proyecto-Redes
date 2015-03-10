#ifndef _Diccionario_h
#define _Diccionario_h

#include <cassert>
#include <utilidades.h>
template <class Tk,class Ti>
class Diccionario {
private:
 Par<Tk,Ti> * parejas; 
 int nelementos; 
 int reservados; 
 bool posicion_indice(int& pos, const Tk& llave) const;
 void expandir();
 void contraer();
public:
/*--------------------------- Iteradores--------------------------- */
class const_iterador {
   private:
	 Par<Tk,Ti>* puntero;
	 const_iterador(Par<Tk,Ti>* p): puntero(p) {}
   public:
	 const_iterador(): puntero(0) {}
	 const_iterador(const const_iterador& it): puntero(it.puntero){}
	 //const_iterador(iterador v): puntero(v.puntero){} // no necesaria
	 const Par<Tk,Ti>& operator*() const
	   { assert(puntero!=0);return *puntero; }
	 const_iterador& operator++() {
		 assert(puntero!=0);puntero++;return *this;}
	 const_iterador& operator--() {
		 assert(puntero!=0);puntero--;return *this;}
	 bool operator!=(const const_iterador& v) const{
	   return puntero!=v.puntero; }
	 bool operator==(const const_iterador& v) const{
	   return puntero==v.puntero; }
	 friend class Diccionario<Tk,Ti>;
 };
 const_iterador begin() const {return const_iterador(parejas);}
 const_iterador end() const {return const_iterador(parejas+nelementos);}
 typedef const_iterador iterador;
 /*------------------------ fin iteradores-------------------------- */

 Diccionario<Tk,Ti>();
 Diccionario<Tk,Ti>(const Diccionario<Tk,Ti>& orig);
 ~Diccionario<Tk,Ti>();
 Diccionario<Tk,Ti>& operator= (const Diccionario<Tk,Ti>& original);
 void insertar(const Tk& llave, const Ti& valor);
iterador borrar(iterador it);
iterador buscar(const Tk& llave);
int num_elementos() const { return nelementos; }

};

#include <diccionario.cpp>

#endif /* _Diccionario_h */
