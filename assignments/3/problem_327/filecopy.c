#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

int main(int argc, char* argv[]) {
    // Error checking for passed-in arguments
    if (argc != 3) {
        printf(RED "\n24, main()::Invalid arguments\nCorrect example: ./run.sh input.txt copy.txt\n" RESET);
        return 1;
    }

    // Essentially faucet valves for both the pipe's ends. Literally, an array of integers
    int parent_to_child[2], child_to_parent[2]; // [0] = read end | [1] = write end

    if (pipe(parent_to_child) == -1) { // If pipe fails to create, exit
        printf(RED "\n33, main()::Failed to call pipe(parent_to_child)\n" RESET);
        return 1;
    }

    if (pipe(child_to_parent) == -1) { // If pipe fails to create, exit
        printf(RED "\n38, main()::Failed to call pipe(child_to_parent)\n" RESET);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) { // If fork() returns with a negative value, exit
        printf(RED "\n45, main()::Fork() returned less than 0\n" RESET);
        return 1;
    } else if (pid == 0) { // Inside the child process...
        close(parent_to_child[1]); // Closing: write from parent -> child
        close(child_to_parent[0]); // Closing: read from child -> parent
        close(child_to_parent[1]); // Closing: write from child -> parent

        FILE* outputFile = fopen(argv[2], "w");
        if (outputFile == NULL) {
            printf(RED "\n54, main()::Output file could not be created\n" RESET);
            return 1;
        }

        // See the how the parent process sent the data (line 77~) to understand
        // the purpose of the variables and how the while loop works
        char buffer[1024];
        ssize_t bytes_read;
        while((bytes_read = read(parent_to_child[0], buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes_read, outputFile); // fwrite() behaves similarly to fread()
        }
        printf(GREEN "<Child, finished writing contents to output file>\n" RESET);

        close(parent_to_child[0]); // Closing: read from parent -> child
    } else if (pid > 0) { // Inside the parent process...
        close(child_to_parent[0]); // Closing: read from child -> parent
        close(child_to_parent[1]); // Closing: write from child -> parent
        close(parent_to_child[0]); // Closing: read from parent -> child

        FILE* inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            printf(RED "\n71, main()::Input file could not be opened\n" RESET);
            return 1;
        }

        char buffer[1024]; // Buffer to store data (1024 bytes)
        size_t bytes_read; // Tracks the amount of bytes actually read each loop iteration
        while((bytes_read = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
            // fread(): Reads up to the buffer size from the file
            //          Returns number of bytes read
            //          - buffer: where to store the data
            //          - 1: Amount of bytes to read at once
            //          - sizeof(buffer): The maximum number of elements to read
            //
            // Finally, the loop ends when bytes_read = 0, indicating there
            // is no more data to read from the file
            write(parent_to_child[1], buffer, bytes_read);
        }
        printf(BLUE "<Parent, finished sending file contents to child>\n" RESET);

        close(parent_to_child[1]); // Closing: Write from parent -> child

        wait(NULL);
    }

    return 0;
}