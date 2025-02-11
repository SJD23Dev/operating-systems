#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SHM_NAME "/collatz_shm"
#define MEM_SIZE 4096

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

    // Fork the process
    pid_t pid = fork();

    if (pid < 0) {
        printf("main()::Error calling fork()");
    } else if (pid == 0) { // Inside the child process...
        // Create shared memory object
        int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

        // If it failed to create, exit...
        if (shm_fd == -1) {
            printf("main()::Error creating shared memory object!");
            return 1;
        }

        // Set size of memory
        ftruncate(shm_fd, MEM_SIZE);

        // Create map shared memory
        void* ptr = mmap(0, MEM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

        // If map shared memory failed to create, exit...
        if (ptr == MAP_FAILED) {
            printf("main()::Error mapping shared memory!\n");
            return 1;
        }

        // Create buffer and set to empty
        char buffer[MEM_SIZE];
        buffer[0] = '\0';
        
        // Create index and save first number to buffer
        int index = 0;
        index += sprintf(index + buffer, "%d", number);

        while (number != 1) {
            if (number % 2 == 0) {
                number /= 2;
            } else {
                number = 3 * number + 1;
            }

            // Save number to buffer
            index += sprintf(index + buffer, ", %d", number);
        }
        index += sprintf(buffer + index, "\n"); // Add '\n' to buffer after sequence found

        // Save the buffer to the shared memory and exit the process
        sprintf(ptr, "%s", buffer);
        exit(0);
    } else if (pid > 0) { // Inside the parent process...
        // Wait for the child process to finish finding and saving sequence to buffer
        wait(NULL);

        // Open shared memory object
        int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);

        // If it fails to open, exit...
        if (shm_fd == -1) {
            printf("Parent::Error opening shared memory!\n");
            return 1;
        }

        // Map shared memory
        void* ptr = mmap(0, MEM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

        // If shared memory fails to map, exit...
        if (ptr == MAP_FAILED) {
            printf("Parent::Error mapping shared memory!\n");
            return 1;
        }

        // Print the sequence from the shared memory map
        printf("Sequence from memory: %s\n", (char*)ptr);

        // Unlink the shared memory
        shm_unlink(SHM_NAME);
    }

    return 0;
}