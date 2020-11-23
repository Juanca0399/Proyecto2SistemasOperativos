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
    int pid;
    time_t date;
    int line;
} message;

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
int shmid;
void *file;
int numLines = 0;

void* writeToFile(void * arg);
void* listenForCommands(void * arg);

//Para compilar: gcc writer.c -o writer -lpthread -lrt
int main(int argc, char const *argv[]){

    sem_init(&mutex, 0, 1);

    // ftok to generate unique key 
    key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    shmid = shmget(key,2000,0666|IPC_CREAT);


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

    pthread_join(newThread, NULL); //Espera a que el ultimo thread que cree termine
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
    while(run){
        if(!isFileFull){
            //lock
            sem_wait(&mutex); 
            printf("\nEntra zona critica\n");

            //write
            void *file = shmat(shmid,NULL,0); //attach
            
            message *mssg;
            int i = 0;
            while(i < numLines){
                mssg = file+(i*sizeof(message));
                i++;
            }
            printf("Thread num %d\n", numThread);
            printf("i: %d\n",i);
            
            mssg->line = i;
            mssg->date = time(NULL);
            printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
            numLines++;
            
            shmdt(file); //detach


            sleep(writeTime);
            
            //unlock
            printf("\nSaliendo zona critica...\n"); 
            sem_post(&mutex); 
            //contadorZonaCritica++;

            sleep(sleepTime);
        }
    }

    return NULL;
}