#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){
    int lineas;

    key_t key = ftok(".",'S');

    printf("Ingrese la cantidad de lineas que desea en la memoria compartida: ");
    scanf("%d", &lineas);

    int idMem = shmget(key, lineas, IPC_CREAT);

    printf("%d", idMem);
    //printf("%d\n", idMem);

    return 0;
}