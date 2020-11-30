#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

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

bool isFileFull = false;
bool run = true;
int cantWriters = 0;
int sleepTime = 0;
int writeTime = 0;
sem_t mutex;
pthread_t newThread;
pthread_t commands;
int contadorZonaCritica = 0;

key_t key;
key_t infoKey;
int shmid;
int shmidInfo;
void *file;
void *inf;
int numLines = 0;
int processNumber = 0;


void* writeToFile(void * arg);
void* listenForCommands(void * arg);

//Para compilar: gcc writer.c -o writer -lpthread -lrt
int main(int argc, char const *argv[]){

    sem_init(&mutex, 0, 1);

    // ftok to generate unique key 
    key = ftok(".",65);
    infoKey = ftok(".", 66);
  
    // shmget returns an identifier in shmid 
    shmidInfo = shmget(infoKey, 100 * sizeof(info), 0);

    //attach
    void *infoVoid = shmat(shmidInfo,NULL,0);
    inf = infoVoid;
    info *sharedInfo = infoVoid;
    // shmget returns an identifier in shmid 
    shmid = shmget(key,sharedInfo->lineas * sizeof(message),0666|IPC_CREAT);


    printf("Ingrese la cantidad de writers que desea crear: ");
    scanf("%d", &cantWriters);

    int numsThreads[cantWriters];

    printf("Ingrese la cantidad de segundos del sleep: ");
    scanf("%d", &sleepTime);

    printf("Ingrese la cantidad de segundos para escribir: ");
    scanf("%d", &writeTime);

    //Crea un hilo por cada writer y lo manda a escribir
    for(int i = 0; i < cantWriters; i++){
        numsThreads[i] = i;
        //crear hilo
        pthread_create(&newThread, NULL, writeToFile, (void *)numsThreads[i]);
        printf("Se creo hilo %d\n", i);
        //sleep(2);
    }
    pthread_create(&commands, NULL, listenForCommands, NULL);
    pthread_join(commands, NULL); //Espera hasta que listenForCommands retorne algo para seguir
int turnEgoista;
    pthread_join(newThread, NULL); //Espera a que el ultimo thread que cree termine
    shmdt(infoVoid); //detach
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

void* writeToFile(void * arg){
// while true podria ir aqui, los semaforos tienen que checkear si hay otro write o read en el file (semaforo entre archivos)
/*
banderas:
bandera de run
cuando writer escribe, saca a todos del archivo
cuando reader egoista escribe, saca a todos del archivo
los readers sí dejan que otros estén el archivo
*/
    int numThread = (int)arg;
    info *sharedInfo = inf;
    sharedInfo->turnEgoista = 0;
    while(run){
        //lock
        sem_wait(&mutex); 
        if(sharedInfo->written < sharedInfo->lineas){
            
            
            printf("\nEntra zona critica\n");
            //write
            void *file = shmat(shmid,NULL,0); //attach
            
            message *mssg = file;
            int i = 0;

            while(mssg->isUsed && i < sharedInfo->lineas - 1){ //maybe check
                i++;
                mssg = file + (i*sizeof(message));
                
            }
            printf("Thread num %d\n", numThread);
            printf("i: %d\n",i);
            
            mssg->line = i;
            mssg->date = time(NULL);
            printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
            mssg->isUsed = 1;
            mssg->pid = numThread;
            sharedInfo->written = sharedInfo->written + 1;

            //detach
            shmdt(file);

            //exit
            printf("\nSaliendo zona critica...\n"); 

            //sleep
            sleep(writeTime);
            

        } 
        sem_post(&mutex);

        //schleep again
        sleep(sleepTime);
    }

    return NULL;
}