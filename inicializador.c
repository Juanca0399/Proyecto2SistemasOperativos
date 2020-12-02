#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

extern int errno;

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
    FILE * fp;
    fp = fopen("./bitacora.txt","w");
    fclose(fp);

    int lineas = 0;
    int errnum;

    key_t key = ftok(".", 65);

    key_t infoKey = ftok(".", 66);


    printf("Ingrese la cantidad de lineas que desea en la memoria compartida: ");
    scanf("%d", &lineas);

    printf("%d\n", key);

    int idMem = shmget(key, lineas * sizeof(message), IPC_CREAT | 0666);

    int idInfo = shmget(infoKey, 100 * sizeof(info), IPC_CREAT | 0666);

    if(idMem < 0){
        errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error opening file: %s\n", strerror(errnum));
    }

    if(idInfo < 0){
        errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error creating 'info' memory: %s\n", strerror(errnum));
    }

    void *infoVoid = shmat(idInfo,NULL,0);
    info *sharedInfo = infoVoid;
    sharedInfo->lineas = lineas;
    sharedInfo->written = 0;
    sharedInfo->turnEgoista = 0;
    sharedInfo-> currentId = -1;
    sharedInfo-> currentType = 0;
    sharedInfo->writers = 0;
    sharedInfo->readers = 0;
    sharedInfo->egoistas = 0;

    void *memAttach = shmat(idMem,NULL,0);
    message *memory = memAttach;

    int lineasEscritas = 0;
    for(int i = 0; i < lineas; i++){
        
        int j = 0;
        while(j <= lineasEscritas){
            memory= memAttach+(j*sizeof(message));
            j++;
        }
        memory->isUsed = 0;
        lineasEscritas++;
    }

    shmdt(memAttach);
    shmdt(infoVoid);


    printf("El id de memoria es: %d\n", idMem);
    printf("El id de info es: %d\n", idInfo);
    //printf("%d\n", idMem);

    return 0;
}