#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

int cantReaders = 0;
int sleepTime = 0;
int readTime = 0;

void* readFromFile(void * arg);

//Para compilar: gcc reader.c -o reader -pthread
int main(int argc, char const *argv[]){

    pthread_t newThread;

    printf("Ingrese la cantidad de readers que desea crear: ");
    scanf("%d\n", &cantReaders);

    printf("Ingrese la cantidad de segundos del sleep: ");
    scanf("%d\n", &sleepTime);

    printf("Ingrese la cantidad de segundos para leer: ");
    scanf("%d\n", &readTime);

    //Crea un hilo por cada reader y lo manda a escribir
    for(int i = 0; i < cantReaders; i++){
        //crear hilo
        pthread_create(&newThread, NULL, readFromFile, NULL);
        pthread_join(newThread, NULL); //se queda en esta linea hasta que el thread retorne null y creo que eso no deberia pasar, creo que se puede eliminar esto sin problema
    }

    return 0;
}

void* readFromFile(void * arg){
    //while(hay un mensaje en la siguiente entrada){
        //readLine
        sleep(readTime);
    //}
    sleep(sleepTime);
}