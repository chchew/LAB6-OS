
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *recurso(void* data);
struct args {FILE* info;};

/*Variables de la cantidad de thread*/

/*Variable de la cantidad de iteraciones*/

/*creamos el semaforo*/
sem_t sem;
/*recursos compartido*/
int recurse = 0;


int main(void){

	/*Creamos y abrimos el archivo txt donde estara la bitacora*/
	FILE *fichero;
	fichero = fopen("BitacoraSemaforo.txt", "wt");

	/*Envio de datos*/
	struct args *archivo = (struct args *)malloc(sizeof(struct args));
	archivo->info = fichero;

	printf("[INFO] Generando archivo BitacoraSemaforo...\n"); 

	/*Encabezado de archivo*/
	fprintf(fichero, "Iniciando programa\n");
	fprintf(fichero, "Creando threads\n");
	
	/*Creamos los thread*/
	pthread_t thread[amount_thread];
	/*Inicializamos el semaforo*/
	sem_init(&sem, 0, 1); 

	/*Ciclo que creara los pthread conforme a la cantidad de pthread deseados
	  el (void*)archivo manda el pthread creado hacia el archivo donde se escribira.
	*/
	for(int i=0; i < amount_thread; i++){
		pthread_create(&thread[i], NULL, *recurso, (void*)archivo);
	}
	/*Ciclo para unir los hilos cuando haya terminado*/
	for(int j=0; j < amount_thread; j++){
		pthread_join(thread[j], NULL);
	}

	/*Se terminara el pograma y se cerrara el archivo*/
	fprintf(fichero, "Fertig!\n");
	fclose(fichero);
	return 0;
}

void *recurso(void* data){
	/*imprimimos cada pthread*/
	fprintf(((struct args*)data)->info, "Iniciando Threads %ld\n", syscall(SYS_gettid));
	fprintf(((struct args*)data)->info, "Esperando threads\n");
	fprintf(((struct args*)data)->info, "%ld - Semaforo abierto con exito\n", syscall(SYS_gettid));

	for(int k=0; k<amount_ite; k++){
		fprintf(((struct args*)data)->info, "Iniciando iteracion%d\n", k+1);
		/*bloqueara el siguiente hilo*/
		sem_wait(&sem);
		/*Se usa el recurso*/
		recurse +=1;
		fprintf(((struct args*)data)->info, "%ld - (!) Recurso tomado\n", syscall(SYS_gettid));
		fprintf(((struct args*)data)->info, "%ld - Buenos dias! Recurso usado\n", syscall(SYS_gettid));
		/*desbloquea unidad del semaforo*/
		sem_post(&sem);
		/*Simular la devolucion del recurso*/
		sleep(0.7);
		fprintf(((struct args*)data)->info, "%ld - Recurso devuelto :)\n", syscall(SYS_gettid));
		
	}

	/*Salida del thread*/
	
	fprintf(((struct args*)data)->info, "%ld - Semaforo muerto\n", syscall(SYS_gettid));
	fprintf(((struct args*)data)->info, "Terminando thread %ld\n", syscall(SYS_gettid));
	pthread_exit(0);
}
