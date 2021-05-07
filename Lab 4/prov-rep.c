#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <assert.h>

key_t key = 1;
int semid;
struct sembuf start[1];
struct sembuf stop[1];

void sem_init(void){
	semid = semget(key, 1, 0666| IPC_CREAT);
	semctl(semid, 0, SETVAL, 1);
	
	start[0].sem_num = 0;
	start[0].sem_op = 1;
	start[0].sem_flg = 0;
	
	stop[0].sem_num = 0;
	stop[0].sem_op = -1;
	stop[0].sem_flg = 0;
};

void parent(char* addr, int size){
	int newresource, type, unit, output, maxoffset, offset;

    	
    	maxoffset = size - 1; 
	while(1){
		//asks whether new resources need to be added
		printf("\n [Provider]\n");
		printf("Enter 1 to add new resources, anything else to not\n");
		scanf("%d", &newresource);
		semop(semid, stop, 1);
		
		//yes, receive input: resource type and #of units
		if (newresource == 1){
			
			printf("\nEnter type of resource.\n");
			scanf("%d", &type);

			
			printf("Enter number of resource to be added.\n");
			scanf("%d", &unit);

			
			for (offset = 0; offset <= maxoffset; offset = offset +4){
	   			if (type +'0' == addr[offset]){
	   				printf("resource added.\n");
	   				output = addr[offset+2] + unit - 48;
	   				if (output > 9){
	   					printf("Max quantity of 9 reached\n");
	   					addr[offset+2] = '9';
	   					if (msync(addr, size, MS_SYNC) == -1){
	    						printf("Failed to sync.\n");
	    						exit(1);
	    					}
	   					break;
	   				}
	   				else{
	   					addr[offset+2] = output + '0';
	   					if (msync(addr, size, MS_SYNC) == -1){
	    						printf("Failed to sync.\n");
	    						exit(1);
	    					}
	   					;
					
					}
				}
			}	
		
		}
		else {
			printf("You don't want to add any resources.\n");
		}
		semop(semid, start, 1);
		  	
	}
    	    
};

void child(char* addr, int size){
	
	while (1){
		sleep(10);
		semop(semid, stop, 1);
		
		printf("\n [Reporter]\n");
		//1. pagesize
		int pagesize = getpagesize();
		printf("Page size: %d\n", pagesize);
		
		//2. state of resources
		printf("Type & Number of Unit(s)\n");
		for (int offset = 0; offset < size; offset = offset + 4)
			printf("%c %c\n", addr[offset], addr[offset+2]);
		
		//3. status of page
		int veclen = (pagesize + size -1)/pagesize;
		char vec[veclen];
		mincore(addr, size, vec);
		//printf("Status of page\n");
		//printf("Mincore value: %d\n\n",vec[sizeof(vec)-1]);	
		int total = 0;	
		for(int i = 0; i < veclen; ++i)
        {
                if(vec[i])
                {
                        total++;
                }
        }
		printf("Minicore:%d\n", total);
		semop(semid, start, 1);
		
	}
};

int main(void){
	int  fd;
	char *addr;
	struct stat sb;
	pid_t childpid; 
	
	/*parent (provider of resources)*/
	//opens res.txt
    fd = open("res.txt", O_RDWR);
    if (fd == -1){
    	printf("File not found. Please try again\n");
    	exit (1);
	}
	//get the size of the file
	//use the value later to specify size of mapping
	if (fstat(fd, &sb)== -1){
		printf("Failed to get size of file.\n");
		exit(1);
	}
	if (sb.st_size == 0){
		printf("File size is 0.\n");
		exit(1);
	}


	//maps file to a memory region using mmap()
	addr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED){
		printf("Failed to map.\n");
		exit(1);
	}
	printf("Allocated %ld (%#lx) bytes starting at %p\n", (long) sb.st_size, (unsigned long)sb.st_size, addr);
    	
    	//close the file
    	if (close(fd) == -1) exit(1);
    	
    	//create semaphore
	sem_init();
    	
    	int size = sb.st_size;
    	
    	//fork child process
    	if ((childpid = fork()) == -1){
    		perror("fork");
    		exit(1);
    	}
    	
    	while (1){
    		if(childpid == 0)/* child proces*/ child(addr, size);
    
    		else/*parent process*/ parent(addr, size);	
    	}
    	    
 }   	
    	    


	
