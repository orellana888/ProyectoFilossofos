// PROGRAMA DE LA CENA DE LOS FILOSOFOS COMENSALES //
// HENRY ORELLANA - 20192001334 //
#include <iostream>
#include <pthread.h> // Biblioteca para hilos
#include <unistd.h> // Biblioteca para gestion de procesos 
#include <cstdlib> // Biblioteca para menejo de memoria 
#include <ctime>  // Biblioteca para hora
#include <semaphore.h> // Biblioteca para sem�foros

using namespace std;

int comida;

class Tenedor {
public:
    Tenedor() : estado(0) {} // Constructor de la clase que inicializa el estado del tenedor en 0
    int getEstado() const { return estado; } // Funci�n que devuelve el estado del tenedor
    void setEstado(int nuevoEstado) { estado = nuevoEstado; } // Funci�n que cambia el estado del tenedor a nuevoEstado
private:
    int estado; // Variable que representa el estado del tenedor, donde 0 significa que est� libre y 1 que est� ocupado
};


class Semaphore { // Se define la clase Semaphore
public:
    Semaphore(int n) : count(n) { // Constructor de la clase Semaphore que toma un argumento de tipo entero n y lo inicializa como valor inicial del contador
        sem_init(&s, 0, n); // Inicializa el sem�foro con valor n
    }

    ~Semaphore() { // Destructor de la clase Semaphore
        sem_destroy(&s); // Destruye el sem�foro
    }

    void wait() { // Funci�n wait de la clase Semaphore, que decrementa el contador del sem�foro
        sem_wait(&s); // Espera a que el sem�foro tenga un valor mayor a cero y lo decrementa
        count--; // Decrementa el contador de la clase Semaphore
    }

    void signal() { // Funci�n signal de la clase Semaphore, que incrementa el contador del sem�foro
        sem_post(&s); // Incrementa el sem�foro
        count++; // Incrementa el contador de la clase Semaphore
    }

    int getCount() const { // Funci�n getCount de la clase Semaphore, que devuelve el valor actual del contador
        return count; // Devuelve el valor actual del contador de la clase Semaphore
    }

private:
    sem_t s; // Se define el sem�foro como una variable miembro de la clase Semaphore
    int count; // Contador que representa el valor actual del sem�foro
};


class Filosofo {
public:
    Filosofo(const string& n, int c, Tenedor* t1, Tenedor* t2, Semaphore* s) : nombre(n), comida(c), ten1(t1), ten2(t2), sem(s) {} // Constructor de la clase Filosofo
    void comer() { // Funci�n comer de la clase Filosofo
        cout << nombre << " est� pensando." << endl;

        while (comida > 0) {
            sem->wait(); // El fil�sofo espera a que haya suficientes sem�foros disponibles para tomar ambos tenedores
            if (ten1->getEstado() == 0 && ten2->getEstado() == 0) {
                cout << nombre << " tiene hambre." << endl;
                ten1->setEstado(1); // toma un tenedor
                ten2->setEstado(1); // toma otro tenedor 
                sem->signal(); // Despu�s de tomar ambos tenedores, el fil�sofo libera un sem�foro
                cout << nombre << " agarr� los dos tenedores." << endl;
                while (comida > 0) {
                    comida--;
                    cout << nombre << " est� comiendo." << endl;
                    sleep(1);
                }
                // se liberan ambos tenedores
                ten1->setEstado(0);
                ten2->setEstado(0);
                sem->signal(); // Despu�s de terminar de comer, el fil�sofo libera ambos tenedores y dos sem�foros
                sem->signal();
                cout << nombre << " termin� de comer." << endl;
            }
            else {
                sem->signal(); // Si no pudo tomar ambos tenedores, libera un sem�foro y espera antes de volver a intentar tomarlos
                cout << nombre << " no puede comer." << endl;
                sleep(1);
            }
        }
    }

private: // Variables miembro de la clase Filosofo
    string nombre;
    int comida;
    Tenedor* ten1;
    Tenedor* ten2;
    Semaphore* sem;
};

void* comer(void* arg) {
    Filosofo* filosofo = (Filosofo*)arg;
    filosofo->comer();
    return NULL;
}

int main() {
   	srand(time(NULL)); // Inicializar la semilla para los n�meros aleatorios
	comida = rand() % 10 + 1; // Generar un n�mero aleatorio entre 1 y 10

    
    Tenedor* tenedores[5]; // Creamos un arreglo de 5 punteros a objetos de la clase Tenedor
	Filosofo* filosofos[5]; // Creamos un arreglo de 5 punteros a objetos de la clase Filosofo
	
	for (int i = 0; i < 5; i++) { // Iteramos sobre el arreglo de tenedores y creamos un nuevo objeto Tenedor para cada posici�n del arreglo
	    tenedores[i] = new Tenedor();
	}
	
	// creamos una instancia de la clase Filosofo y lo guardamos en el arreglo 'filosofos[]' 
	Semaphore* sem = new Semaphore(4); // Creamos un sem�foro con capacidad para 4 tenedores disponibles simult�neamente
	filosofos[0] = new Filosofo("Platon", comida, tenedores[0], tenedores[1], sem);
	filosofos[1] = new Filosofo("Descartes", comida, tenedores[1], tenedores[2], sem);
	filosofos[2] = new Filosofo("Nietsche", comida, tenedores[2], tenedores[3], sem);
	filosofos[3] = new Filosofo("Hegel", comida, tenedores[3], tenedores[4], sem);
	filosofos[4] = new Filosofo("Aristoteles", comida, tenedores[4], tenedores[0], sem);
	
	pthread_t threads[5]; // declara un arreglo de tipo pthread_t con capacidad para 5 hilos. 
	
	for (int i = 0; i < 5; i++) {
	    pthread_create(&threads[i], NULL, comer, filosofos[i]);
	}
	
	for (int i = 0; i < 5; i++) {
	    int result = pthread_join(threads[i], NULL);
	    if (result != 0) {
	        cout << "Error uni�ndose al hilo para el fil�sofo " << i << endl;
	    }
	}
	
	delete sem; // eliminando el objeto Semaphore din�micamente creado
	
	for (int i = 0; i < 5; i++) { // Liberamos la memoria reservada para los objetos Tenedor
	    delete tenedores[i];
	}
	
	for (int i = 0; i < 5; i++) { // Liberamos la memoria reservada para los objetos Filosofo
	    delete filosofos[i];
	}
	
	return 0;

}


