#include <stdio.h> 
#include <signal.h> 
#include <unistd.h>
#include <stdlib.h>


// user-defined signal handler for alarm. 
void alarm_handler(int signo){
    if (signo == SIGALRM){ 
        printf("Alarm\n");
    }
    else if (signo == SIGINT){
        printf("  CTRL+C pressed!\n");
    }
    else if (signo == SIGTSTP){
        printf("  CTRL+Z pressed!\n");
        exit(1);
    }
}

int main(void){
// register the signal handler
    if (signal(SIGALRM , alarm_handler) == SIG_ERR){
        printf("failed to register alarm handler.");
        exit(1); 
    }
    signal(SIGINT, alarm_handler);
    signal(SIGTSTP, alarm_handler);
  
     // set alarm to fire in 2 seconds.
    while(1){
        sleep(2); 
        alarm(2); 
        
    } // wait until alarm goes off 
}