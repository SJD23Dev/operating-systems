#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_STRING_SIZE 50
#define RED "\x1b[31m" // Used for error messages
#define GREEN "\x1b[32m" // Used to denote events in the child
#define BLUE "\x1b[34m" // Used to denote events in the parent
#define YELLOW "\x1b[33m" // Used for limit warning when collecting string
#define RESET "\x1b[0m" // Used to remove color

int main(int argc, char* argv[]) {
    // Collect string from the user
    char string[MAX_STRING_SIZE];
    printf("\n<Enter a message, please> " YELLOW "[50 character limit!] " RESET);
    fgets(string, MAX_STRING_SIZE, stdin);

    int parent_to_child[2], child_to_parent[2]; // Int arrays to control "ends" (read/write) of pipes

    if (pipe(parent_to_child) == -1) { // If pipe fails to create, exit
        printf(RED "\n17, main()::Failed to call pipe(parent_to_child)" RESET);
        return 1;
    }

    if (pipe(child_to_parent) == -1) { // If pipe fails to create, exit
        printf(RED "\n22, main()::Failed to call pipe(child_to_parent)" RESET);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) { // If fork() returns with a negative value, exit
        printf(RED "\n34, main()::Fork() returned less than 0" RESET);
        return 1;
    } else if (pid == 0) { // Inside the child process...
        close(parent_to_child[1]); // Closing: write from parent -> child
        close(child_to_parent[0]); // Closing: read from child -> parent

        // Read the string from the parent and close the pipe end
        char receivedString[MAX_STRING_SIZE];
        read(parent_to_child[0], receivedString, MAX_STRING_SIZE);
        close(parent_to_child[0]); // Closing : read from parent -> child
        printf(GREEN "<Child, read, parent_to_child> " RESET "%s", receivedString);

        // Flip characters in the string
        for (int i = 0; i < strlen(receivedString); i++) {
            if (isupper(receivedString[i]))
                receivedString[i] = tolower(receivedString[i]);
            else if (islower(receivedString[i]))
                receivedString[i] = toupper(receivedString[i]);
        }

        // 
        write(child_to_parent[1], receivedString, strlen(receivedString) + 1);
        printf(GREEN "<Child, write, child_to_parent> " RESET "%s", receivedString);
        close(child_to_parent[1]); // Close: write child -> parent
    } else if (pid > 0) { // Parent process
        close(parent_to_child[0]); // Closing: read from parent -> child
        close(child_to_parent[1]); // Closing: write from child -> parent

        // Write string to child and close the pipe end
        write(parent_to_child[1], string, strlen(string) + 1);
        close(parent_to_child[1]); // Closing: write from parent -> child
        printf(BLUE "<Parent, write, parent_to_child> " RESET "%s", string);

        wait(NULL);

        // Read the string from the child and close the pipe end
        char problemSolution[MAX_STRING_SIZE];
        read(child_to_parent[0], problemSolution, MAX_STRING_SIZE);
        close(child_to_parent[0]); // Closing: read from child -> parent
        printf(BLUE "<Parent, read, child_to_parent> " RESET "%s", problemSolution);
    }

    return 0;
}