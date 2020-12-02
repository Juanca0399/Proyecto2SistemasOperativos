#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    int currentId;
    int currentType; //0: ninguno, 1:writer, 2:reader 3:egoista
    int writers; //Cuantos hay de cada tipo
    int readers;
    int egoistas;
} info;

key_t key;
key_t infoKey;
int shmid;
int shmidInfo;
void *file;
void *inf;
sem_t *sem;

void printMem();
void printProcesses();

//Para compilar: gcc espia.c -o espia
int main(int argc, char const *argv[]){

    //sem_unlink("/sem");

    //sem = sem_open("/sem", O_CREAT, 0660, 0);
    //if (sem == SEM_FAILED){
        //perror("sem_open/sem");
        //exit(EXIT_FAILURE);
    //}

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

    printMem();
    printProcesses();

    //Prepare to end program:
    // destroy the shared memory 
    //shmctl(shmid,IPC_RMID,NULL);
    shmdt(infoVoid); //detach
    //shmctl(shmidInfo,IPC_RMID,NULL);  
    //sem_destroy(&mutex); //destruye el semaforo
    return 0;
}

void printProcesses(){
    info *sharedInfo = inf;
    printf("Procesos actuales------------------------------------------\n");
    printf("Tipo de proceso corriendo:"); 
    if(sharedInfo->currentType == 0){ //Ninguno esta corriendo
        printf("Ninguno\n");
    } else if(sharedInfo->currentType == 1){
        printf("Writer\n");
    } else if(sharedInfo->currentType == 2){
        printf("Reader\n");
    } else if(sharedInfo->currentType == 3){
        printf("Egoista\n");
    }
    printf("Id de proceso corriendo: %d\n\n", sharedInfo->currentId);

    if(sharedInfo->currentType == 0){ //Ninguno esta corriendo
        printf("Cantidad writers en espera: %d\nCantidad readers en espera: %d\nCantidad egoistas en espera: %d\n",
        sharedInfo->writers,
        sharedInfo->readers,
        sharedInfo->egoistas);
        printf("Ids writers: ");
        for(int i = 0; i < sharedInfo->writers; i++){
            printf("%d, ", i);
        }
        printf("\n");
        printf("Ids readers: ");
        for(int i = 0; i < sharedInfo->readers; i++){
            printf("%d, ", i);
        }
        printf("\n");
        printf("Ids egoistas: ");
        for(int i = 0; i < sharedInfo->egoistas; i++){
            printf("%d, ", i);
        }
        printf("\n");
    } else if(sharedInfo->currentType == 1){
        printf("Cantidad writers en espera: %d\nCantidad readers en espera: %d\nCantidad egoistas en espera: %d\n",
        sharedInfo->writers - 1,
        sharedInfo->readers,
        sharedInfo->egoistas);

        printf("Ids writers: ");
        for(int i = 0; i < sharedInfo->writers; i++){
            if(sharedInfo->currentId != i){
                printf("%d, ", i);
            }
        }
        printf("\n");
        printf("Ids readers: ");
        for(int i = 0; i < sharedInfo->readers; i++){
            printf("%d, ", i);
        }
        printf("\n");
        printf("Ids egoistas: ");
        for(int i = 0; i < sharedInfo->egoistas; i++){
            printf("%d, ", i);
        }
        printf("\n");

    } else if(sharedInfo->currentType == 2){
        printf("Cantidad writers en espera: %d\nCantidad readers en espera: %d\nCantidad egoistas en espera: %d\n",
        sharedInfo->writers,
        sharedInfo->readers - 1,
        sharedInfo->egoistas);

        printf("Ids writers: ");
        for(int i = 0; i < sharedInfo->writers; i++){
            printf("%d, ", i);
        }
        printf("\n");
        printf("Ids readers: ");
        for(int i = 0; i < sharedInfo->readers; i++){
            if(sharedInfo->currentId != i){
                printf("%d, ", i);
            }
        }
        printf("\n");
        printf("Ids egoistas: ");
        for(int i = 0; i < sharedInfo->egoistas; i++){
            printf("%d, ", i);
        }
        printf("\n");

    } else if(sharedInfo->currentType == 3){
        printf("Cantidad writers en espera: %d\nCantidad readers en espera: %d\nCantidad egoistas en espera: %d\n",
        sharedInfo->writers,
        sharedInfo->readers,
        sharedInfo->egoistas - 1);

        printf("Ids writers: ");
        for(int i = 0; i < sharedInfo->writers; i++){
            printf("%d, ", i);
        }
        printf("\n");
        printf("Ids readers: ");
        for(int i = 0; i < sharedInfo->readers; i++){
            printf("%d, ", i);  
        }
        printf("\n");
        printf("Ids egoistas: ");
        for(int i = 0; i < sharedInfo->egoistas; i++){
            if(sharedInfo->currentId != i){
                printf("%d, ", i);
            }
        }
        printf("\n");

    }


}

void printMem(){
    int i = 0;
    info *sharedInfo = inf;
    printf("Memoria----------------------------------------------------\n");
    
    printf("Capacidad de lineas: %d\n", sharedInfo->lineas);
    printf("Lineas ocupadas: %d\n", sharedInfo->written);

    printf("LINEAS:\n");   

    while(i < sharedInfo-> lineas){
        void *file = shmat(shmid,NULL,0); //attach
        message *mssg = file;
        mssg = file + (i*sizeof(message));
        printf("Linea #%d\n", i+1);
        if(mssg->isUsed){
            printf("Id: %d\n", mssg->line);
            printf("Fecha: %s\n", asctime(gmtime(&mssg->date)));
        } else{
            printf("Linea vacia\n");
        }
        shmdt(file); //detach

        i++;
    }


    printf("-------------------------------------------------------------\n");
}
