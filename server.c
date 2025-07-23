#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#define FIFO_PATH "/tmp/server_fifo"
#define MAX_CLIENTS 5

typedef struct {
    int id;
    int visible;
    char fifo_name[50];} 
Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void list_clients(int client_id);
void hide_client(int client_id);
void unhide_client(int client_id);
void remove_client(int client_id);

int main() {
    mkfifo(FIFO_PATH, 0666);
    printf("I am the server's main thread.  Waiting for client requests...\n");
     
    while (1) {
        int server_fd = open(FIFO_PATH, O_RDONLY);
        if (server_fd == -1) {
            perror("Server: Failed to connect");
            exit(1);
        }
        char client_fifo[50];
        read(server_fd, client_fifo, sizeof(client_fifo));
        close(server_fd);

        pthread_mutex_lock(&lock);
        strcpy(clients[client_count].fifo_name, client_fifo);
        clients[client_count].id = client_count + 1;
        clients[client_count].visible = 1;
        printf("Received REGISTER command from Client %d.\n", clients[client_count].id);
        printf("Successfully registered Client %d to the client list.\n", clients[client_count].id);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &clients[client_count]);
        client_count++;
        pthread_mutex_unlock(&lock);
    }
    unlink(FIFO_PATH);
    return 0;
}

void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    int client_fd;
    char command[256];

    while (1) {
        client_fd = open(client->fifo_name, O_RDONLY);
        read(client_fd, command, sizeof(command));
        close(client_fd);
        printf("Received command from Client %d: %s\n", client->id, command);

        if (strncmp(command, "CHPT", 4) == 0) {
            printf("Client %d changed prompt to: %s\n", client->id, command + 5);
        } else if (strncmp(command, "REGISTER", 4) == 0){
            printf("Client %d is already registered.\n", client->id);
        } else if (strncmp(command, "LIST", 4) == 0) {
	    printf("Client %d requested to LIST all visible clients.\n", client->id);
            list_clients(client->id);
        } else if (strncmp(command, "HIDE", 4) == 0) {
            hide_client(client->id);
        } else if (strncmp(command, "UNHIDE", 6) == 0) {
            unhide_client(client->id);
        } else if (strncmp(command, "EXIT", 4) == 0) {
            printf("Gracefully exiting...\n");
            printf("EXIT complete.\n");
            remove_client(client->id);
            pthread_exit(NULL);
        } else if (strncmp(command, "SHUTDOWN", 8) == 0) {
            printf("Gracefully shutting down...\n");
            printf("SHUTDOWN complete.\n");
            remove_client(client->id);
            pthread_exit(NULL);
        } else {
            printf("Received invalid command from Client %d \n", client->id);
        }
    }
}
void list_clients(int client_id) {
    char response[256] = "Currently visible Clients: \n";
    int visible_count = 0;
    pthread_mutex_lock(&lock);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].visible) {
            char temp[20];
            sprintf(temp, "Client %d, ", clients[i].id);
            strcat(response, temp);
            visible_count++;
        }
    }
    pthread_mutex_unlock(&lock);
    if (visible_count == 0) {
        strcpy(response, "All clients are hidden.");
    } else {
        response[strlen(response) - 2] = '\0';
    }
    int fd = open(clients[client_id - 1].fifo_name, O_WRONLY);
    write(fd, response, strlen(response) + 1);
    close(fd);
}
void hide_client(int client_id) {
    clients[client_id - 1].visible = 0;
    printf("Client %d is now hidden.\n", client_id);
}
void unhide_client(int client_id) {
    clients[client_id - 1].visible = 1;
    printf("Client %d is now visible.\n", client_id);
}
void remove_client(int client_id) {
    printf("Client %d disconnected successfully. Type CTRL+C to disconnect server.\n", client_id);
    clients[client_id - 1].id = -1;
}