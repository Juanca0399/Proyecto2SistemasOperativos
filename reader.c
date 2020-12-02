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

#define SEM_NAME "/semaphore"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

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
    int currentId;
    int currentType; //0: ninguno, 1:writer, 2:reader 3:egoista
    int writers; //Cuantos hay de cada tipo
    int readers;
    int egoistas;
} info;

int cantReaders = 0;
int sleepTime = 0;
int readTime = 0;
bool run = true;
int idThread = 0;

pthread_t commands;
pthread_t newThread;
sem_t *sem;
key_t key;
key_t infoKey;
int shmid;
int shmidInfo;
void *file;
void *inf;

FILE * bitacora;

void* readFromFile(void * arg);
void* listenForCommands(void * arg);

//Para compilar: gcc reader.c -o reader -lpthread -lrt
int main(int argc, char const *argv[]){

    // Setup de semaforos
    //sem_unlink(SEM_NAME);
    

    sem = sem_open(SEM_NAME, O_RDWR);
    if (sem == SEM_FAILED){
        perror("error:");
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
    sharedInfo->readers = cantReaders;

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
    int numThread = idThread;
    idThread++;
    int i = 0;
    info *sharedInfo = inf;
    while(run){
        sem_wait(sem);
        if(sharedInfo->written > 0){
            printf("\nEntra zona critica\n");
            
            sharedInfo->currentId = numThread;
            sharedInfo->currentType = 2;
            
            void *file = shmat(shmid,NULL,0); //attach
            message *mssg = file;
            sharedInfo->turnEgoista = 0;
            mssg = file + (i*sizeof(message));

            while(!mssg->isUsed){
                i = (i + 1) % sharedInfo->lineas;

                mssg =  file + (i*sizeof(message));
            }
            bitacora = fopen ("./bitacora.txt","a");

            printf("Id: %d\n", mssg->line);
            printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
            
            i = (i + 1) % sharedInfo->lineas;

            fprintf(bitacora, "%s", "El reader: ");
            fprintf(bitacora, "%d", numThread);
            fprintf(bitacora, "%s", "\n");
            fprintf(bitacora, "%s", "Leyo:\n ");
            fprintf(bitacora, "%d", mssg->line);
            fprintf(bitacora, "%s", "\n");
            fprintf(bitacora, "%s", asctime(gmtime(&mssg->date)));
            fprintf(bitacora, "%s", "\n");
            fprintf(bitacora, "%s", "==========|==========");
            fprintf(bitacora, "%s", "\n");
            fclose(bitacora);

            shmdt(file); //detach
            sleep(readTime);
            printf("\nSaliendo zona critica...\n"); 
        }
        sharedInfo->currentId = -1;
        sharedInfo->currentType = 0;
        //schleep
        sem_post(sem);

        
        sleep(sleepTime);
    }
    sem_close(sem);
    return NULL;
}