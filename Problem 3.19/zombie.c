#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Call fork(), storing pid for each process
    pid_t pid = fork();

    if (pid < 0) {
        printf("main()::Could not call fork()\n");
        return -1;
    } else if (pid == 0) { // Inside the child process...
        // Kill the child
        exit(0);
    } else if (pid > 0) { // Inside the original process...
        // Sleep program for 10 seconds
        printf("Waiting to call 'ps -l' for 10 seconds...");
        sleep(10);

        // Call command to list processes to terminal
        system("ps -l");
        
        // Reap the child
        pid = wait(NULL);
    }
    
    return 0;
}