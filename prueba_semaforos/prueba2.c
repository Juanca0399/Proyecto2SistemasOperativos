#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

sem_t *sem1;
sem_t *sem2;

#define TOTAL 5
#define SEM_NAME1 "/example1"
#define SEM_NAME2 "/example2"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)



int main(){


    sem1 = sem_open(SEM_NAME1, O_RDWR);
    if (sem1 == SEM_FAILED){
        perror("error:");
        exit(EXIT_FAILURE);
    }

    sem2 = sem_open(SEM_NAME2, O_RDWR);
    if (sem2 == SEM_FAILED){
        perror("error:");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    printf("Start\n");

    while(i < TOTAL){
        
        sem_wait(sem2);
        printf("Entrando a zona critica\n");
        sleep(5);
        printf("Hola prueba 2\n");
        printf("Saliendo\n\n");
        sem_post(sem1);
        i++;
    }
   

}