#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){

    int valor = shmctl (32780, IPC_RMID, 0);

    printf("%d\n", valor);

    return 0;
}