#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){
    int lineas = 0;

    key_t key = ftok(".", 65);

    printf("Ingrese la cantidad de lineas que desea en la memoria compartida: ");
    scanf("%d", &lineas);

    int idMem = shmget(key, lineas, 0666|IPC_CREAT);

    printf("%d", idMem);
    //printf("%d\n", idMem);

    return 0;
}