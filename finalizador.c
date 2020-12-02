#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdbool.h>
#include <semaphore.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
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

int main(){
    sem_t *sem = sem_open(SEM_NAME, O_RDWR);
    key_t key = ftok(".",65);
    key_t infoKey = ftok(".", 66);

    int shmidInfo = shmget(infoKey, 100 * sizeof(info), 0);

    //attach
    void *infoVoid = shmat(shmidInfo,NULL,0);
    info *sharedInfo = infoVoid;
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,sharedInfo->lineas * sizeof(message),0666|IPC_CREAT);

    int deletedMem = shmctl (shmid, IPC_RMID, 0); //0 si la llave existe, -1 si no
    int deleteInfo = shmctl(shmidInfo, IPC_RMID, 0);

    printf("%d\n", deletedMem); //como estoy eliminando, de ahora en adelante la llave da -1
    printf("%s","\n");
    printf("%d\n", deleteInfo);

    sem_destroy(sem);

    return 0;
}