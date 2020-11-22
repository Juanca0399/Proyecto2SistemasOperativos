#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h> 

bool isFileFull = false;
int cantWriters = 0;
int sleepTime = 0;
int writeTime = 0;
sem_t mutex;

void* writeToFile(void * arg);

//Para compilar: gcc writer.c -o writer -lpthread -lrt
int main(int argc, char const *argv[]){

    pthread_t newThread;

    sem_init(&mutex, 0, 1); 

    printf("Ingrese la cantidad de writers que desea crear: ");
    scanf("%d", &cantWriters);

    printf("Ingrese la cantidad de segundos del sleep: ");
    scanf("%d", &sleepTime);

    printf("Ingrese la cantidad de segundos para escribir: ");
    scanf("%d", &writeTime);

    //Crea un hilo por cada writer y lo manda a escribir
    for(int i = 0; i < cantWriters; i++){
        //crear hilo
        pthread_create(&newThread, NULL, writeToFile, NULL);
        printf("Se creo hilo %d\n", i);
        //pthread_join(newThread, NULL); //se queda en esta linea hasta que el thread retorne null y creo que eso no deberia pasar, creo que se puede eliminar esto sin problema
    }
    pthread_join(newThread, NULL);
    sem_destroy(&mutex); 
    return 0;
}

void* writeToFile(void * arg){

    if(!isFileFull){
        //lock
        sem_wait(&mutex); 
        printf("\nEntra zona critica\n");

        //write
        printf("HOLA\n");

        sleep(writeTime);
        
        //unlock
        printf("\nSaliendo zona critica...\n"); 
        sem_post(&mutex); 

        sleep(sleepTime);
    }

    return NULL;
}