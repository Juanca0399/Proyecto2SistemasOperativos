
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 

typedef struct message{
    int pid;
    time_t date;
    int line;
} message;
  
int main() 
{ 
    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,2000,0666|IPC_CREAT);


    void *file = shmat(shmid,NULL,0);
    int numLines = 0;
    while(numLines <= 5){
        message *mssg;
        int i = 0;
        while(i < numLines){
            mssg = file+(i*sizeof(message));
            i++;
        }
        printf("Id: %d\n", mssg->line);
        numLines++;
    }

    // shmat to attach to shared memory 
    //char *str = (char*) shmat(shmid,NULL,0); 
  
      
    //detach from shared memory  
    shmdt(file); 
    
    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 
     
    return 0; 
} 
