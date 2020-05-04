#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

struct args {FILE* info;};
void *recurso(void* data);

/*Variables de la cantidad de thread*/
#define amount_thread 10
/*Variable de la cantidad de iteraciones*/
#define amount_ite 4

/*recursos disponibles y recursos consumidos*/
int available_resources = 50;
int count = 40;

/*recursos compartido*/
int recurse = 0;

/*Se crea el mutex y la variable condicion*/
pthread_cond_t condition;
pthread_mutex_t mutex;

/*main*/
int main(void){

	/*Creamos y abrimos el archivo txt donde estara la bitacora*/
	FILE *fichero;
	fichero = fopen("Monitor.txt", "wt");

	/*Envio de datos*/
	struct args *archivo = (struct args *)malloc(sizeof(struct args));
	archivo->info = fichero;

	printf("[INFO] Generando archivo BitacoraMonitores...\n"); 

	/*Encabezado de archivo*/
	fprintf(fichero, "Start...\n");
	fprintf(fichero, "Creating threads...\n");
	
	/*Creamos los thread*/
	pthread_t thread[amount_thread];
	
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

/*decrease available resources by count resources */
/*return 0 if sufficient resource available,  */
/*otherwise return -1*/
int decrease_count(int count, void *data){

	fprintf(((struct args*)data)->info, "Recursos suficientes disponibles, consumiendo...\n");
	available_resources -= count;
	/*Simulara el cambio de mutex*/
	sleep(1);
	
	fprintf(((struct args*)data)->info, "Terminando decrease_count\n");
	pthread_mutex_unlock(&mutex);
}

/*increase available resources by count*/
int increase_count(int count, void *data){
	pthread_mutex_lock(&mutex);

	if(available_resources <= count){
		fprintf(((struct args*)data)->info, "Avisando a la mara\n");
		pthread_cond_signal(&condition);
	}

	available_resources += count;
	return 0;
} 

void *recurso(void* data){
	/*imprimimos cada pthread*/
	fprintf(((struct args*)data)->info, "Thread start %ld\n", syscall(SYS_gettid));

	for(int k=0; k<amount_ite; k++){
		fprintf(((struct args*)data)->info, "Start iteracion%d\n", k+1);
		fprintf(((struct args*)data)->info, "Se utilizan %d recursos\n", count);

		/*se bloquea el mutex*/
		pthread_mutex_lock(&mutex);

		/*Se usa el recurso*/
		recurse +=1;
		fprintf(((struct args*)data)->info, "%ld - Recurso obtenido\n", syscall(SYS_gettid));
		fprintf(((struct args*)data)->info, "%ld - Recurso usado\n", syscall(SYS_gettid));

		fprintf(((struct args*)data)->info, "Empezando deacrease\n");
		fprintf(((struct args*)data)->info, "Ingresando al monitor\n");

		decrease_count(count, (void*)((struct args*)data));

		fprintf(((struct args*)data)->info, "Empezando deacrease\n");

		increase_count(count, (void*)((struct args*)data));
		fprintf(((struct args*)data)->info, "Mutex liberado\n");

		/*se usa el siguiente monitor*/
		pthread_mutex_unlock(&mutex);
		fprintf(((struct args*)data)->info, "Terminando increase_count\n");
		
	}

	/*Salida del thread*/
	pthread_exit(0);

}
