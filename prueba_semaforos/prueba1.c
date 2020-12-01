#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

#define TOTAL 5
#define SEM_NAME1 "/example1"
#define SEM_NAME2 "/example2"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)



int main(){

    sem_unlink(SEM_NAME1);
    sem_unlink(SEM_NAME2);


    sem_t *sem1 = sem_open(SEM_NAME1, O_CREAT | O_EXCL, SEM_PERMS, 1);
    if (sem1 == SEM_FAILED){
        perror("error:");
        exit(EXIT_FAILURE);
    }

    sem_t *sem2 = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, 0);
    if (sem2 == SEM_FAILED){
        perror("error:");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    printf("Start\n");

    while(i < TOTAL){
        sem_wait(sem1);
        printf("Entrando a zona critica\n");
        sleep(5);
        printf("Hola prueba 1\n");
        printf("Saliendo\n\n");
        sem_post(sem2);
        i++;
    }

    sem_close(sem1);
    sem_close(sem2);
   

}