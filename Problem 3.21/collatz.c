#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    // If there are too many command arguments, exit...
    if (argc != 2) {
        printf("main()::Error collecting number!");
        return 1;
    }

    // Collect the integer
    int number = atoi(argv[1]);

    // If the integer is 0 or negative, exit...
    if (number <= 0) {
        printf("Please input a positive, non-zero integer.\n");
        return 1;
    }

    // Echo the inputted number back
    printf("Given number: %d\n", number);

    pid_t pid = fork();

    if (pid < 0) { // If there was an error calling fork(), exit...
        printf("main()::Error calling fork()");
    } else if (pid > 0) { // Inside the parent process...
        // Call wait() for the parent
        wait(NULL);
    } else if (pid == 0) { // Inside the child process...
        // Initial print of the inputted number
        printf("%d, ", number);
        while (number != 1) { // While the number isn't 1...
            if (number % 2 == 0) { // If even, divide by 2
                number /= 2;
            } else { // If odd, multiply by 3 and add 1
                number = 3 * number + 1;
            }
            // Print number
            if (number == 1) {
                printf("%d\n", number);
            } else {
                printf("%d, ", number);
            }
        }
    }

    return 0;
}