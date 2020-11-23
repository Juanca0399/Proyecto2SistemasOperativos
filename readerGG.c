
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 

typedef struct message{
    int pid;
    time_t fechaHora;
    int linea;
} message;
  
int main() 
{ 
    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,2000,0666|IPC_CREAT);


    void *archivo = shmat(shmid,NULL,0);
    int cant_lineas = 0;
    while(cant_lineas <= 5){
        message *pMensaje;
        int i = 0;
        while(i < cant_lineas){
            pMensaje = archivo+(i*sizeof(message));
            i++;
        }
        printf("Id: %d\n", pMensaje->linea);
        cant_lineas++;
    }

    // shmat to attach to shared memory 
    //char *str = (char*) shmat(shmid,NULL,0); 
  
      
    //detach from shared memory  
    shmdt(archivo); 
    
    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 
     
    return 0; 
} 
