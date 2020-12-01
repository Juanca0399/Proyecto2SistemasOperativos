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
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

typedef struct message{
    bool isUsed;
    int pid;
    time_t date;
    int line;
} message;

typedef struct info{
    int lineas;
    int written;
    int turnEgoista;
} info;

int cantReaders = 0;
int sleepTime = 0;
int readTime = 0;
bool run = true;

pthread_t commands;
pthread_t newThread;
sem_t *sem;
key_t key;
key_t infoKey;
int shmid;
int shmidInfo;
void *file;
void *inf;

void* readFromFile(void * arg);
void* listenForCommands(void * arg);

//Para compilar: gcc reader.c -o reader -lpthread -lrt
int main(int argc, char const *argv[]){

    sem_unlink("/sem");

    sem = sem_open("/sem", O_CREAT, 0660, 0);
    if (sem == SEM_FAILED){
        perror("sem_open/sem");
        exit(EXIT_FAILURE);
    }

    //sem_init(&mutex, 0, 1);

    // ftok to generate unique key 
    key = ftok(".",65);
    infoKey = ftok(".", 66);
  
    // shmget returns an identifier in shmid 
    shmidInfo = shmget(infoKey, 100 * sizeof(info), 0);

    //attach
    void *infoVoid = shmat(shmidInfo,NULL,0);
    inf = infoVoid;
    info *sharedInfo = infoVoid;

    shmid = shmget(key,sharedInfo->lineas * sizeof(message),0);

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
    shmdt(infoVoid); //detach
    shmctl(shmidInfo,IPC_RMID,NULL);  
    //sem_destroy(&mutex); //destruye el semaforo
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
    int i = 0;
    info *sharedInfo = inf;
    while(run){
        sem_wait(sem);
        if(sharedInfo->written > 0){
            void *file = shmat(shmid,NULL,0); //attach
            message *mssg = file;
            sharedInfo->turnEgoista = 0;
            mssg = file + (i*sizeof(message));

            while(!mssg->isUsed){
                i = (i + 1) % sharedInfo->lineas;

                mssg =  file + (i*sizeof(message));
            }

            printf("Id: %d\n", mssg->line);
            printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
            
            i = (i + 1) % sharedInfo->lineas;
            shmdt(file); //detach

            //schleep
            sleep(readTime);
            sem_post(sem);
            sleep(sleepTime);
        }
    }
    sem_close(sem);
    return NULL;
}