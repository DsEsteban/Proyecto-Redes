 #include <cassert>
 #include <diccionario.h>
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 bool Diccionario<Tk,Ti>::posicion_indice(int& pos, const Tk& llave) const
 {
   int izq=0, der=nelementos-1,centro;
 
   while (der-izq>=0) {
     centro=(izq+der)/2;
     if (llave<parejas[centro].primero)
       der=centro-1;
     else if (llave>parejas[centro].primero)
            izq=centro+1;
          else {
            pos=centro;
            return true;
          }
   }
   pos= izq;
   return false;
 }
 
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 void Diccionario<Tk,Ti>::expandir()
 {
   reservados= (nelementos==0)?1:nelementos*2;
   Par<Tk,Ti> * nuevas_parejas= new Par<Tk,Ti>[reservados];
   for (int j= 0; j<nelementos ;++j)
     nuevas_parejas[j]= parejas[j];
   delete[] parejas;
   parejas= nuevas_parejas;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 void Diccionario<Tk,Ti>::contraer()
 {
   assert(nelementos<=reservados/2);
   reservados= reservados/2;
   Par<Tk,Ti> * nuevas_parejas= new Par<Tk,Ti>[reservados];
   for (int j= 0; j<nelementos ;++j)
     nuevas_parejas[j]= parejas[j];
   delete[] parejas;
   parejas= nuevas_parejas;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>::Diccionario<Tk,Ti>()
 {
   parejas=0;
   nelementos=reservados=0;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>::Diccionario<Tk,Ti>(const Diccionario<Tk,Ti>& orig)
 {
   reservados= nelementos= orig.nelementos;
   if (nelementos>0) {
     parejas= new Par<Tk,Ti>[nelementos];
     for (int i=0; i<nelementos;++i)
       parejas[i]= original.parejas[i];
   }
   else parejas=0;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>::~Diccionario<Tk,Ti>()
 {
   if (reservados>0)
     delete[] parejas;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>&
     Diccionario<Tk,Ti>::operator= (const Diccionario<Tk,Ti>& original)
 {
   if (this!= &original) {
     if (reservados>0)
       delete[] parejas;
     reservados= nelementos= orig.nelementos;
     if (nelementos>0) {
       parejas= new Par<Tk,Ti>[nelementos];
       for (int i=0; i<nelementos;++i)
         parejas[i]= original.parejas[i];
     }
     else parejas=0;
   }
   return *this;
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 void Diccionario<Tk,Ti>::insertar(const Tk& llave, const Ti& valor)
 {
   int pos;
   if (posicion_indice(pos,llave)) // ya está
     parejas[pos].segundo=valor;
   else { // hay que insertarlo
       if (nelementos==reservados)
          expandir();
       for (int j=nelementos; j>pos; --j)
           parejas[j]= parejas[j-1];
       parejas[pos].primero= llave;
       parejas[pos].segundo= valor;
       ++nelementos;
   }
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>::iterador Diccionario<Tk,Ti>::borrar(iterador it)
 {
   assert(it!=end());
   nelementos= nelementos-1;
   int pos=it.puntero-parejas;
   for (int j=pos;j<nelementos;++j)
          parejas[j]=parejas[j+1];
   if (nelementos<reservados/4)
         contraer();
 }
 
 /* _________________________________________________________________________ */
 
 template<class Tk, class Ti>
 Diccionario<Tk,Ti>::iterador Diccionario<Tk,Ti>::buscar(const Tk& llave)
 {
   int pos;
   if (posicion_indice(pos,llave))
     return iterador(parejas+pos);
   else return iterador(parejas+nelementos); // end()
 }
 
 /* Fin diccionario.cpp */
