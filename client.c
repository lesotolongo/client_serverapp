#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO "/tmp/server_fifo"

char client_fifo[50];
char prompt[50] = ">> ";

int main() {
    char client_fifo[50];
    sprintf(client_fifo, "/tmp/client_fifo_%d", getpid());
    mkfifo(client_fifo, 0666);

    int server_fd = open(SERVER_FIFO, O_WRONLY);
    write(server_fd, client_fifo, sizeof(client_fifo));
    close(server_fd);
    printf("I am the Client's main thread. My parent process is the Server.\n"
    "Client initialized.  Enter commands (type EXIT to quit).\n");

    while (1) {
        char command[256];
        printf("%s", prompt);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        int client_fd = open(client_fifo, O_WRONLY);
        write(client_fd, command, strlen(command) + 1);
        close(client_fd);

        if (strncmp(command, "CHPT", 4) == 0) {
            snprintf(prompt, sizeof(prompt), "%s ", command + 5);
            printf("Prompt changed to: [%s]\n", command + 5);
            continue;
        }
	    else if (strncmp(command, "REGISTER", 4)==0) {
            printf("You are already registered on the client list.\n");
        }
        else if (strncmp(command, "HIDE", 4)==0) {
            printf("You are now hidden from list...\n");
        }
        else if (strncmp(command, "UNHIDE", 4)==0) {
            printf("You are now visible in the list...\n");
        }
        else if (strncmp(command, "LIST", 4) == 0) {
            client_fd = open(client_fifo, O_RDONLY);
            char response[256];
            read(client_fd, response, sizeof(response));
            close(client_fd);
            printf("%s\n", response);
        }
        else if (strncmp(command, "EXIT", 4) == 0 || strncmp(command, "SHUTDOWN", 4)==0){
            printf("Exiting gracefully...\n"
            "Cleaning up resources...\n"
            "Shutdown complete.\n");
            break;
        }
        else {
            printf("Invalid input. Please use a valid command or type EXIT to quit.\n");
        }
    }
    unlink(client_fifo);
    return 0;
}