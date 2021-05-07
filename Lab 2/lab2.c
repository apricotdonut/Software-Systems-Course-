/*
Q1
ps -e shows all the running processes'
ps -l shows threads of a process

Q2 
$ ps ax | grep firefox
$ kill <PID>

Q3 2 times
Q4 pstree shows running processes as a tree
*/
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>     
#include <string.h>
#include <sys/types.h>

int main(void){
    //fd is pipe for child to write and parent to read
    //FD is pipe for parent to write and child to read
    int fd[2], FD[2], sum = 0, num, final;
    pid_t childpid;
    char integers[80];
    char readbuffer [80];
    pipe(fd);
    pipe(FD);

    if((childpid = fork()) == -1){ 
        perror("fork");
        exit(0);
    }
    if(childpid == 0){ //child process
        close(fd[0]); 
        close(FD[1]);
        
        int i = 0;
        int ch;
        printf("input integers:\n");
        while(ch != -1){
            scanf("%d",&ch);
            integers[i] = ch;
            i ++; 
        };
        write(fd[1], integers, sizeof(integers));
        read(FD[0], &final, sizeof(final));
        printf("\nSum of all integers is %d", final);
        exit(0);

    } 
    else{ //parent process
        close(fd[1]); 
        close(FD[0]); 
        
        read(fd[0], readbuffer, sizeof(readbuffer));
        for (int i = 0; i < strlen(readbuffer)-1; i++){
            num = (int)(readbuffer[i]);
            sum += num;
        };
        
        write(FD[1], &sum, sizeof(sum));

    }
    return 0;

}

