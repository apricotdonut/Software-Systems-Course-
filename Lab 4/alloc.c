// implements a resource allocator program

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/mman.h>

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

int main(void){
	int  fd, minoffset, maxoffset, offset, input, output;
	char *addr, inputc, outputc;
	struct stat sb;
	sem_init();

	//opens res.txt
    	fd = open("res.txt", O_RDWR);

    	if (fd == -1){
    	    printf("File not found. Please try again\n");
    	    exit (1);
   	}

	//get the size of the file
	//use the value to specify size of mapping
	if (fstat(fd, &sb)== -1){
	printf("Failed to get size of file.");
	exit(1);
	}

	if (sb.st_size == 0){
	printf("File size is 0.");
	exit(1);
	}


	//maps file to a memory region using mmap()
	addr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED){
	printf("Failed to map.");
	exit(1);
	}
	//close the file
	if (close(fd) == -1){
	exit(1);
	}
	printf("Allocated %ld (%#lx) bytes starting at %p\n", (long) sb.st_size, (unsigned long)sb.st_size, addr);


	//perform operation
	//ask how many units of a resource type is needed
	//subtracts the units from that resource type
	minoffset = 0;
	maxoffset = sb.st_size -1;

	
	while(1){
		
		semop(semid, stop, 1);

		for (offset = 0; offset <= maxoffset; offset = offset +4){
			printf("\nPlease input an integer less than 10.\n");
			printf("How many units of resource type %c is needed?\n", addr[offset]);
			scanf("%d",&input);

			inputc = input + '0';

			if (inputc <= addr[offset+2]){
			
				output = addr[offset+2]-inputc;
				outputc = output + '0';
						addr[offset+2] = outputc;
					if (msync(addr, sb.st_size, MS_SYNC) == -1){
						printf("Failed to sync.");
						exit(0);
					}
					printf("resource allocated.\n");
			}
			else printf("not enough resource available.\n");
			semop(semid, start, 1);

	
		}

	}
}
	

