#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

extern int errno;

int main(){
    int lineas = 0;
    int errnum;

    key_t key = ftok(".", 65);

    printf("Ingrese la cantidad de lineas que desea en la memoria compartida: ");
    scanf("%d", &lineas);

    printf("%d\n", key);

    int idMem = shmget(key, lineas * 20, IPC_CREAT | 0666);

    if(idMem < 0){
        errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error opening file: %s\n", strerror(errnum));
    }

    printf("El id de memoria es: %d\n", idMem);
    //printf("%d\n", idMem);

    return 0;
}