#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){

    int valor = shmctl (65595, IPC_RMID, 0); //0 si la llave existe, -1 si no

    printf("%d\n", valor); //como estoy eliminando, de ahora en adelante la llave da -1

    return 0;
}