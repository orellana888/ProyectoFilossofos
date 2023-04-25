import sys
import threading
import time

# Clase para manejar semáforos
class Semaforo(object):

    def __init__(self, initial):
        # Utiliza un threading.Condition y un threading.Lock para sincronización
        self.lock = threading.Condition(threading.Lock())
        self.value = initial

    # Método para incrementar el valor del semáforo
    def up(self):
        with self.lock:
            self.value += 1
            self.lock.notify()

    # Método para decrementar el valor del semáforo
    def down(self):
        with self.lock:
            while self.value == 0:
                self.lock.wait()
            self.value -= 1

# Clase para representar un palillo
class Palillo(object):

    def __init__(self, number):
        # El número del palillo
        self.number = number
        # El número del filósofo que está usando el palillo (-1 si no está siendo usado)
        self.user = -1
        # Utiliza un threading.Condition y un threading.Lock para sincronización
        self.lock = threading.Condition(threading.Lock())
        # Indica si el palillo está siendo usado actualmente
        self.taken = False

    # Método para tomar el palillo
    def take(self, user):
        with self.lock:
            while self.taken == True:
                self.lock.wait()
            self.user = user
            self.taken = True
            # Mensaje para indicar que el filósofo ha tomado el palillo
            sys.stdout.write("Filosofo[%s] toma el palillo:%s\n" % 
                             (user, self.number))
            self.lock.notify_all()

    # Método para soltar el palillo
    def drop(self, user):
        with self.lock:
            while self.taken == False:
                self.lock.wait()
            self.user = -1
            self.taken = False
            # Mensaje para indicar que el filósofo ha soltado el palillo
            sys.stdout.write("Filosofo[%s] deja el palillo:%s\n" % 
                             (user, self.number))
            self.lock.notify_all()

# Clase para representar un filósofo
class Filosofo (threading.Thread):

    def __init__(self, number,left,right,butler):
        threading.Thread.__init__(self)
        # El número del filósofo
        self.number = number
        # El palillo izquierdo
        self.left = left
        # El palillo derecho
        self.right = right
        # El mayordomo (semáforo)
        self.butler = butler

    def run(self):
        # El filósofo come una sola vez
        for i in range(1):
            # El filósofo pide permiso al mayordomo para sentarse a la mesa
            self.butler.down()
            # El filósofo piensa durante un tiempo aleatorio
            print("Filosofo", self.number, "piensa")
            time.sleep(0.1)
            # El filósofo toma el palillo izquierdo
            self.left.take(self.number)
            time.sleep(0.1)
            # El filósofo toma el palillo derecho
            self.right.take(self.number)
            # El filósofo come durante un tiempo aleatorio
            print("Filosofo", self.number, "come 🥳")
            time.sleep(0.1)  # Espera 0.1 segundos
            self.right.drop(self.number)  # Suelta el palillo de su derecha
            self.left.drop(self.number)  # Suelta el palillo de su izquierda
            self.butler.up()  # Libera el permiso de tomar palillos
        sys.stdout.write("Filosofo[%s] termina de pensar y comer\n" % self.number)  # Imprime que el filósofo ha terminado de pensar y comer

def main():

    n = 6

    butler = Semaforo(n-1)

    c = [Palillo(i) for i in range(n)]

    p = [Filosofo(i, c[i], c[(i+1) % n], butler) for i in range(n)]

    for i in range(n):
        p[i].start()

if __name__ == "__main__":
    main() 