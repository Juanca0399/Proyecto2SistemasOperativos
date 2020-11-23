
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
  
    // shmat to attach to shared memory 

    //char *str = (char*) shmat(shmid,NULL,0); 
    int numLines = 0;
    void *file = shmat(shmid,NULL,0);
    while(numLines <= 5){
        message *mssg;
        int i = 0;
        while(i < numLines){
            mssg = file+(i*sizeof(message));
            i++;
        }
        printf("i: %d\n",i);
        mssg->line = i;
        numLines++;
    }
  
    //printf("Write data: \n");

    //scanf("%s",str);
  
    //printf("Data written in memory: %s\n",str); 
      
    //detach from shared memory  
    shmdt(file); 
  
    return 0; 
} 