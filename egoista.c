#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <semaphore.h>

typedef struct message{
    int pid;
    time_t date;
    int line;
} message;

int cantReaders = 0;
int sleepTime = 0;
int readTime = 0;
bool run = true;

pthread_t commands;
pthread_t newThread;
sem_t mutex;
key_t key;
int shmid;
void *file;

void* readFromFile(void * arg);
void* listenForCommands(void * arg);

//Para compilar: gcc egoista.c -o egoista -lpthread -lrt
int main(int argc, char const *argv[]){

    sem_init(&mutex, 0, 1);

    // ftok to generate unique key 
    key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    shmid = shmget(key,2000,0);//cambiar el 2000 por lineas*sizeof(message)

    printf("Ingrese la cantidad de readers que desea crear: ");
    scanf("%d", &cantReaders);

    printf("Ingrese la cantidad de segundos del sleep: ");
    scanf("%d", &sleepTime);

    printf("Ingrese la cantidad de segundos para leer: ");
    scanf("%d", &readTime);

    //Crea un hilo por cada reader y lo manda a escribir
    for(int i = 0; i < cantReaders; i++){
        //crear hilo
        pthread_create(&newThread, NULL, readFromFile, NULL);
        //pthread_join(newThread, NULL); //se queda en esta linea hasta que el thread retorne null y creo que eso no deberia pasar, creo que se puede eliminar esto sin problema
    }

    pthread_create(&commands, NULL, listenForCommands, NULL);
    pthread_join(commands, NULL); //Espera hasta que listenForCommands retorne algo para seguir

    pthread_join(newThread, NULL);

    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 
    sem_destroy(&mutex); //destruye el semaforo
    return 0;
}

void* listenForCommands(void * arg){
    char command[50] = {0};
    while(run){
        printf("Escriba 'fin' para terminar la ejecucion: \n");
        scanf("%s", command);
        if(!strcmp(command,"fin")){
            run = false;
            printf("Saliendo del programa...\n");
        } else{
            printf("Comando no reconocido, intente de nuevo\n");
        }
    }
    
    return NULL;
}

void* readFromFile(void * arg){
    int numLines = 0;

    while(run){
        
        //while(hay un mensaje en la siguiente entrada){
            //read
            
            //lock
            sem_wait(&mutex);
            printf("\nEntra zona critica\n");

            void *file = shmat(shmid,NULL,0); //attach
            
            if(numLines <= 5){ //cambiar este 5 por el total de lineas
                message *mssg = file;
                int i = 0;
                while(i <= numLines){
                    mssg = file+(i*sizeof(message));
                    i++;
                }
                printf("Id: %d\n", mssg->line);
                printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
                numLines++;
            }
            shmdt(file); //detach

            sleep(readTime);
             //unlock
            printf("\nSaliendo zona critica...\n"); 
            sem_post(&mutex); 
        //}
        sleep(sleepTime);
    }
    return NULL;
}