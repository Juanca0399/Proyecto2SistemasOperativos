#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

bool isFileFull = false;
int cantWriters = 0;
int sleepTime = 0;
int writeTime = 0;

void* writeToFile(void * arg);

//Para compilar: gcc writer.c -o writer -pthread
int main(int argc, char const *argv[]){

    pthread_t newThread;

    printf("Ingrese la cantidad de writers que desea crear: ");
    scanf("%d\n", &cantWriters);

    printf("Ingrese la cantidad de segundos del sleep: ");
    scanf("%d\n", &sleepTime);

    printf("Ingrese la cantidad de segundos para escribir: ");
    scanf("%d\n", &writeTime);

    //Crea un hilo por cada writer y lo manda a escribir
    for(int i = 0; i < cantWriters; i++){
        //crear hilo
        pthread_create(&newThread, NULL, writeToFile, NULL);
        pthread_join(newThread, NULL); //se queda en esta linea hasta que el thread retorne null y creo que eso no deberia pasar, creo que se puede eliminar esto sin problema
    }

    return 0;
}

void* writeToFile(void * arg){

    if(!isFileFull){
        //lock
        //write
        sleep(writeTime);
        //unlock
        sleep(sleepTime);
    }

    return NULL;
}